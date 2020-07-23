#define Uses_THardwareInfo
#include <tvision/tv.h>

#include <internal/buffdisp.h>
#include <internal/codepage.h>
#include <internal/getenv.h>
#include <internal/cursor.h>
#include <chrono>
using std::chrono::microseconds;
using std::chrono::steady_clock;

BufferedDisplay *BufferedDisplay::instance = 0;
std::set<ScreenCursor*> BufferedDisplay::cursors;

BufferedDisplay::BufferedDisplay() :
    widePlaceholder(THardwareInfo::isLinuxConsole(1) ? ' ' : '\0')
{
    instance = this;
}

BufferedDisplay::~BufferedDisplay()
{
    instance = 0;
}

void BufferedDisplay::init()
{
    // Check if FPS shall be limited.
    int fps = getEnv<int>("TVISION_MAX_FPS", defaultFPS);
    limitFPS = (fps > 0);
    if (limitFPS)
        flushDelay = microseconds((int) 1e6/fps);
    // Initialize variables.
    screenChanged = true;
    caretMoved = false;
    caretPosition = {-1, -1};
    resetBuffer();
}

void BufferedDisplay::resetBuffer()
{
    int rows = getScreenRows(), cols = getScreenCols();

    size = {cols, rows};
    buffer.resize(rows*cols);
    rowDamage.resize(rows);

    for (int i = 0; i < rows; ++i) {
        rowDamage[i] = {INT_MAX, INT_MIN};
        for (int j = 0; j < cols; ++j) {
            buffer[i*size.x + j] = {};
        }
    }
}

void BufferedDisplay::setCaretPosition(int x, int y)
{
    caretPosition = {x, y};
    caretMoved = true;
}

void BufferedDisplay::screenWrite(int x, int y, TScreenCell *buf, int len)
{
    auto damage = rowDamage[y];
    const auto [cols, rows] = size;
    for (int i = 0; i < len; i++, x++)
    {
        BufferCell newCell {buf[i]};
        ensurePrintable(newCell);
        auto bufCell = buffer[y*cols + x];
        if (newCell != bufCell)
        {
            screenChanged = true;
            setDirty(x, newCell, damage);
            buffer[y*cols + x] = newCell;
        }
    }
    rowDamage[y] = damage;
}

void BufferedDisplay::setDirty(int x, BufferCell &cell, Range &damage)
{
    cell.Cell.dirty = 1;
    Range dam = damage;
    if (x < dam.begin)
        dam.begin = x;
    if (x > dam.end)
        dam.end = x;
    damage = dam;
}

bool BufferedDisplay::timeToFlush()
{
    // Avoid flushing faster than the maximum FPS.
    if (limitFPS)
    {
        auto now = steady_clock::now();
        if (frameDrops == maxFrameDrops || ((now - lastFlush) >= flushDelay))
        {
            lastFlush = now;
            frameDrops = 0;
        }
        else
        {
            ++frameDrops;
            return false;
        }
    }
    return true;
}

void BufferedDisplay::drawCursors()
{
    const auto [cols, rows] = size;
    for (auto* cursor : cursors)
        if (cursor->isVisible()) {
            const auto [x, y] = cursor->getPos();
            auto &cell = buffer[y*cols + x];
            cursor->apply(cell.Cell.Attr);
            setDirty(x, cell, rowDamage[y]);
        }
}

void BufferedDisplay::undrawCursors()
{
    const auto [cols, rows] = size;
    for (const auto* cursor : cursors)
        if (cursor->isVisible()) {
            const auto [x, y] = cursor->getPos();
            auto &cell = buffer[y*cols + x];
            cursor->restore(cell.Cell.Attr);
            setDirty(x, cell, rowDamage[y]);
        }
}

void BufferedDisplay::flushScreen()
{
    if ((screenChanged || caretMoved) && timeToFlush())
    {
        drawCursors();
        FlushScreenAlgorithm(*this).run();
        if (caretPosition.x != -1)
            lowlevelMoveCursor(caretPosition.x, caretPosition.y);
        undrawCursors();
        screenChanged = false;
        caretMoved = false;
        lowlevelFlush();
    }
}

void BufferedDisplay::onScreenResize()
{
    resetBuffer();
}


void BufferedDisplay::ensurePrintable(BufferCell &cell) const
{
    uint &ch = cell.Cell.Char.asInt;
    if (ch == '\0')
        ch = ' ';
    else if (ch < ' ' || (0x7F <= ch && ch < 0x100)) {
        // Translate from codepage as fallback.
        ch = CpTranslator::toUtf8Int(ch);
    } else if (ch == TScreenCell::wideCharTrail) {
        ch = widePlaceholder;
        cell.Cell.extraWidth = 0;
    }
}

void FlushScreenAlgorithm::run()
{
    size = disp.size;
    last = {-1, -1};
    for (y = 0; y < size.y; ++y)
    {
        damage = disp.rowDamage[y];
        newDamage = {INT_MAX, INT_MIN};
        for (x = damage.begin; x <= damage.end; ++x)
        {
            getCell();
            if (cell.dirty) {
                pCell->dirty = 0;
                if (wideCanSpill()) {
                    if (__builtin_expect(cell.extraWidth, 0)) {
                        handleWideCharSpill();
                        continue;
                    } else if (__builtin_expect(cell.Char.asInt == '\0', 0)) {
                        handleNull();
                        continue;
                    }
                }
                writeCell();
            }
        }
        if (wideCanSpill() && x < size.x) {
            getCell();
            if (__builtin_expect(cell.Char.asInt == '\0', 0))
                handleNull();
        }
        disp.rowDamage[y] = newDamage;
    }
}

void FlushScreenAlgorithm::writeCell()
{
//     Workaround for Ncurses bug
//     if (y != last.y)
//         lowlevelFlush();
    if (y != last.y)
        disp.lowlevelMoveCursor(x, y);
    else if (x != last.x + 1)
        disp.lowlevelMoveCursorX(x, y);

    disp.lowlevelWriteChars(cell.Char.bytes, cell.Attr);
    last = {x, y};
}

void FlushScreenAlgorithm::handleWideCharSpill()
{
    uchar width = cell.extraWidth;
    if (x + width < size.x)
        writeCell();
    else {
        // Replace with spaces if it would otherwise be printed on the next line.
        cell.Char.asInt = ' ';
        writeCell();
        while (width-- && ++x < size.x) {
            getCell();
            if (cell.Char.asInt != '\0') {
                --x;
                return;
            }
            cell.Char.asInt = ' ';
            writeCell();
        }
    }
    // Check character does not spill on next cells.
    while (width-- && ++x < size.x) {
        getCell();
        pCell->dirty = 0;
        if (cell.Char.asInt != '\0') {
            disp.lowlevelMoveCursorX(x, y);
            writeCell();
            return;
        }
    }
    // Otherwise, skip placeholders. x now points to the last column occupied
    // by the wide character in the screen.
    last = {x, y};
    if (x + 1 < size.x) {
        // Anyway, print the next character just in case it is not dirty
        // to avoid attribute spill.
        ++x;
        getCell();
        pCell->dirty = 0;
        writeCell();
    }
}

void FlushScreenAlgorithm::handleNull()
{
    // Having '\0' in a cell implies wide characters can spill, as '\0'
    // is otherwise discarded in ensurePrintable().
    const auto Attr = cell.Attr;
    if (x > 0) {
        --x;
        getCell();
        // Check the character behind the placeholder.
        if (cell.extraWidth) {
            handleWideCharSpill();
            return;
        }
        ++x;
        getCell();
    }
    // Print successive placeholders as spaces.
    do {
        cell.Char.asInt = ' ';
        writeCell();
        ++x;
        if (x < size.x) {
            getCell();
            pCell->dirty = 0;
        } else
            return;
    } while (cell.Char.asInt == '\0');
    // We now got a normal character.
    if (x < damage.end) {
        // Decrease for next iteration
        --x;
    } else if (Attr != cell.Attr) {
        // Redraw a character that would otherwise not be printed,
        // to prevent attribute spill.
        writeCell();
    }
}


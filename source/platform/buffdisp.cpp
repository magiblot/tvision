#define Uses_TScreenCell
#include <tvision/tv.h>

#include <internal/buffdisp.h>
#include <internal/codepage.h>
#include <internal/getenv.h>
#include <internal/cursor.h>
#include <chrono>
using std::chrono::microseconds;
using std::chrono::steady_clock;

#ifdef _MSC_VER
#define __builtin_expect(x, y) x
#endif

BufferedDisplay *BufferedDisplay::instance = 0;

BufferedDisplay::BufferedDisplay() noexcept :
    // This could be checked at runtime, but for now this is as much as I know.
    // Replace with space if terminal treats all characters one column wide.
    widePlaceholder(TCellChar::wideCharTrail)
#ifdef _WIN32
    , wideOverlapping(false)
#else
    , wideOverlapping(true)
#endif
    , flushDelay()
    , lastFlush()
{
    instance = this;
    // Check if FPS shall be limited.
    int fps = getEnv<int>("TVISION_MAX_FPS", defaultFPS);
    limitFPS = (fps > 0);
    if (limitFPS)
        flushDelay = microseconds((int) 1e6/fps);
    // Initialize variables.
    screenTouched = true;
    caretMoved = false;
    caretPosition = {-1, -1};
    newCaretSize = 0;
}

BufferedDisplay::~BufferedDisplay()
{
    instance = 0;
}

void BufferedDisplay::reloadScreenInfo() noexcept
{
    DisplayStrategy::reloadScreenInfo();
    resizeBuffer();
}

void BufferedDisplay::resizeBuffer() noexcept
{
    for (auto *buf : {&buffer, &flushBuffer})
    {
        buf->resize(0);
        buf->resize(size.x*size.y); // Zero-initialized.
    }

    rowDamage.resize(0);
    rowDamage.resize(size.y, {INT_MAX, INT_MIN});
}

void BufferedDisplay::setCaretSize(int size) noexcept
{
    newCaretSize = size;
}

void BufferedDisplay::setCaretPosition(int x, int y) noexcept
{
    caretPosition = {x, y};
    caretMoved = true;
}

void BufferedDisplay::screenWrite(int x, int y, TScreenCell *buf, int len) noexcept
{
    if (inBounds(x, y) && len)
    {
        len = min(len, size.x - x);
        memcpy(&buffer[y*size.x + x], buf, len*sizeof(TScreenCell));

        setDirty(x, y, len);
        screenTouched = true;
    }
}

void BufferedDisplay::setDirty(int x, int y, int len) noexcept
{
    Range dam = rowDamage[y];
    if (x < dam.begin)
        dam.begin = x;
    if (x + len - 1 > dam.end)
        dam.end = x + len - 1;
    rowDamage[y] = dam;
}

bool BufferedDisplay::timeToFlush() noexcept
{
    // Avoid flushing faster than the maximum FPS.
    if (limitFPS)
    {
        auto now = steady_clock::now();
        if (now - lastFlush >= flushDelay)
            lastFlush = now;
        else
            return false;
    }
    return true;
}

void BufferedDisplay::drawCursors() noexcept
{
    for (auto* cursor : cursors)
        if (cursor->isVisible())
        {
            auto pos = cursor->getPos();
            auto x = pos.x, y = pos.y;
            if (inBounds(x, y))
            {
                auto *cell = &buffer[y*size.x + x];
                if ( cell->ch.isWideCharTrail() &&
                     x > 0 && (cell - 1)->wide )
                    --cell, --x;
                cursor->apply(cell->attr);
                setDirty(x, y, 1);
            }
        }
}

void BufferedDisplay::undrawCursors() noexcept
{
    for (const auto* cursor : cursors)
        if (cursor->isVisible())
        {
            auto pos = cursor->getPos();
            auto x = pos.x, y = pos.y;
            if (inBounds(x, y))
            {
                auto *cell = &buffer[y*size.x + x];
                if ( cell->ch.isWideCharTrail() &&
                     x > 0 && (cell - 1)->wide )
                    --cell, --x;
                cursor->restore(cell->attr);
                setDirty(x, y, 1);
            }
        }
}

bool BufferedDisplay::needsFlush() const noexcept
{
    return screenTouched || caretMoved || caretSize != newCaretSize;
}

namespace buffdisp {
namespace {

    void flushScreenAlgorithm(BufferedDisplay &) noexcept;

}
}

void BufferedDisplay::flushScreen() noexcept
{
    using namespace buffdisp;
    if (needsFlush() && timeToFlush())
    {
        drawCursors();
        flushScreenAlgorithm(*this);
        if (caretPosition.x != -1)
            lowlevelMoveCursor(caretPosition.x, caretPosition.y);
        undrawCursors();
        if (caretSize != newCaretSize)
            lowlevelCursorSize(newCaretSize);
        lowlevelFlush();
        screenTouched = false;
        caretMoved = false;
        caretSize = newCaretSize;
    }
}

inline void BufferedDisplay::validateCell(TScreenCell &cell) const noexcept
{
    auto &ch = cell.ch;
    if (!ch[1]) // size 1
    {
        uchar c = ch[0];
        if (c == '\0')
            ch[0] = ' ';
        else if (c < ' ' || 0x7F <= c)
            // Translate from codepage as fallback.
            ch = CpTranslator::toUtf8Int(c);
    }
    else if (ch.isWideCharTrail())
        ch = widePlaceholder;
}

//////////////////////////////////////////////////////////////////////////
// FlushScreenAlgorithm

namespace buffdisp {
namespace {

struct FlushScreenAlgorithm
{

    BufferedDisplay &disp;
    TPoint size;
    TPoint last;
    int x, y, rowOffs;
    TScreenCell *cell;
    BufferedDisplay::Range damage;

    FlushScreenAlgorithm(BufferedDisplay &disp) noexcept :
        disp(disp)
    {
    }

    const TScreenCell &cellAt(int x) const noexcept;
    void getCell() noexcept;
    bool cellDirty() const noexcept;
    bool wideCanSpill() const noexcept;
    bool wideCanOverlap() const noexcept;

    void run() noexcept;
    void processCell() noexcept;
    void writeCell() noexcept;
    void writeSpace() noexcept;
    void writeCell(const TCellChar &Char, const TColorAttr &Attr, bool wide) noexcept;
    void commitDirty() noexcept;
    void handleWideCharSpill() noexcept;
    void handleTrail() noexcept;

};

inline bool isTrail(const TScreenCell &cell) noexcept
{
    return __builtin_expect(cell.ch.isWideCharTrail(), 0);
}

inline bool isWide(const TScreenCell &cell) noexcept
{
    return __builtin_expect(cell.wide, 0);
}

inline const TScreenCell& FlushScreenAlgorithm::cellAt(int x) const noexcept
{
    return disp.buffer[rowOffs + x];
}

inline void FlushScreenAlgorithm::getCell() noexcept
{
    cell = &disp.buffer[rowOffs + x];
    disp.validateCell(*cell);
}

inline bool FlushScreenAlgorithm::cellDirty() const noexcept
{
    return *cell != disp.flushBuffer[cell - &disp.buffer[0]];
}

inline void FlushScreenAlgorithm::commitDirty() noexcept
{
    disp.flushBuffer[cell - &disp.buffer[0]] = *cell;
}

inline bool FlushScreenAlgorithm::wideCanSpill() const noexcept
{
    return disp.widePlaceholder == TCellChar::wideCharTrail;
}

inline bool FlushScreenAlgorithm::wideCanOverlap() const noexcept
{
    return disp.wideOverlapping;
}

inline void flushScreenAlgorithm(BufferedDisplay &disp) noexcept
{
    FlushScreenAlgorithm(disp).run();
}

inline void FlushScreenAlgorithm::run() noexcept
{
    size = disp.size;
    last = {INT_MIN, INT_MIN};
    rowOffs = 0;
    for (y = 0; y < size.y; ++y, rowOffs += size.x)
    {
        damage = disp.rowDamage[y];
        if (damage.begin <= damage.end)
        {
            x = damage.begin;
            // Check for a wide character before the first draw position
            // if overlapping is not allowed.
            bool wideSpillBefore = wideCanSpill() && !wideCanOverlap() && 0 < x && isWide(cellAt(x-1));
            for (; x <= damage.end; ++x)
            {
                getCell();
                if (cellDirty() || isTrail(*cell)) {
                    if (wideSpillBefore) {
                        --x;
                        getCell();
                        wideSpillBefore = false;
                    }
                    commitDirty();
                    processCell();
                } else
                    wideSpillBefore = wideCanSpill() && !wideCanOverlap() && isWide(*cell);
            }
            if (x < size.x) {
                getCell();
                if (isTrail(*cell))
                    // The beginning of a wide character has been overwritten
                    // and attribute spill may happen in the remaining trails.
                    handleTrail();
            }
        }
        disp.rowDamage[y] = {INT_MAX, INT_MIN};
    }
}

inline void FlushScreenAlgorithm::processCell() noexcept
{
    if (wideCanSpill()) {
        if (isWide(*cell)) {
            handleWideCharSpill();
            return;
        } else if (isTrail(*cell)) {
            handleTrail();
            return;
        }
    }
    writeCell();
}

inline void FlushScreenAlgorithm::writeCell() noexcept
{
    writeCell(cell->ch, cell->attr, cell->wide);
}

inline void FlushScreenAlgorithm::writeSpace() noexcept
{
    writeCell(' ', cell->attr, 0);
}

inline void FlushScreenAlgorithm::writeCell( const TCellChar &Char,
                                             const TColorAttr &Attr,
                                             bool wide ) noexcept
{
    // 'last' is the last written cell occupied by text.
    // That is, the hardware cursor is located at {last.x + 1, last.y}.

    if (y != last.y)
        disp.lowlevelMoveCursor(x, y);
    else if (x != last.x + 1)
        disp.lowlevelMoveCursorX(x, y);

    disp.lowlevelWriteChars(Char.getText(), Attr);
    last = {x + wide, y};
}

void FlushScreenAlgorithm::handleWideCharSpill() noexcept
{
    uchar width = cell->wide;
    const auto Attr = cell->attr;
    if (x + width < size.x)
        writeCell();
    else {
        // Replace with spaces if it would otherwise be printed on the next line.
        writeSpace();
        while (--width && ++x < size.x) {
            getCell();
            if (!isTrail(*cell)) {
                --x;
                return;
            }
            writeSpace();
        }
        return;
    }
    // Ensure character does not spill on next cells.
    int wbegin = x;
    while (width-- && ++x < size.x) {
        getCell();
        commitDirty();
        if (!isTrail(*cell)) {
            if (wideCanOverlap()) {
                // Write over the wide character.
                writeCell();
            } else {
                // Replace the whole wide character with spaces.
                int wend = x;
                x = wbegin;
                do {
                    getCell();
                    writeSpace();
                } while (++x < wend);
                getCell();
                writeCell();
            }
            return;
        }
    }
    // If the next character has a different attribute, print it anyway
    // to avoid attribute spill.
    if (x + 1 < size.x) {
        ++x;
        getCell();
        if (Attr != cell->attr) {
            commitDirty();
            processCell();
        } else
            --x;
    }
}

void FlushScreenAlgorithm::handleTrail() noexcept
{
    // Having TCellChar::wideCharTrail in a cell implies wide characters
    // can spill, as the value is otherwise discarded in ensurePrintable().
    const auto Attr = cell->attr;
    if (x > 0) {
        --x;
        getCell();
        // Check the character behind the placeholder.
        if (cell->wide) {
            handleWideCharSpill();
            return;
        }
        ++x;
        getCell();
    }
    // Print successive placeholders as spaces.
    do {
        commitDirty();
        writeSpace();
        ++x;
        if (x < size.x)
            getCell();
        else
            return;
    } while (isTrail(*cell));
    // We now got a normal character.
    if (x > damage.end && Attr != cell->attr) {
        // Redraw a character that would otherwise not be printed,
        // to prevent attribute spill.
        processCell();
    } else {
        // Decrease for next iteration
        --x;
    }

}

} // namespace
} // namespace buffdisp

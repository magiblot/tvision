#include <internal/buffdisp.h>
#include <internal/codepage.h>
#include <internal/getenv.h>
#include <internal/cursor.h>
#include <chrono>
using std::chrono::microseconds;
using std::chrono::steady_clock;

BufferedDisplay *BufferedDisplay::instance = 0;
std::set<ScreenCursor*> BufferedDisplay::cursors;

BufferedDisplay::BufferedDisplay()
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

    buffer.~Array2D();
    new (&buffer) Array2D<BufferCharInfo>(rows, cols, 0);

    rowDamage.~vector();
    new (&rowDamage) std::vector<Range>(rows, {INT_MAX, INT_MIN});
}

void BufferedDisplay::setCaretPosition(int x, int y)
{
    caretPosition = {x, y};
    caretMoved = true;
}

void BufferedDisplay::screenWrite( int x, int y, ushort *buf, int len )
{
    auto damage = rowDamage[y];
    for (int i = 0; i < len; i++, x++)
    {
        auto binfo = reinterpret_cast<BufferCharInfo*>(buf)[i];
        auto &cinfo = buffer[y][x];
        if (binfo.character == '\0')
            binfo.character = ' '; // Treat null character as a space.
        if (cinfo != binfo)
        {
            screenChanged = true;
            setDirty(x, binfo, damage);
            cinfo = binfo;
        }
    }
    rowDamage[y] = damage;
}

void BufferedDisplay::setDirty(int x, BufferCharInfo &cinfo, Range &damage)
{
    cinfo.dirty = 1;
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
    bool b = true;
    if (limitFPS)
    {
        auto now = steady_clock::now();
        if ((b = ((now - lastFlush) >= flushDelay)))
            lastFlush = now;
    }
    return b;
}

void BufferedDisplay::drawCursors()
{
    for (auto* cursor : cursors)
        if (cursor->isVisible()) {
            const auto [x, y] = cursor->getPos();
            auto &cinfo = buffer[y][x];
            cursor->apply(cinfo.attr);
            setDirty(x, cinfo, rowDamage[y]);
        }
}

void BufferedDisplay::undrawCursors()
{
    for (const auto* cursor : cursors)
        if (cursor->isVisible()) {
            const auto [x, y] = cursor->getPos();
            auto &cinfo = buffer[y][x];
            cursor->restore(cinfo.attr);
            setDirty(x, cinfo, rowDamage[y]);
        }
}

std::string_view BufferedDisplay::translateChar(char c)
{
    return CpTranslator::toUtf8(c);
}

void BufferedDisplay::flushScreen()
{
    if ((screenChanged || caretMoved) && timeToFlush())
    {
        drawCursors();
        TPoint last = {-1, -1};
        for (int y = 0; y < int(rowDamage.size()); ++y)
        {
            auto &damage = rowDamage[y];
            for (int x = damage.begin; x <= damage.end; ++x)
            {
                auto &cinfo = buffer[y][x];
                if (cinfo.dirty)
                {
//                     Workaround for Ncurses bug
//                     if (y != last.y)
//                         lowlevelFlush();
                    if (y != last.y || x != last.x + 1)
                        lowlevelMoveCursor(x, y);
                    lowlevelWriteChars(translateChar(cinfo.character), cinfo.attr);
                    last = {x, y};
                    cinfo.dirty = 0;
                }
            }
            damage = {INT_MAX, INT_MIN};
        }
        if (caretPosition.x != -1)
            lowlevelMoveCursor(caretPosition.x, caretPosition.y);
        lowlevelFlush();
        screenChanged = false;
        caretMoved = false;
        undrawCursors();
    }
}

void BufferedDisplay::onScreenResize()
{
    resetBuffer();
}


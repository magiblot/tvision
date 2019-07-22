#include <internal/buffdisp.h>
#include <internal/getenv.h>
#include <chrono>
using std::chrono::microseconds;
using std::chrono::steady_clock;
using std::chrono::time_point;

void BufferedDisplay::initBuffer()
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
    // Allocate buffer.
    int rows = getScreenRows(), cols = getScreenCols();
    charBuffer = Array2D<uchar>(rows, cols, 0);
    attrBuffer = Array2D<ushort>(rows, cols, 0);
}

void BufferedDisplay::setCaretPosition(int x, int y)
{
    caretPosition = {y, x};
    caretMoved = true;
}

void BufferedDisplay::screenWrite( int x, int y, ushort *buf, int len )
{
    for (int i = 0; i < len; i++, x++)
    {
        uchar character = buf[i*2];
        ushort attr = buf[i*2 + 1];
        if (character != charBuffer[y][x] || attr != attrBuffer[y][x])
        {
            screenChanged = true;
            changedCells.insert({y, x});
            charBuffer[y][x] = character;
            attrBuffer[y][x] = attr;
        }
    }
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

void BufferedDisplay::flushScreen()
{
    if ((screenChanged || caretMoved) && timeToFlush())
    {
        CellPos last = {-1, -1};
        for (auto [y, x] : changedCells)
        {
            if (y != last.y || x != last.x + 1)
                lowlevelMoveCursor(x, y);
            lowlevelWriteChar(charBuffer[y][x], attrBuffer[y][x]);
            last = {y, x};
        }
        if (caretPosition.x != -1)
            lowlevelMoveCursor(caretPosition.x, caretPosition.y);
        lowlevelFlush();
        screenChanged = false;
        caretMoved = false;
        changedCells.clear();
    }
}

void BufferedDisplay::onScreenResize()
{
    int rows = getScreenRows(), cols = getScreenCols();
    charBuffer = Array2D<uchar>(rows, cols, 0);
    attrBuffer = Array2D<ushort>(rows, cols, 0);
    changedCells.clear();
}


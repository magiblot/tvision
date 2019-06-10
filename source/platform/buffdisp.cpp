#include <internal/buffdisp.h>
#include <utility>
using std::pair;

void BufferedDisplay::initBuffer()
{
    // Initialize variables.
    lastX = lastY = -1;
    needsFlush = true;
    cursorMoved = false;
    // Allocate buffer.
    int rows = getScreenRows(), cols = getScreenCols();
    charBuffer = Array2D<uchar>(rows, cols, 0);
    attrBuffer = Array2D<ushort>(rows, cols, 0);
}

void BufferedDisplay::setCaretPosition(int x, int y)
{
    lastX = x; lastY = y;
    cursorMoved = true;
}

void BufferedDisplay::screenWrite( int x, int y, ushort *buf, int len )
{
    for (int i = 0; i < len; i++, x++)
    {
        uchar character = buf[i*2];
        ushort attr = buf[i*2 + 1];
        if (character != charBuffer[y][x] || attr != attrBuffer[y][x])
        {
            needsFlush = true;
            changes.insert({y, x});
            charBuffer[y][x] = character;
            attrBuffer[y][x] = attr;
        }
    }
}

void BufferedDisplay::flushScreen()
{
    if (needsFlush || cursorMoved)
    {
        for (const pair<int, int> &pos : changes)
        {
            int x = pos.second, y = pos.first;
            lowlevelWriteChar(x, y, charBuffer[y][x], attrBuffer[y][x]);
        }
        if (lastX != -1)
            lowlevelMoveCursor(lastX, lastY);
        lowlevelFlush();
        cursorMoved = false;
        needsFlush = false;
        changes.clear();
    }
}


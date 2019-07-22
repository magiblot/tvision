#ifndef BUFFDISP_H
#define BUFFDISP_H

#include <internal/platform.h>
#include <internal/array2d.h>
#include <set>
#include <chrono>

class BufferedDisplay : public DisplayStrategy {

    struct CellPos {
        int y, x;  // row, col
        bool operator<(const CellPos& other) const { // Required by std::set
            return y == other.y ? x < other.x : y < other.y;
        }
    };

    Array2D<uchar> charBuffer;
    Array2D<ushort> attrBuffer;

    bool screenChanged;
    std::set<CellPos> changedCells;

    bool caretMoved;
    CellPos caretPosition;

    bool limitFPS;
    static const int defaultFPS = 60;
    std::chrono::microseconds flushDelay;
    std::chrono::time_point<std::chrono::steady_clock> lastFlush;
    bool timeToFlush();

protected:

    void initBuffer();

    void setCaretPosition(int x, int y);
    void screenWrite(int x, int y, ushort *buf, int len);
    void flushScreen();
    void onScreenResize();

    virtual void lowlevelWriteChar(uchar character, ushort attr) = 0;
    virtual void lowlevelMoveCursor(int x, int y) = 0;
    virtual void lowlevelFlush() = 0;

};

#endif

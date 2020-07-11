#ifndef BUFFDISP_H
#define BUFFDISP_H

#define Uses_TPoint
#include <tvision/tv.h>

#include <internal/platform.h>
#include <internal/textattr.h>
#include <internal/array2d.h>
#include <vector>
#include <set>
#include <chrono>

class ScreenCursor;

class BufferedDisplay : public DisplayStrategy {

    friend class ScreenCursor;

    struct Range {
        int begin, end;
    };

    Array2D<BufferCharInfo> buffer;
    std::vector<Range> rowDamage;
    bool screenChanged;

    bool caretMoved;
    TPoint caretPosition;

    bool limitFPS;
    std::chrono::microseconds flushDelay;
    std::chrono::time_point<std::chrono::steady_clock> lastFlush;

    static BufferedDisplay *instance;
    static std::set<ScreenCursor*> cursors;
    static constexpr int defaultFPS = 60;

    void resetBuffer();
    void setDirty(int x, BufferCharInfo &cinfo, Range &damage);

    void drawCursors();
    void undrawCursors();

    bool timeToFlush();

protected:

    BufferedDisplay();
    ~BufferedDisplay();

    void init();

    void setCaretPosition(int x, int y);
    void screenWrite(int x, int y, ushort *buf, int len);
    void flushScreen();
    void onScreenResize();

    virtual void lowlevelWriteChar(uchar character, ushort attr) = 0;
    virtual void lowlevelMoveCursor(uint x, uint y) = 0;
    virtual void lowlevelFlush() = 0;

};

#endif

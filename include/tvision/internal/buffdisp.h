#ifndef BUFFDISP_H
#define BUFFDISP_H

#define Uses_TPoint
#include <tvision/tv.h>

#include <internal/platform.h>
#include <internal/textattr.h>
#include <algorithm>
#include <vector>
#include <chrono>
#include <string_view>

class ScreenCursor;

class BufferedDisplay : public DisplayStrategy {

    friend struct FlushScreenAlgorithm;

    struct Range {
        int begin, end;
    };

    TPoint size;
    std::vector<BufferCell> buffer;
    std::vector<Range> rowDamage;
    bool screenChanged;

    const bool wideOverlapping;
    const uint widePlaceholder;
    bool caretMoved;
    TPoint caretPosition;

    bool limitFPS;
    int frameDrops;
    int maxFrameDrops;
    std::chrono::microseconds flushDelay;
    std::chrono::time_point<std::chrono::steady_clock> lastFlush;

    static BufferedDisplay *instance;
    static constexpr int defaultMaxFrameDrops = 4;
    static constexpr int defaultFPS = 60;

    void resetBuffer();
    void setDirty(int x, BufferCell &cell, Range &damage);
    void ensurePrintable(BufferCell &cell) const;

    std::vector<ScreenCursor*> cursors;
    void drawCursors();
    void undrawCursors();

    bool timeToFlush();

public:

    static void addCursor(ScreenCursor *cursor);
    static void removeCursor(ScreenCursor *cursor);
    static void changeCursor();

protected:

    BufferedDisplay();
    ~BufferedDisplay();

    void init();

    void setCaretPosition(int x, int y);
    void screenWrite(int x, int y, TScreenCell *buf, int len);
    void flushScreen();
    void onScreenResize();

    virtual void lowlevelWriteChars(const uchar chars[4], TCellAttribs attr) = 0;
    virtual void lowlevelMoveCursor(uint x, uint y) = 0;
    virtual void lowlevelMoveCursorX(uint x, uint y) { lowlevelMoveCursor(x, y); }
    virtual void lowlevelMoveCursorYby1(uint x, uint y) { lowlevelMoveCursor(x, y); }
    virtual void lowlevelFlush() {};

};

inline void BufferedDisplay::addCursor(ScreenCursor *cursor)
{
    auto &cursors = instance->cursors;
    if (std::find(cursors.begin(), cursors.end(), cursor) == cursors.end())
        cursors.push_back(cursor);
}

inline void BufferedDisplay::removeCursor(ScreenCursor *cursor)
{
    changeCursor();
    auto &cursors = instance->cursors;
    auto &&it = std::find(cursors.begin(), cursors.end(), cursor);
    if (it != cursors.end())
        cursors.erase(it);
}

inline void BufferedDisplay::changeCursor()
{
    instance->caretMoved = true;
}

struct FlushScreenAlgorithm {

    BufferedDisplay &disp;
    TPoint size;
    TPoint last;
    int x, y;
    BufferCell cell, *pCell;
    BufferedDisplay::Range damage;

    FlushScreenAlgorithm(BufferedDisplay &disp_) :
        disp(disp_)
    {
    }

    BufferCell &cellAt(int y, int x);
    void getCell();
    bool wideCanSpill() const;
    bool wideCanOverlap() const;

    void run();
    void processCell();
    void writeCell();
    void handleWideCharSpill();
    void handleNull();

};

inline BufferCell& FlushScreenAlgorithm::cellAt(int y, int x)
{
    return disp.buffer[y*size.x + x];
}

inline void FlushScreenAlgorithm::getCell()
{
    pCell = &cellAt(y, x);
    cell = *pCell;
}

inline bool FlushScreenAlgorithm::wideCanSpill() const
{
    return disp.widePlaceholder == '\0';
}

inline bool FlushScreenAlgorithm::wideCanOverlap() const
{
    return disp.wideOverlapping;
}

#endif

#ifndef BUFFDISP_H
#define BUFFDISP_H

#define Uses_TPoint
#include <tvision/tv.h>

#include <internal/platform.h>
#include <internal/textattr.h>
#include <set>
#include <vector>
#include <chrono>
#include <string_view>

class ScreenCursor;

class BufferedDisplay : public DisplayStrategy {

    friend class ScreenCursor;
    friend struct FlushScreenAlgorithm;

    struct Range {
        int begin, end;
    };

    TPoint size;
    std::vector<BufferCell> buffer;
    std::vector<Range> rowDamage;
    bool screenChanged;

    const uint widePlaceholder;
    bool caretMoved;
    TPoint caretPosition;

    bool limitFPS;
    int frameDrops;
    int maxFrameDrops;
    std::chrono::microseconds flushDelay;
    std::chrono::time_point<std::chrono::steady_clock> lastFlush;

    static BufferedDisplay *instance;
    static std::set<ScreenCursor*> cursors;
    static constexpr int defaultMaxFrameDrops = 4;
    static constexpr int defaultFPS = 60;

    void resetBuffer();
    void setDirty(int x, BufferCell &cell, Range &damage);
    void ensurePrintable(BufferCell &cell) const;

    void drawCursors();
    void undrawCursors();

    bool timeToFlush();

protected:

    BufferedDisplay();
    ~BufferedDisplay();

    void init();

    void setCaretPosition(int x, int y);
    void screenWrite(int x, int y, TScreenCell *buf, int len);
    void flushScreen();
    void onScreenResize();

    virtual void lowlevelWriteChars(const uchar chars[4], TCellAttribs attr) = 0; // 'chars' is null-terminated
    virtual void lowlevelMoveCursor(uint x, uint y) = 0;
    virtual void lowlevelMoveCursorX(uint x, uint y) { lowlevelMoveCursor(x, y); }
    virtual void lowlevelMoveCursorYby1(uint x, uint y) { lowlevelMoveCursor(x, y); }
    virtual void lowlevelFlush() = 0;

};

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

    void getCell();
    bool wideCanSpill() const;

    void run();
    void processCell();
    void writeCell();
    void handleWideCharSpill();
    void handleNull();

};

inline void FlushScreenAlgorithm::getCell()
{
    pCell = &disp.buffer[y*size.x + x];
    cell = *pCell;
}

inline bool FlushScreenAlgorithm::wideCanSpill() const
{
    return disp.widePlaceholder == '\0';
}

#endif

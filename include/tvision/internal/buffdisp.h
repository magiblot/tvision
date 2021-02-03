#ifndef BUFFDISP_H
#define BUFFDISP_H

#define Uses_TPoint
#include <tvision/tv.h>

#include <internal/platform.h>
#include <internal/textattr.h>
#include <algorithm>
#include <vector>
#include <chrono>

class ScreenCursor;

class BufferedDisplay : public DisplayStrategy {

    friend struct FlushScreenAlgorithm;

    struct Range {
        int begin, end;
    };

    std::vector<TScreenCell> buffer, flushBuffer;
    std::vector<Range> rowDamage;
    bool screenTouched;

    const bool wideOverlapping;
    const uint widePlaceholder;
    bool caretMoved;
    TPoint caretPosition;
    int newCaretSize;

    bool limitFPS;
    int frameDrops;
    int maxFrameDrops;
    std::chrono::microseconds flushDelay;
    std::chrono::time_point<std::chrono::steady_clock> lastFlush;

    static BufferedDisplay *instance;
    static constexpr int defaultMaxFrameDrops = 4;
    static constexpr int defaultFPS = 60;

    bool inBounds(int x, int y) const;

    void resizeBuffer();
    void setDirty(int x, int y, int len);
    void validateCell(TScreenCell &cell) const;

    std::vector<ScreenCursor*> cursors;
    void drawCursors();
    void undrawCursors();

    bool needsFlush() const;
    bool timeToFlush();

public:

    static void addCursor(ScreenCursor *cursor);
    static void removeCursor(ScreenCursor *cursor);
    static void changeCursor();

protected:

    BufferedDisplay();
    ~BufferedDisplay();

    void setCaretSize(int size) override;
    void setCaretPosition(int x, int y) override;
    void screenWrite(int x, int y, TScreenCell *buf, int len) override;
    void flushScreen() override;
    void reloadScreenInfo() override;

    virtual void lowlevelWriteChars(TStringView chars, TCellAttribs attr) = 0;
    virtual void lowlevelMoveCursor(uint x, uint y) = 0;
    virtual void lowlevelMoveCursorX(uint x, uint y) { lowlevelMoveCursor(x, y); }
    virtual void lowlevelCursorSize(int size) = 0;
    virtual void lowlevelFlush() {};

};

inline bool BufferedDisplay::inBounds(int x, int y) const
{
    return 0 <= x && x < size.x &&
           0 <= y && y < size.y;
}

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
    TScreenCell cell;
    size_t iCell;
    BufferedDisplay::Range damage;

    FlushScreenAlgorithm(BufferedDisplay &disp) :
        disp(disp)
    {
    }

    TScreenCell &cellAt(int y, int x);
    void getCell();
    bool cellDirty() const;
    bool wideCanSpill() const;
    bool wideCanOverlap() const;

    void run();
    void processCell();
    void writeCell();
    void commitDirty();
    void handleWideCharSpill();
    void handleNull();

};

inline TScreenCell& FlushScreenAlgorithm::cellAt(int y, int x)
{
    return disp.buffer[y*size.x + x];
}

inline void FlushScreenAlgorithm::getCell()
{
    auto *pCell = &cellAt(y, x);
    iCell = pCell - &cellAt(0, 0);
    cell = *pCell;
    disp.validateCell(cell);
}

inline bool FlushScreenAlgorithm::cellDirty() const
{
    return memcmp(&disp.flushBuffer[iCell], &cell, sizeof(TScreenCell)) != 0;
}

inline void FlushScreenAlgorithm::commitDirty()
{
    disp.flushBuffer[iCell] = cell;
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

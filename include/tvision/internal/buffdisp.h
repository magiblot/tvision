#ifndef BUFFDISP_H
#define BUFFDISP_H

#define Uses_TPoint
#include <tvision/tv.h>

#include <internal/platform.h>
#include <algorithm>
#include <vector>
#include <chrono>

class ScreenCursor;

namespace buffdisp {
namespace {

    struct FlushScreenAlgorithm;

}
}

class BufferedDisplay : public DisplayStrategy {

    friend buffdisp::FlushScreenAlgorithm;

    struct Range {
        int begin, end;
    };

    std::vector<TScreenCell> buffer, flushBuffer;
    std::vector<Range> rowDamage;
    bool screenTouched;

    const uint widePlaceholder;
    const bool wideOverlapping;
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

    virtual void lowlevelWriteChars(TStringView chars, TColorAttr attr) = 0;
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

#endif

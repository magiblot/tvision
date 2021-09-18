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
    std::chrono::microseconds flushDelay;
    std::chrono::time_point<std::chrono::steady_clock> lastFlush;

    static BufferedDisplay *instance;
    static constexpr int defaultFPS = 60;

    bool inBounds(int x, int y) const noexcept;

    void resizeBuffer() noexcept;
    void setDirty(int x, int y, int len) noexcept;
    void validateCell(TScreenCell &cell) const noexcept;

    std::vector<ScreenCursor*> cursors;
    void drawCursors() noexcept;
    void undrawCursors() noexcept;

    bool needsFlush() const noexcept;
    bool timeToFlush() noexcept;

public:

    static void addCursor(ScreenCursor *cursor) noexcept;
    static void removeCursor(ScreenCursor *cursor) noexcept;
    static void changeCursor() noexcept;

protected:

    BufferedDisplay() noexcept;
    ~BufferedDisplay();

    void setCaretSize(int size) noexcept override;
    void setCaretPosition(int x, int y) noexcept override;
    void screenWrite(int x, int y, TScreenCell *buf, int len) noexcept override;
    void flushScreen() noexcept override;
    void reloadScreenInfo() noexcept override;

    virtual void lowlevelWriteChars(TStringView chars, TColorAttr attr) noexcept = 0;
    virtual void lowlevelMoveCursor(uint x, uint y) noexcept = 0;
    virtual void lowlevelMoveCursorX(uint x, uint y) noexcept { lowlevelMoveCursor(x, y); }
    virtual void lowlevelCursorSize(int size) noexcept = 0;
    virtual void lowlevelFlush() noexcept {};

};

inline bool BufferedDisplay::inBounds(int x, int y) const noexcept
{
    return 0 <= x && x < size.x &&
           0 <= y && y < size.y;
}

inline void BufferedDisplay::addCursor(ScreenCursor *cursor) noexcept
{
    auto &cursors = instance->cursors;
    if (std::find(cursors.begin(), cursors.end(), cursor) == cursors.end())
        cursors.push_back(cursor);
}

inline void BufferedDisplay::removeCursor(ScreenCursor *cursor) noexcept
{
    changeCursor();
    auto &cursors = instance->cursors;
    auto &&it = std::find(cursors.begin(), cursors.end(), cursor);
    if (it != cursors.end())
        cursors.erase(it);
}

inline void BufferedDisplay::changeCursor() noexcept
{
    instance->caretMoved = true;
}

#endif

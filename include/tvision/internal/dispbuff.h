#ifndef TVISION_DISPBUFF_H
#define TVISION_DISPBUFF_H

#define Uses_TPoint
#define Uses_TScreenCell
#include <tvision/tv.h>

#include <vector>
#include <chrono>

namespace tvision
{

class DisplayAdapter;

namespace
{
struct FlushScreenAlgorithm;
}

class DisplayBuffer
{
    friend FlushScreenAlgorithm;

    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    struct Range {
        int begin, end;
    };

    std::vector<TScreenCell> buffer, flushBuffer;
    std::vector<Range> rowDamage;

    const bool wideOverlapping;
    bool screenTouched {true};
    bool caretOrCursorChanged {false};

    TPoint caretPosition {-1, -1};
    int newCaretSize {0};

    TPoint cursorPosition {-1, -1};
    bool cursorVisible {false};
    TColorAttr attrUnderCursor;

    bool limitFPS;
    std::chrono::microseconds flushDelay {};
    TimePoint lastFlush {};
    TimePoint pendingFlush {};

#ifdef _WIN32
    static constexpr int defaultFPS = 120; // Just 60 feels notably slower on Windows, I don't know why.
#else
    static constexpr int defaultFPS = 60;
#endif

    bool inBounds(int x, int y) const noexcept;

    void resizeBuffer() noexcept;
    void setDirty(int x, int y, int len) noexcept;
    void validateCell(TScreenCell &cell) const noexcept;

    void drawCursor() noexcept;
    void undrawCursor() noexcept;

    bool needsFlush() const noexcept;
    bool timeToFlush() noexcept;

public:

    TPoint size {};
    int caretSize {-1};

    DisplayBuffer() noexcept;

    void setCaretSize(int size) noexcept;
    void setCaretPosition(int x, int y) noexcept;
    void screenWrite(int x, int y, TScreenCell *buf, int len) noexcept;
    void clearScreen(DisplayAdapter &) noexcept;
    void redrawScreen(DisplayAdapter &) noexcept;
    void flushScreen(DisplayAdapter &) noexcept;
    TScreenCell *reloadScreenInfo(DisplayAdapter &) noexcept;

    void setCursorPosition(int x, int y) noexcept;
    void setCursorVisibility(bool visible) noexcept;

    int timeUntilPendingFlushMs() noexcept;
};

inline bool DisplayBuffer::inBounds(int x, int y) const noexcept
{
    return 0 <= x && x < size.x &&
           0 <= y && y < size.y;
}

} // namespace tvision

#endif // TVISION_DISPBUFF_H

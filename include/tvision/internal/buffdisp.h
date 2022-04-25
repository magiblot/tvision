#ifndef TVISION_BUFFDISP_H
#define TVISION_BUFFDISP_H

#define Uses_TPoint
#include <tvision/tv.h>

#include <vector>
#include <chrono>

namespace tvision
{

class ScreenCursor;
class DisplayStrategy;

namespace
{
struct FlushScreenAlgorithm;
}

class BufferedDisplay
{
    friend FlushScreenAlgorithm;

    struct Range {
        int begin, end;
    };

    std::vector<TScreenCell> buffer, flushBuffer;
    std::vector<Range> rowDamage;

    const bool wideOverlapping;
    bool screenTouched {true};
    bool caretMoved {false};
    TPoint caretPosition {-1, -1};
    int newCaretSize {0};

    bool limitFPS;
    std::chrono::microseconds flushDelay {};
    std::chrono::time_point<std::chrono::steady_clock> lastFlush {};

    static BufferedDisplay *instance;
#ifdef _WIN32
    static constexpr int defaultFPS = 120; // Just 60 feels notably slower on Windows, I don't know why.
#else
    static constexpr int defaultFPS = 60;
#endif

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

    TPoint size {};
    int caretSize {};

    BufferedDisplay() noexcept;
    ~BufferedDisplay();

    void setCaretSize(int size) noexcept;
    void setCaretPosition(int x, int y) noexcept;
    void screenWrite(int x, int y, TScreenCell *buf, int len) noexcept;
    void clearScreen(DisplayStrategy &) noexcept;
    void flushScreen(DisplayStrategy &) noexcept;
    TScreenCell *reloadScreenInfo(DisplayStrategy &) noexcept;

    static void addCursor(ScreenCursor *cursor) noexcept;
    static void removeCursor(ScreenCursor *cursor) noexcept;
    static void changeCursor() noexcept;
};

inline bool BufferedDisplay::inBounds(int x, int y) const noexcept
{
    return 0 <= x && x < size.x &&
           0 <= y && y < size.y;
}

inline void BufferedDisplay::addCursor(ScreenCursor *cursor) noexcept
{
    auto &cursors = instance->cursors;
    for (auto it = cursors.begin(); it != cursors.end(); ++it)
        if (*it == cursor)
            return;
    cursors.push_back(cursor);
}

inline void BufferedDisplay::removeCursor(ScreenCursor *cursor) noexcept
{
    changeCursor();
    auto &cursors = instance->cursors;
    for (auto it = cursors.begin(); it != cursors.end(); ++it)
        if (*it == cursor)
            return (void) cursors.erase(it);
}

inline void BufferedDisplay::changeCursor() noexcept
{
    instance->caretMoved = true;
}

} // namespace tvision

#endif // TVISION_BUFFDISP_H

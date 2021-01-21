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

protected:

    // 'caretSize' must be a value in the range 0 to 100. Zero means it is not
    // visible.

    int caretSize;

    // 'getCaretSize()' must return a number in the range 1 to 100,
    // because the original THardwareInfo interface maps directly to
    // the Win32 Console API (see CONSOLE_CURSOR_INFO).

    int getCaretSize() override { return max(caretSize, 1); }
    bool isCaretVisible() override { return caretSize > 0; }
    void setCaretSize(int size) override { caretSize = size; }

private:

    friend struct FlushScreenAlgorithm;

    struct Range {
        int begin, end;
    };

    std::vector<BufferCell> buffer;
    std::vector<Range> rowDamage;
    bool screenChanged;

    const bool wideOverlapping;
    const uint widePlaceholder;
    bool caretMoved;
    TPoint caretPosition;
    int lastCaretSize;

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
    static void setDirty(int x, BufferCell &cell, Range &damage);
    void ensurePrintable(BufferCell &cell) const;

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

    void setCaretPosition(int x, int y);
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
    BufferCell cell, *pCell;
    BufferedDisplay::Range damage;

    FlushScreenAlgorithm(BufferedDisplay &disp) :
        disp(disp)
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

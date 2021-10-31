#ifndef PLATFORM_H
#define PLATFORM_H

#define Uses_TPoint
#include <tvision/tv.h>
#include <internal/stdioctl.h>
#include <internal/events.h>

class DisplayStrategy
{
public:

    virtual ~DisplayStrategy() {}

    // Invariant: these variables are is in sync with the actual screen state
    // after 'DisplayStrategy::reloadScreenInfo()' has been invoked.

    TPoint size {};
    int caretSize {};

    // This function can be overriden by inheritors in order to update
    // internal structs when the display properties change. But
    // 'DisplayStrategy::reloadScreenInfo()' must be invoked eventually.

    virtual void reloadScreenInfo() noexcept
    {
        size = getScreenSize();
        caretSize = getCaretSize();
    }

protected:

    // These functions are meant to be overriden by inheritors. Their only
    // responsibility should be to return updated values.

    virtual TPoint getScreenSize() noexcept { return size; }

    // 'getCaretSize()' must return a value in the range 0 to 100. Zero means
    // the caret is not visible.

    virtual int getCaretSize() noexcept { return caretSize; }

public:

    virtual void clearScreen() noexcept {}
    virtual void setCaretPosition(int x, int y) noexcept {}
    virtual ushort getScreenMode() noexcept { return 0; }
    virtual void setCaretSize(int size) noexcept {}
    virtual void screenWrite(int x, int y, TScreenCell *buf, int len) noexcept {}
    virtual void flushScreen() noexcept {}
};

struct TEvent;

class InputStrategy : public EventSource
{
public:

    InputStrategy(SysHandle aHandle) noexcept :
        EventSource(aHandle)
    {
    }

    virtual ~InputStrategy() {}
    virtual bool getEvent(TEvent &ev) noexcept { return false; }
    virtual int getButtonCount() noexcept { return 0; }
    virtual void cursorOn() noexcept {}
    virtual void cursorOff() noexcept {}
};

struct ConsoleStrategy
{
    DisplayStrategy &display;
    InputStrategy &input;

    ConsoleStrategy(DisplayStrategy &aDisplay, InputStrategy &aInput) noexcept :
        display(aDisplay),
        input(aInput)
    {
    }

    virtual ~ConsoleStrategy() {}
    virtual bool isAlive() noexcept { return true; }
    virtual void forEachSource(void *, void (&)(void *, EventSource &)) noexcept {}
};

class Platform
{
    StdioCtl io;
    EventWaiter waiter;
    DisplayStrategy dummyDisplay;
    InputStrategy dummyInput {(SysHandle) 0};
    ConsoleStrategy dummyConsole {dummyDisplay, dummyInput};
    // Invariant: 'console' is either a non-owning reference to 'dummyConsole'
    // or an owning reference to a heap-allocated ConsoleStrategy object.
    ConsoleStrategy *console {&dummyConsole};

    Platform() noexcept;
    ~Platform();

    void checkConsole() noexcept;
    ConsoleStrategy &createConsole() noexcept;

    static int errorCharWidth(TStringView, char32_t) noexcept;

public:

    static Platform instance;
    static int (*charWidth)(TStringView, char32_t) noexcept;

    void setUpConsole() noexcept;
    void restoreConsole() noexcept;

    bool getEvent(TEvent &ev) noexcept { return waiter.getEvent(ev); }
    void waitForEvents(int ms) noexcept { checkConsole(); waiter.waitForEvents(ms); }
    void stopEventWait() noexcept { waiter.stopEventWait(); }

    int getButtonCount() noexcept { return console->input.getButtonCount(); }
    void cursorOn() noexcept { console->input.cursorOn(); }
    void cursorOff() noexcept { console->input.cursorOff(); }

    // Adjust the caret size to the range 1 to 100 because that's what the original
    // THardwareInfo::getCaretSize() did and what TScreen expects.
    int getCaretSize() noexcept { return min(max(console->display.caretSize, 1), 100); }
    bool isCaretVisible() noexcept { return console->display.caretSize > 0; }
    void clearScreen() noexcept { console->display.clearScreen(); }
    int getScreenRows() noexcept { return console->display.size.y; }
    int getScreenCols() noexcept { return console->display.size.x; }
    void setCaretPosition(int x, int y) noexcept { console->display.setCaretPosition(x, y); }
    ushort getScreenMode() noexcept { return console->display.getScreenMode(); }
    void setCaretSize(int size) noexcept { console->display.setCaretSize(size); }
    void screenWrite(int x, int y, TScreenCell *b, int l) noexcept { console->display.screenWrite(x, y, b, l); }
    void flushScreen() noexcept { console->display.flushScreen(); }
    void reloadScreenInfo() noexcept { console->display.reloadScreenInfo(); }
};

#endif // PLATFORM_H

#ifndef PLATFORM_H
#define PLATFORM_H

#define Uses_TPoint
#include <tvision/tv.h>
#include <internal/stdioctl.h>
#include <internal/events.h>
#include <atomic>

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

using ThreadId = const void *;

class ThisThread
{
    static thread_local constexpr struct {} idBase {};

public:

    static const void *id() noexcept { return &idBase; }
};

#if ATOMIC_POINTER_LOCK_FREE < 2
#warning The code below assumes that atomic pointers are lock-free, but they are not.
#endif

template <class T>
struct SignalThreadSafe
{
    T t;
    std::atomic<ThreadId> lockingThread {nullptr};

    using Self = SignalThreadSafe;

    struct Lock
    {
        Self *self;
        Lock(Self *aSelf) noexcept : self(aSelf)
        {
            ThreadId zero = nullptr;
            // Use a spin lock because regular mutexes are not signal-safe.
            if (self)
                while (self->lockingThread.compare_exchange_weak(zero, ThisThread::id()))
                    ;
        }
        ~Lock()
        {
            if (self)
                self->lockingThread = nullptr;
        }
    };

    template <class Func>
    // 'func' takes a 'T &' by parameter.
    auto lock(Func &&func) noexcept
    {
        Lock lk {lockedByThisThread() ? nullptr : this};
        return func(t);
    }

    bool lockedByThisThread() const noexcept
    {
        return lockingThread == ThisThread::id();
    }
};

class Platform
{
    StdioCtl io;
    EventWaiter waiter;
    DisplayStrategy dummyDisplay;
    InputStrategy dummyInput {(SysHandle) 0};
    ConsoleStrategy dummyConsole {dummyDisplay, dummyInput};
    // Invariant: 'console' contains either a non-owning reference to 'dummyConsole'
    // or an owning reference to a heap-allocated ConsoleStrategy object.
    SignalThreadSafe<ConsoleStrategy *> console {&dummyConsole};

    Platform() noexcept;
    ~Platform();

    void checkConsole() noexcept;
    ConsoleStrategy &createConsole() noexcept;

    static int errorCharWidth(TStringView, char32_t) noexcept;
    static void signalCallback(bool) noexcept;

public:

    static Platform instance;
    static int (*charWidth)(TStringView, char32_t) noexcept;

    void setUpConsole() noexcept;
    void restoreConsole() noexcept;

    bool getEvent(TEvent &ev) noexcept { return waiter.getEvent(ev); }
    void waitForEvents(int ms) noexcept { checkConsole(); waiter.waitForEvents(ms); }
    void stopEventWait() noexcept { waiter.stopEventWait(); }

    int getButtonCount() noexcept
        { return console.lock([] (auto *c) { return c->input.getButtonCount(); }); }
    void cursorOn() noexcept
        { console.lock([] (auto *c) { c->input.cursorOn(); }); }
    void cursorOff() noexcept
        { console.lock([] (auto *c) { c->input.cursorOff(); }); }

    // Adjust the caret size to the range 1 to 100 because that's what the original
    // THardwareInfo::getCaretSize() does and what TScreen expects.
    int getCaretSize() noexcept
        { return min(max(console.lock([] (auto *c) { return c->display.caretSize; }), 1), 100); }
    bool isCaretVisible() noexcept
        { return console.lock([] (auto *c) { return c->display.caretSize; }) > 0; }
    void clearScreen() noexcept
        { console.lock([] (auto *c) { c->display.clearScreen(); }); }
    int getScreenRows() noexcept
        { return console.lock([] (auto *c) { return c->display.size.y; }); }
    int getScreenCols() noexcept
        { return console.lock([] (auto *c) { return c->display.size.x; }); }
    void setCaretPosition(int x, int y) noexcept
        { console.lock([&] (auto *c) { c->display.setCaretPosition(x, y); }); }
    ushort getScreenMode() noexcept
        { return console.lock([] (auto *c) { return c->display.getScreenMode(); }); }
    void setCaretSize(int size) noexcept
        { console.lock([&] (auto *c) { c->display.setCaretSize(size); }); }
    void screenWrite(int x, int y, TScreenCell *b, int l) noexcept
        { console.lock([&] (auto *c) { c->display.screenWrite(x, y, b, l); }); }
    void flushScreen() noexcept
        { console.lock([] (auto *c) { c->display.flushScreen(); }); }
    void reloadScreenInfo() noexcept
        { console.lock([] (auto *c) { c->display.reloadScreenInfo(); }); }
};

#endif // PLATFORM_H

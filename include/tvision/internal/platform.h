#ifndef PLATFORM_H
#define PLATFORM_H

#define Uses_TPoint
#include <tvision/tv.h>
#include <internal/stdioctl.h>
#include <internal/buffdisp.h>
#include <internal/events.h>
#include <atomic>

class DisplayStrategy
{
public:

    virtual ~DisplayStrategy() {}

    virtual TPoint getScreenSize() noexcept { return {}; }
    virtual int getCaretSize() noexcept { return 0; } // Range [0, 100].
    virtual void clearScreen() noexcept {}
    virtual ushort getScreenMode() noexcept { return 0; }
    virtual void reloadScreenInfo() noexcept {}
    virtual void lowlevelWriteChars(TStringView chars, TColorAttr attr) noexcept {}
    virtual void lowlevelMoveCursor(uint x, uint y) noexcept {};
    virtual void lowlevelMoveCursorX(uint x, uint y) noexcept { lowlevelMoveCursor(x, y); }
    virtual void lowlevelCursorSize(int size) noexcept {};
    virtual void lowlevelFlush() noexcept {};
    virtual TPoint actualScreenSize() noexcept { return {}; }
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

struct ThisThread
{
    static const void *id() noexcept
    {
        static thread_local struct {} idBase;
        return &idBase;
    }
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
#ifdef _TV_UNIX
    StdioCtl io;
#endif
    EventWaiter waiter;
    BufferedDisplay displayBuf;
    DisplayStrategy dummyDisplay;
    InputStrategy dummyInput {(SysHandle) 0};
    ConsoleStrategy dummyConsole {dummyDisplay, dummyInput};
    // Invariant: 'console' contains either a non-owning reference to 'dummyConsole'
    // or an owning reference to a heap-allocated ConsoleStrategy object.
    SignalThreadSafe<ConsoleStrategy *> console {&dummyConsole};
    TPoint lastSize {};

    Platform() noexcept;
    ~Platform();

    void checkConsole() noexcept;
    bool sizeChanged(TEvent &ev) noexcept;
    ConsoleStrategy &createConsole() noexcept;

    static int errorCharWidth(TStringView, char32_t) noexcept;
    static void signalCallback(bool) noexcept;

    TPoint actualScreenSize() noexcept
        { return console.lock([] (auto *c) { return c->display.actualScreenSize(); }); }

public:

    static Platform instance;
    static int (*charWidth)(TStringView, char32_t) noexcept;

    void setUpConsole() noexcept;
    void restoreConsole() noexcept;

    bool getEvent(TEvent &ev) noexcept;
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
    int getCaretSize() noexcept { return min(max(displayBuf.caretSize, 1), 100); }
    bool isCaretVisible() noexcept { return displayBuf.caretSize > 0; }
    void clearScreen() noexcept
        { console.lock([&] (auto *c) { displayBuf.clearScreen(c->display); }); }
    int getScreenRows() noexcept { return displayBuf.size.y; }
    int getScreenCols() noexcept { return displayBuf.size.x; }
    void setCaretPosition(int x, int y) noexcept { displayBuf.setCaretPosition(x, y); }
    ushort getScreenMode() noexcept
        { return console.lock([] (auto *c) { return c->display.getScreenMode(); }); }
    void setCaretSize(int size) noexcept { displayBuf.setCaretSize(size); }
    void screenWrite(int x, int y, TScreenCell *b, int l) noexcept { displayBuf.screenWrite(x, y, b, l); }
    void flushScreen() noexcept
        { console.lock([&] (auto *c) { displayBuf.flushScreen(c->display); }); }
    TScreenCell *reloadScreenInfo() noexcept
    {
        auto *buffer = console.lock([&] (auto *c) { return displayBuf.reloadScreenInfo(c->display); });
        lastSize = displayBuf.size;
        return buffer;
    }

};

#endif // PLATFORM_H

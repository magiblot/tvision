#ifndef TVISION_PLATFORM_H
#define TVISION_PLATFORM_H

#define Uses_TPoint
#define Uses_TColorAttr
#include <tvision/tv.h>
#include <internal/stdioctl.h>
#include <internal/dispbuff.h>
#include <internal/events.h>
#include <atomic>
#include <vector>

struct TEvent;

namespace tvision
{

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
    virtual bool screenChanged() noexcept { return false; }
};

class InputStrategy : public EventSource
{
public:

    InputStrategy(SysHandle aHandle) noexcept :
        EventSource(aHandle)
    {
    }

    virtual ~InputStrategy() {}

    virtual int getButtonCount() noexcept { return 0; }
    virtual void cursorOn() noexcept {}
    virtual void cursorOff() noexcept {}
};

struct ConsoleStrategy
{
    DisplayStrategy &display;
    InputStrategy &input;
    const std::vector<EventSource *> sources;

    ConsoleStrategy( DisplayStrategy &aDisplay, InputStrategy &aInput,
                     std::vector<EventSource *> &&aSources ) noexcept :
        display(aDisplay),
        input(aInput),
        sources(std::move(aSources))
    {
    }

    virtual ~ConsoleStrategy() {}

    virtual bool isAlive() noexcept { return true; }
    virtual bool setClipboardText(TStringView) noexcept { return false; }
    virtual bool requestClipboardText(void (&)(TStringView)) noexcept { return false; }
};

using ThreadId = const void *;

struct ThisThread
{
    static ThreadId id() noexcept
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
    std::atomic<ThreadId> lockingThread {};

    struct LockGuard
    {
        SignalThreadSafe *self;
        LockGuard(SignalThreadSafe *aSelf) noexcept : self(aSelf)
        {
            ThreadId none {};
            // Use a spin lock because regular mutexes are not signal-safe.
            if (self)
                while (self->lockingThread.compare_exchange_weak(none, ThisThread::id()))
                    ;
        }
        ~LockGuard()
        {
            if (self)
                self->lockingThread = ThreadId {};
        }
    };

    template <class Func>
    // 'func' takes a 'T &' by parameter.
    auto lock(Func &&func) noexcept
    {
        LockGuard lk {lockedByThisThread() ? nullptr : this};
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
    DisplayBuffer displayBuf;
    DisplayStrategy dummyDisplay;
    InputStrategy dummyInput {(SysHandle) 0};
    ConsoleStrategy dummyConsole {dummyDisplay, dummyInput, {}};
    // Invariant: 'console' contains either a non-owning reference to 'dummyConsole'
    // or an owning reference to a heap-allocated ConsoleStrategy object.
    SignalThreadSafe<ConsoleStrategy *> console {&dummyConsole};

    Platform() noexcept;
    ~Platform();

    void setUpConsole(ConsoleStrategy *&) noexcept;
    void restoreConsole(ConsoleStrategy *&) noexcept;
    void checkConsole() noexcept;
    bool sizeChanged(TEvent &ev) noexcept;
    ConsoleStrategy &createConsole() noexcept;

    static int errorCharWidth(uint32_t) noexcept;
    static void signalCallback(bool) noexcept;

    bool screenChanged() noexcept
        { return console.lock([] (auto *c) { return c->display.screenChanged(); }); }

public:

    static Platform instance;
    static int (*charWidth)(uint32_t) noexcept;

    // Explicit 'this' required by GCC 5.
    void setUpConsole() noexcept
        { console.lock([&] (auto *&c) { this->setUpConsole(c); }); }
    void restoreConsole() noexcept
        { console.lock([&] (auto *&c) { this->restoreConsole(c); }); }

    bool getEvent(TEvent &ev) noexcept;
    void waitForEvent(int ms) noexcept { checkConsole(); waiter.waitForEvent(ms); }
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
        { return console.lock([&] (auto *c) { return displayBuf.reloadScreenInfo(c->display); }); }

    bool setClipboardText(TStringView text) noexcept
        { return console.lock([&] (auto *c) { return c->setClipboardText(text); }); }
    bool requestClipboardText(void (&accept)(TStringView)) noexcept
        { return console.lock([&] (auto *c) { return c->requestClipboardText(accept); }); }
};

} // namespace tvision

#endif // TVISION_PLATFORM_H

#ifndef PLATFORM_H
#define PLATFORM_H

#define Uses_TPoint
#include <tvision/tv.h>
#include <memory>
#include <functional>
#include <queue>
#include <vector>
#include <cstddef>

class DisplayStrategy {

public:

    virtual ~DisplayStrategy() {}

    // Invariant: these variables are is in sync with the actual screen state
    // after 'DisplayStrategy::reloadScreenInfo()' has been invoked.

    TPoint size {};
    int caretSize {};

    // This function can be overriden by inheritors in order to update
    // internal structs when the display properties change. But
    // 'DisplayStrategy::reloadScreenInfo()' must be invoked eventually.

    virtual void reloadScreenInfo()
    {
        size = getScreenSize();
        caretSize = getCaretSize();
    }

protected:

    // These functions are meant to be overriden by inheritors. Their only
    // responsibility should be to return updated values.

    virtual TPoint getScreenSize() { return size; }

    // 'getCaretSize()' must return a value in the range 0 to 100. Zero means
    // the caret is not visible. Note that PlatformStrategy::getCaretSize()
    // adjusts this to the range 1 to 100, because that's what the original
    // THardwareInfo::getCaretSize() did, and what TScreen expects.

    virtual int getCaretSize() { return caretSize; }

public:

    bool isCaretVisible() { return caretSize != 0; }
    virtual void clearScreen() {}
    virtual void setCaretPosition(int x, int y) {}
    virtual ushort getScreenMode() { return 0; }
    virtual void setCaretSize(int size) {}
    virtual void screenWrite(int x, int y, TScreenCell *buf, int len) {}
    virtual void flushScreen() {}

};

struct TEvent;

class InputStrategy {

public:

    virtual ~InputStrategy() {}
    virtual bool getEvent(TEvent &ev) { return false; }
    virtual int getButtonCount() { return 0; }
    virtual void cursorOn() {}
    virtual void cursorOff() {}

};

class PlatformStrategy {

protected:

    std::unique_ptr<DisplayStrategy> display;
    std::unique_ptr<InputStrategy> input;

public:

    PlatformStrategy() {}
    PlatformStrategy( DisplayStrategy *d, InputStrategy *i ) :
        display(d),
        input(i)
    {
        if (display) display->reloadScreenInfo();
    }

    virtual ~PlatformStrategy() {}

    virtual bool waitForEvent(long ms, TEvent &ev) { return false; }
    virtual int getButtonCount() { return input ? input->getButtonCount() : 0; }
    virtual void cursorOn() { if (input) input->cursorOn(); }
    virtual void cursorOff() { if (input) input->cursorOff(); }

    virtual int getCaretSize() { return min(max(display->caretSize, 1), 100); }
    virtual bool isCaretVisible() { return display->isCaretVisible(); }
    virtual void clearScreen() { display->clearScreen(); }
    virtual int getScreenRows() { return display->size.y; }
    virtual int getScreenCols() { return display->size.x; }
    virtual void setCaretPosition(int x, int y) { display->setCaretPosition(x, y); }
    virtual ushort getScreenMode() { return display->getScreenMode(); }
    virtual void setCaretSize(int size) { display->setCaretSize(size); }
    virtual void screenWrite(int x, int y, TScreenCell *buf, int len) { display->screenWrite(x, y, buf, len); }
    virtual void flushScreen() { display->flushScreen(); }
    virtual void reloadScreenInfo() { display->reloadScreenInfo(); }
    virtual int charWidth(TStringView mbc, char32_t wc) { return 1; }

    static PlatformStrategy *instance; // hardware.cpp

};

class NullPlatform : public PlatformStrategy {

    NullPlatform() = default;
    ~NullPlatform() = default;

public:

    int getButtonCount() override { return 0; }
    void cursorOn() override {}
    void cursorOff() override {}

    int getCaretSize() override { return 0; }
    bool isCaretVisible() override { return false; }
    void clearScreen() override {}
    int getScreenRows() override { return 0; }
    int getScreenCols() override { return 0; }
    void setCaretPosition(int x, int y) override {}
    ushort getScreenMode() override { return 0; }
    void setCaretSize(int size) override {}
    void screenWrite(int x, int y, TScreenCell *buf, int len) override {}
    void flushScreen() override {}

    static NullPlatform instance; // hardware.cpp

};

#ifdef _TV_UNIX
struct pollfd;

class FdInputStrategy : public InputStrategy {

    std::function<bool (TEvent&)> eventGetter;
    static std::vector<FdInputStrategy*> listeners;
    static std::vector<struct pollfd> fds;
    static std::queue<size_t> ready;

public:

    FdInputStrategy();
    ~FdInputStrategy();

    static void addListener(FdInputStrategy*, int);
    static void deleteListener(FdInputStrategy*);
    static bool waitForEvent(int ms, TEvent &ev);
    void overrideEventGetter(std::function<bool (TEvent&)>&&);
    virtual bool hasPendingEvents() { return false; }

};

class UnixPlatformStrategy : public PlatformStrategy {

public:

    using PlatformStrategy::PlatformStrategy;

    bool waitForEvent(long ms, TEvent &ev) override
    {
        return FdInputStrategy::waitForEvent(ms, ev);
    }

    int charWidth(TStringView mbc, char32_t wc) override; // ttext.cpp

};

#endif // _TV_UNIX

#endif // PLATFORM_H

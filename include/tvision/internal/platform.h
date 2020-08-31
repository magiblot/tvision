#ifndef PLATFORM_H
#define PLATFORM_H

#include <memory>
#include <functional>
#include <queue>
#include <vector>
#include <cstddef>

class DisplayStrategy {

public:

    virtual ~DisplayStrategy() {}
    virtual int getCaretSize() { return 0; }
    virtual bool isCaretVisible() { return false; }
    virtual void clearScreen() {}
    virtual int getScreenRows() { return 0; }
    virtual int getScreenCols() { return 0; }
    virtual void setCaretPosition(int x, int y) {}
    virtual ushort getScreenMode() { return 0; }
    virtual void setCaretSize(int size) {}
    virtual void screenWrite(int x, int y, TScreenCell *buf, int len) {}
    virtual void flushScreen() {}
    virtual void onScreenResize() {}

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

};
#endif

class PlatformStrategy {

protected:

    std::unique_ptr<DisplayStrategy> display;
    std::unique_ptr<InputStrategy> input;

public:

    PlatformStrategy() {}
    PlatformStrategy(DisplayStrategy* d, InputStrategy *i) :
        display(d), input(i) {}

    virtual ~PlatformStrategy() {}

    virtual bool waitForEvent(long ms, TEvent &ev)
#ifdef _TV_UNIX
    { return FdInputStrategy::waitForEvent(ms, ev); }
#else
    { return false; }
#endif
    virtual int getButtonCount() { return input ? input->getButtonCount() : 0; }
    virtual void cursorOn() { if (input) input->cursorOn(); }
    virtual void cursorOff() { if (input) input->cursorOff(); }

    virtual int getCaretSize() { return display->getCaretSize(); }
    virtual bool isCaretVisible() { return display->isCaretVisible(); }
    virtual void clearScreen() { display->clearScreen(); }
    virtual int getScreenRows() { return display->getScreenRows(); }
    virtual int getScreenCols() { return display->getScreenCols(); }
    virtual void setCaretPosition(int x, int y) { display->setCaretPosition(x, y); }
    virtual ushort getScreenMode() { return display->getScreenMode(); }
    virtual void setCaretSize(int size) { display->setCaretSize(size); }
    virtual void screenWrite(int x, int y, TScreenCell *buf, int len) { display->screenWrite(x, y, buf, len); }
    virtual void flushScreen() { display->flushScreen(); }
    virtual void onScreenResize() { display->onScreenResize(); }

};

#endif

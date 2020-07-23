#ifndef PLATFORM_H
#define PLATFORM_H

#define Uses_TEvent
#include <tvision/tv.h>

#include <memory>
#include <functional>
#include <queue>
#include <vector>
#include <cstddef>

class TScreenCell;

class DisplayStrategy {

public:

    virtual ~DisplayStrategy() {}
    virtual int getCaretSize() = 0;
    virtual bool isCaretVisible() = 0;
    virtual void clearScreen() = 0;
    virtual int getScreenRows() = 0;
    virtual int getScreenCols() = 0;
    virtual void setCaretPosition(int x, int y) = 0;
    virtual ushort getScreenMode() = 0;
    virtual void setCaretSize(int size) = 0;
    virtual void screenWrite(int x, int y, TScreenCell *buf, int len) = 0;
    virtual void flushScreen() = 0;
    virtual void onScreenResize() {}

};

struct pollfd;

class FdInputStrategy {

    std::function<bool (TEvent&)> eventGetter;
    static std::vector<FdInputStrategy*> listeners;
    static std::vector<struct pollfd> fds;
    static std::queue<size_t> ready;

public:

    FdInputStrategy();
    virtual ~FdInputStrategy();

    virtual bool getEvent(TEvent &ev);
    virtual int getButtonCount();

    static void addListener(FdInputStrategy*, int);
    static void deleteListener(FdInputStrategy*);
    static bool waitForEvent(int ms, TEvent &ev);
    void overrideEventGetter(std::function<bool (TEvent&)>&&);

};

class PlatformStrategy {

protected:

    std::unique_ptr<DisplayStrategy> display;
    std::unique_ptr<FdInputStrategy> input;

public:

    PlatformStrategy(DisplayStrategy* d, FdInputStrategy *i) :
        display(d), input(i) {}

    virtual ~PlatformStrategy() {}

    inline bool waitForEvent(long ms, TEvent &ev) { return FdInputStrategy::waitForEvent(ms, ev); }
    virtual int getButtonCount() { return input ? input->getButtonCount() : 0; }

    inline int getCaretSize() { return display->getCaretSize(); }
    inline bool isCaretVisible() { return display->isCaretVisible(); }
    inline void clearScreen() { display->clearScreen(); }
    inline int getScreenRows() { return display->getScreenRows(); }
    inline int getScreenCols() { return display->getScreenCols(); }
    inline void setCaretPosition(int x, int y) { display->setCaretPosition(x, y); }
    inline ushort getScreenMode() { return display->getScreenMode(); }
    inline void setCaretSize(int size) { display->setCaretSize(size); }
    inline void screenWrite(int x, int y, TScreenCell *buf, int len) { display->screenWrite(x, y, buf, len); }
    virtual void flushScreen() { display->flushScreen(); }
    void onScreenResize() { display->onScreenResize(); }

};

#endif

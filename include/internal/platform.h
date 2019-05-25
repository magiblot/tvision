#ifndef PLATFORM_H
#define PLATFORM_H

#define Uses_TEvent
#include <tvision/tv.h>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <functional>
#include <list>

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
    virtual void screenWrite(int x, int y, ushort *buf, int len) = 0;
    virtual void flushScreen() = 0;

};

class AsyncInputStrategy {

    void startInputThread();

public:

    struct waiter {
        std::mutex m;
        std::condition_variable cv;
    };

    AsyncInputStrategy();
    virtual ~AsyncInputStrategy();
    virtual bool getEvent(TEvent &ev) = 0;
    void overrideEventGetter(std::function<bool (TEvent&)>&&);
    virtual int getButtonCount() = 0;

    static void resumeListening();
    static bool waitForEvent(long ms, TEvent &ev);
    static void notifyEvent(TEvent &ev, waiter &get);

private:

    std::thread inputThread;
    std::function<bool (TEvent&)> eventGetter;

    static std::list<AsyncInputStrategy*> listeners;
    static waiter *inputListener;
    static waiter eventRequester;
    static std::mutex notifying;

    static bool evReceived;
    static bool evProcessed;
    static TEvent received;

};

class PlatformStrategy {

protected:

    std::unique_ptr<DisplayStrategy> display;
    std::unique_ptr<AsyncInputStrategy> input;

public:

    PlatformStrategy(DisplayStrategy* d, AsyncInputStrategy *i) :
        display(d), input(i) {}

    virtual ~PlatformStrategy() {}

    inline void resumeListening() { AsyncInputStrategy::resumeListening(); }
    inline bool waitForEvent(long ms, TEvent &ev) { return AsyncInputStrategy::waitForEvent(ms, ev); }
    virtual int getButtonCount() { return input ? input->getButtonCount() : 0; }

    inline int getCaretSize() { return display->getCaretSize(); }
    inline bool isCaretVisible() { return display->isCaretVisible(); }
    inline void clearScreen() { display->clearScreen(); }
    inline int getScreenRows() { return display->getScreenRows(); }
    inline int getScreenCols() { return display->getScreenCols(); }
    inline void setCaretPosition(int x, int y) { display->setCaretPosition(x, y); }
    inline ushort getScreenMode() { return display->getScreenMode(); }
    inline void setCaretSize(int size) { display->setCaretSize(size); }
    inline void screenWrite(int x, int y, ushort *buf, int len) { display->screenWrite(x, y, buf, len); }
    virtual void flushScreen() { display->flushScreen(); }

};

#endif

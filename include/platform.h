#ifndef PLATFORM_H
#define PLATFORM_H

#define Uses_TEvent
#include <tvision/tv.h>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <memory>

class DisplayStrategy {

public:

    virtual void startDisplay() = 0;
    virtual void endDisplay() = 0;

    virtual int getCaretSize() = 0;
    virtual bool isCaretVisible() = 0;
    virtual void clearScreen() = 0;
    virtual int getScreenRows() = 0;
    virtual int getScreenCols() = 0;
    virtual void setCaretPosition(int x, int y) = 0;
    virtual ushort getScreenMode() = 0;
    virtual void setCaretSize(int size) = 0;
    virtual void screenWrite(int x, int y, ushort *buf, int len) = 0;

};

class AsyncInputStrategy {

public:

    struct waiter {
        std::mutex m;
        std::condition_variable cv;
    };

    virtual ~AsyncInputStrategy() {};
    virtual void startInput() = 0;
    virtual void endInput() = 0;

    void resumeListening();
    bool waitForEvent(long ms, TEvent &ev);
    void notifyEvent(TEvent &ev, waiter &get);

private:

    waiter *inputListener;
    waiter eventRequester;
    std::mutex notifying;

    bool evReceived = false;
    bool evProcessed = true;
    TEvent received;

};

class PlatformStrategy {

    std::shared_ptr<DisplayStrategy> display;

public:

    PlatformStrategy(DisplayStrategy* d) : display(d)
    {
        display->startDisplay();
    }

    virtual ~PlatformStrategy()
    {
        display->endDisplay();
    }

    inline int getCaretSize() { return display->getCaretSize(); }
    inline bool isCaretVisible() { return display->isCaretVisible(); }
    inline void clearScreen() { display->clearScreen(); }
    inline int getScreenRows() { return display->getScreenRows(); }
    inline int getScreenCols() { return display->getScreenCols(); }
    inline void setCaretPosition(int x, int y) { display->setCaretPosition(x, y); }
    inline ushort getScreenMode() { return display->getScreenMode(); }
    inline void setCaretSize(int size) { display->setCaretSize(size); }
    inline void screenWrite(int x, int y, ushort *buf, int len) { display->screenWrite(x, y, buf, len); }

};

class NcursesDisplay : public DisplayStrategy {


    bool hasColors;
    std::unordered_map<ushort, int> pairIdentifiers;
    ushort definedPairs;

    void getCaretPosition(int &x, int &y);
    uint translateAttributes(ushort attr);
    uint getColorPair(uchar pairKey);
    static uchar swapRedBlue (uchar c);

public:

    void startDisplay();
    void endDisplay();

    void setCaretSize(int size);
    int getCaretSize();
    void setCaretPosition(int x, int y);
    bool isCaretVisible();
    void clearScreen();
    int getScreenRows();
    int getScreenCols();
    ushort getScreenMode();
    void screenWrite(int x, int y, ushort *buf, int len);

};

#endif


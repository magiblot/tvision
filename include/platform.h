#ifndef PLATFORM_H
#define PLATFORM_H

#define Uses_TKeys
#define Uses_TPoint
#define Uses_TEvent
#include <tvision/tv.h>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <memory>
#include <string>

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
    virtual void flushScreen() = 0;

};

class AsyncInputStrategy {

public:

    struct waiter {
        std::mutex m;
        std::condition_variable cv;
    };

    virtual ~AsyncInputStrategy() {};
    virtual void startInput(bool mouse = true) = 0;
    virtual void endInput() = 0;
    virtual bool getEvent(TEvent &ev) = 0;
    virtual int getButtonCount() = 0;

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
    std::shared_ptr<AsyncInputStrategy> input;

public:

    PlatformStrategy(DisplayStrategy* d, AsyncInputStrategy *i) : display(d), input(i)
    {
        display->startDisplay();
        input->startInput();
    }

    virtual ~PlatformStrategy()
    {
        input->endInput();
        display->endDisplay();
    }

    void resumeListening() { input->resumeListening(); }
    bool waitForEvent(long ms, TEvent &ev) { return input->waitForEvent(ms, ev); }
    void notifyEvent(TEvent &ev, AsyncInputStrategy::waiter &get) { input->notifyEvent(ev, get); }
    int getButtonCount() { return input->getButtonCount(); }

    inline int getCaretSize() { return display->getCaretSize(); }
    inline bool isCaretVisible() { return display->isCaretVisible(); }
    inline void clearScreen() { display->clearScreen(); }
    inline int getScreenRows() { return display->getScreenRows(); }
    inline int getScreenCols() { return display->getScreenCols(); }
    inline void setCaretPosition(int x, int y) { display->setCaretPosition(x, y); }
    inline ushort getScreenMode() { return display->getScreenMode(); }
    inline void setCaretSize(int size) { display->setCaretSize(size); }
    inline void screenWrite(int x, int y, ushort *buf, int len) { display->screenWrite(x, y, buf, len); }
    inline void flushScreen() { display->flushScreen(); }

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
    void flushScreen();

};

class NcursesInput : public AsyncInputStrategy {

    std::unordered_map<std::string, char> Utf8toCp437;
    static std::unordered_map<int, KeyDownEvent> fromCursesKeyCode;
    static std::unordered_map<std::string, KeyDownEvent> fromCursesHighKey;

    std::thread inputThread;
    TPoint lastMousePos;
    uchar buttonState;
    int buttonCount;

    void detectAlt(int keys[4], bool &Alt);
    void parsePrintableChar(TEvent &ev, int keys[4], int &num_keys);
    void setAltModifier(TEvent &ev);
    void readUtf8Char(int keys[4], int &num_keys);
    bool parseMouseEvent(TEvent&);

public:

    void startInput(bool mouse);
    void endInput();
    bool getEvent(TEvent &ev);
    int getButtonCount();

};

#endif


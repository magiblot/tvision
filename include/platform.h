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
#include <functional>

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

public:

    struct waiter {
        std::mutex m;
        std::condition_variable cv;
    };

    virtual ~AsyncInputStrategy() {}
    virtual void startInputThread();
    void startInputThread(std::function<bool (TEvent&)>);
    virtual void endInputThread();
    virtual bool getEvent(TEvent &ev) = 0;
    virtual int getButtonCount() = 0;

    static void resumeListening();
    static bool waitForEvent(long ms, TEvent &ev);
    static void notifyEvent(TEvent &ev, waiter &get);

protected:

    std::thread inputThread;

private:

    static waiter *inputListener;
    static waiter eventRequester;
    static std::mutex notifying;

    static bool evReceived;
    static bool evProcessed;
    static TEvent received;

};

class PlatformStrategy {

    std::unique_ptr<DisplayStrategy> display;
    std::unique_ptr<AsyncInputStrategy> input;

public:

    PlatformStrategy(DisplayStrategy* d, AsyncInputStrategy *i) : display(d), input(i)
    {
        if (input) input->startInputThread();
    }

    virtual ~PlatformStrategy()
    {
        if (input) input->endInputThread();
    }

    inline void resumeListening() { AsyncInputStrategy::resumeListening(); }
    inline bool waitForEvent(long ms, TEvent &ev) { return AsyncInputStrategy::waitForEvent(ms, ev); }
    inline int getButtonCount() { return input ? input->getButtonCount() : 0; }

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

    NcursesDisplay();
    ~NcursesDisplay();

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

    static std::unordered_map<int, KeyDownEvent> fromCursesKeyCode;
    static std::unordered_map<std::string, KeyDownEvent> fromCursesHighKey;

    TPoint lastMousePos;
    uchar buttonState;
    int buttonCount;

    void detectAlt(int keys[4], bool &Alt);
    void parsePrintableChar(TEvent &ev, int keys[4], int &num_keys);
    void setAltModifier(TEvent &ev);
    void readUtf8Char(int keys[4], int &num_keys);
    bool parseMouseEvent(TEvent&);

public:

    NcursesInput(bool mouse = true);
    ~NcursesInput();

    bool getEvent(TEvent &ev);
    int getButtonCount();

};

class GpmInput : public AsyncInputStrategy {

    TPoint mousePos;
    uchar buttonState;
    int buttonCount;

public:

    GpmInput();
    ~GpmInput();
    bool getEvent(TEvent &ev);
    int getButtonCount();
    void drawPointer();

};

class LinuxConsoleStrategy : public PlatformStrategy {

    std::unique_ptr<AsyncInputStrategy> keyboard;

    bool patchKeyEvent(TEvent &ev);
    static void applyKeyboardModifiers(KeyDownEvent &key);

public:

    LinuxConsoleStrategy(DisplayStrategy*, AsyncInputStrategy*);
    ~LinuxConsoleStrategy();

};

#endif


#ifndef WIN32CON_H
#define WIN32CON_H

#include <internal/buffdisp.h>

#ifdef _WIN32

#include <tvision/tv.h>
#include <tvision/borland/win.h>

class Win32Input;
class Win32Display;

class Win32ConsoleStrategy : public PlatformStrategy
{

    friend class Win32Display;
    friend class Win32Input;

    enum ConsoleType { cnInput = 0, cnOutput = 1, cnStartup = 2 };

    HANDLE consoleHandle[3];
    UINT consoleCp[2];
    DWORD consoleMode[2];
    DWORD pendingEvent;
    CONSOLE_CURSOR_INFO crInfo;
    CONSOLE_SCREEN_BUFFER_INFO sbInfo;

    void initConsole();
    void reloadScreenBufferInfo();
    void resetConsole();

    static Win32ConsoleStrategy *instance;

public:

    Win32ConsoleStrategy();
    ~Win32ConsoleStrategy();

    bool waitForEvent(long ms, TEvent &ev) override;

    static void write(const void *data, size_t bytes);

};

inline void Win32ConsoleStrategy::write(const void *data, size_t bytes)
{
    if (instance)
        WriteConsole(instance->consoleHandle[cnOutput], data, bytes, nullptr, nullptr);
}

class Win32Input : public InputStrategy
{

    Win32ConsoleStrategy &cnState;
    bool insertState;
    ushort surrogate;

    bool getKeyEvent(KEY_EVENT_RECORD, TEvent &ev);
    bool getUnicodeEvent(KEY_EVENT_RECORD, TEvent &ev);
    bool getMouseEvent(MOUSE_EVENT_RECORD, TEvent &ev);

    HANDLE cnHandle() const;

public:

    Win32Input(Win32ConsoleStrategy &);

    int getButtonCount() override;
    void cursorOn() override;
    void cursorOff() override;
    bool getEvent(TEvent &ev);

};

inline HANDLE Win32Input::cnHandle() const
{
    return cnState.consoleHandle[0];
}

class Win32Display : public BufferedDisplay
{

    Win32ConsoleStrategy &cnState;

    uchar lastAttr;
    std::vector<char> buf;

    HANDLE cnHandle() const;

public:

    Win32Display(Win32ConsoleStrategy &);

    void setCaretSize(int size) override;
    int getCaretSize() override;
    bool isCaretVisible() override;
    void clearScreen() override;
    int getScreenRows() override;
    int getScreenCols() override;
    ushort getScreenMode() override;

protected:

    void lowlevelWriteChars(const uchar chars[4], TCellAttribs attr) override;
    void lowlevelMoveCursor(uint x, uint y) override;
    void lowlevelFlush() override;

};

inline HANDLE Win32Display::cnHandle() const
{
    return cnState.consoleHandle[1];
}

#endif // _WIN32

#endif // WIN32CON_H

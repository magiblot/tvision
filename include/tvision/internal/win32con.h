#ifndef WIN32CON_H
#define WIN32CON_H

#include <internal/termdisp.h>

#ifdef _WIN32

#include <tvision/tv.h>
#include <tvision/compat/win.h>

class Win32Input;
class Win32Display;

class Win32ConsoleStrategy final : public PlatformStrategy
{
    EventWaiter waiter;
    UINT cpInput, cpOutput;

    Win32ConsoleStrategy( UINT cpInput, UINT cpOutput,
                          DisplayStrategy *display,
                          InputStrategy *input ) noexcept;
    ~Win32ConsoleStrategy();

    static bool initConsole( UINT &cpInput, UINT &cpOutput,
                             std::unique_ptr<DisplayStrategy> &display,
                             std::unique_ptr<InputStrategy> &input ) noexcept;
    void restoreConsole() noexcept;
    void resetConsole() noexcept;

public:

    static Win32ConsoleStrategy *create() noexcept;

    bool getEvent(TEvent &ev) noexcept override;
    void waitForEvents(int ms) noexcept override;
    void stopEventWait() noexcept override;

    int charWidth(TStringView mbc, char32_t wc) noexcept override; // ttext.cpp
};

class Win32Input : public InputStrategy
{

    bool insertState;
    ushort surrogate;

    bool getEvent(const INPUT_RECORD &, TEvent &ev) noexcept;
    bool getKeyEvent(KEY_EVENT_RECORD, TEvent &ev) noexcept;
    bool getUnicodeEvent(KEY_EVENT_RECORD, TEvent &ev) noexcept;
    bool getMouseEvent(MOUSE_EVENT_RECORD, TEvent &ev) noexcept;

public:

    Win32Input() noexcept;

    int getButtonCount() noexcept override;
    void cursorOn() noexcept override;
    void cursorOff() noexcept override;
    bool getEvent(TEvent &ev) noexcept;

};

class Win32Display : public TerminalDisplay
{

    COORD dwSize;
    uchar lastAttr;
    std::vector<char> buf;

public:

    Win32Display() noexcept;

    void reloadScreenInfo() noexcept override;
    TPoint getScreenSize() noexcept override;
    int getCaretSize() noexcept override;
    int getColorCount() noexcept override;

    void clearScreen() noexcept override;

protected:

    void lowlevelWriteChars(TStringView chars, TColorAttr attr) noexcept override;
    void lowlevelMoveCursor(uint x, uint y) noexcept override;
    void lowlevelCursorSize(int size) noexcept override;
    void lowlevelFlush() noexcept override;

};

#endif // _WIN32

#endif // WIN32CON_H

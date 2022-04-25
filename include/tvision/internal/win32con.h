#ifndef WIN32CON_H
#define WIN32CON_H

#include <internal/termdisp.h>

#ifdef _WIN32

#include <tvision/tv.h>
#include <tvision/compat/windows/windows.h>
#include <internal/stdioctl.h>

class Win32Input;
class Win32Display;

class Win32ConsoleStrategy final : public ConsoleStrategy
{
    const StdioCtl &io;
    UINT cpInput, cpOutput;


    Win32ConsoleStrategy( const StdioCtl &aIo,
                          UINT cpInput, UINT cpOutput,
                          DisplayStrategy &aDisplay,
                          InputStrategy &aInput ) noexcept :
        ConsoleStrategy(aDisplay, aInput),
        io(aIo),
        cpInput(cpInput),
        cpOutput(cpOutput)
    {
    }

    ~Win32ConsoleStrategy();

    bool isAlive() noexcept override;
    void forEachSource(void *, void (&)(void *, EventSource &)) noexcept override;

public:

    static Win32ConsoleStrategy &create() noexcept;
    static int charWidth(uint32_t) noexcept;
};

class Win32Input final : public InputStrategy
{
    const StdioCtl &io;
    bool insertState {true};
    ushort surrogate {0};

    bool getEvent(const INPUT_RECORD &, TEvent &ev) noexcept;
    bool getKeyEvent(KEY_EVENT_RECORD, TEvent &ev) noexcept;
    bool getUnicodeEvent(KEY_EVENT_RECORD, TEvent &ev) noexcept;
    bool getMouseEvent(MOUSE_EVENT_RECORD, TEvent &ev) noexcept;

public:

    // The lifetime of 'aIo' must exceed that of 'this'.
    Win32Input(const StdioCtl &aIo) noexcept :
        InputStrategy(aIo.in()),
        io(aIo)
    {
    }

    int getButtonCount() noexcept override;
    void cursorOn() noexcept override;
    void cursorOff() noexcept override;
    bool getEvent(TEvent &ev) noexcept override;
};

class Win32Display : public TerminalDisplay
{
    TPoint size {};
    uchar lastAttr {'\x00'};
    std::vector<char> buf;
    CONSOLE_FONT_INFO lastFontInfo {};

public:

    // The lifetime of 'aIo' must exceed that of 'this'.
    Win32Display(const StdioCtl &aIo) noexcept :
        TerminalDisplay(aIo)
    {
        initCapabilities();
    }

    void reloadScreenInfo() noexcept override;
    TPoint getScreenSize() noexcept override;
    int getCaretSize() noexcept override;
    int getColorCount() noexcept override;
    void clearScreen() noexcept override;
    bool screenChanged() noexcept override;

protected:

    void lowlevelWriteChars(TStringView chars, TColorAttr attr) noexcept override;
    void lowlevelMoveCursor(uint x, uint y) noexcept override;
    void lowlevelCursorSize(int size) noexcept override;
    void lowlevelFlush() noexcept override;
};

#endif // _WIN32

#endif // WIN32CON_H

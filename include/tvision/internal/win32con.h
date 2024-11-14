#ifndef WIN32CON_H
#define WIN32CON_H

#include <tvision/tv.h>
#include <compat/windows/windows.h>
#include <internal/termdisp.h>
#include <internal/termio.h>
#include <internal/ansiwrit.h>

namespace tvision
{

bool getWin32Key(const KEY_EVENT_RECORD &, TEvent &, InputState &) noexcept;
void getWin32Mouse(const MOUSE_EVENT_RECORD &, TEvent &, InputState &) noexcept;

#ifdef _WIN32

class ConsoleCtl;
class Win32Input;
class Win32Display;

class Win32ConsoleStrategy final : public ConsoleStrategy
{
    ConsoleCtl &con;
    UINT cpInput, cpOutput;

    Win32ConsoleStrategy( ConsoleCtl &aCon,
                          UINT cpInput, UINT cpOutput,
                          DisplayStrategy &aDisplay,
                          InputStrategy &aInput ) noexcept :
        ConsoleStrategy(aDisplay, aInput, {&aInput}),
        con(aCon),
        cpInput(cpInput),
        cpOutput(cpOutput)
    {
    }

    ~Win32ConsoleStrategy();

    bool isAlive() noexcept override;
    bool setClipboardText(TStringView) noexcept override;
    bool requestClipboardText(void (&)(TStringView)) noexcept override;

public:

    static Win32ConsoleStrategy &create() noexcept;
    static int charWidth(uint32_t) noexcept;
};

class Win32Input final : public InputStrategy
{
    ConsoleCtl &con;
    InputState state;

    bool getEvent(const INPUT_RECORD &, TEvent &ev) noexcept;
    bool getMouseEvent(MOUSE_EVENT_RECORD, TEvent &ev) noexcept;

public:

    // The lifetime of 'con' must exceed that of 'this'.
    Win32Input(ConsoleCtl &aCon) noexcept;

    bool getEvent(TEvent &ev) noexcept override;
    int getButtonCount() noexcept override;
    void cursorOn() noexcept override;
    void cursorOff() noexcept override;
};

class Win32Display : public TerminalDisplay
{
public:

    // The lifetime of 'con' must exceed that of 'this'.
    Win32Display(ConsoleCtl &con, bool useAnsi) noexcept;
    ~Win32Display();

private:

    TPoint size {};
    CONSOLE_FONT_INFO lastFontInfo {};

    AnsiScreenWriter *ansiScreenWriter;

    uchar lastAttr {'\x00'};
    std::vector<char> buf;

protected:

    TPoint reloadScreenInfo() noexcept override;
    void clearScreen() noexcept override;

    int getColorCount() noexcept override;

    void lowlevelWriteChars(TStringView chars, TColorAttr attr) noexcept override;
    void lowlevelMoveCursor(uint x, uint y) noexcept override;
    void lowlevelMoveCursorX(uint x, uint y) noexcept override;
    void lowlevelCursorSize(int size) noexcept override;
    void lowlevelFlush() noexcept override;
};

#endif // _WIN32

} // namespace tvision

#endif // WIN32CON_H

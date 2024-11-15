#ifndef WIN32CON_H
#define WIN32CON_H

#include <tvision/tv.h>
#include <compat/windows/windows.h>
#include <internal/platform.h>
#include <internal/termio.h>
#include <internal/ansiwrit.h>

namespace tvision
{

bool getWin32Key(const KEY_EVENT_RECORD &, TEvent &, InputState &) noexcept;
void getWin32Mouse(const MOUSE_EVENT_RECORD &, TEvent &, InputState &) noexcept;

#ifdef _WIN32

class ConsoleCtl;

class Win32ConsoleAdapter final : public ConsoleAdapter
{
    ConsoleCtl &con;
    InputAdapter &input;
    DWORD startupMode;
    UINT cpInput, cpOutput;

    Win32ConsoleAdapter( ConsoleCtl &aCon, DWORD aStartupMode,
                         UINT cpInput, UINT cpOutput,
                         DisplayAdapter &aDisplay,
                         InputAdapter &aInput ) noexcept :
        ConsoleAdapter(aDisplay, {&aInput}),
        con(aCon),
        input(aInput),
        startupMode(aStartupMode),
        cpInput(cpInput),
        cpOutput(cpOutput)
    {
    }

    ~Win32ConsoleAdapter();

    bool isAlive() noexcept override;
    bool setClipboardText(TStringView) noexcept override;
    bool requestClipboardText(void (&)(TStringView)) noexcept override;

public:

    static Win32ConsoleAdapter &create() noexcept;
    static int charWidth(uint32_t) noexcept;
};

class Win32Input final : public InputAdapter
{
    ConsoleCtl &con;
    InputState state;

    bool getEvent(const INPUT_RECORD &, TEvent &ev) noexcept;
    bool getMouseEvent(MOUSE_EVENT_RECORD, TEvent &ev) noexcept;

public:

    // The lifetime of 'con' must exceed that of 'this'.
    Win32Input(ConsoleCtl &aCon) noexcept;

    bool getEvent(TEvent &ev) noexcept override;
};

class Win32Display final : public DisplayAdapter
{
public:

    // The lifetime of 'con' must exceed that of 'this'.
    Win32Display(ConsoleCtl &con, bool useAnsi) noexcept;
    ~Win32Display();

private:

    ConsoleCtl &con;

    TPoint size {};
    CONSOLE_FONT_INFO lastFontInfo {};

    AnsiScreenWriter *ansiScreenWriter {nullptr};

    TPoint caretPos {-1, -1};
    uchar lastAttr {'\x00'};
    std::vector<char> buf;

    TPoint reloadScreenInfo() noexcept override;

    int getColorCount() noexcept override;
    TPoint getFontSize() noexcept override;

    void writeCell(TPoint, TStringView, TColorAttr, bool) noexcept override;
    void setCaretPosition(TPoint) noexcept override;
    void setCaretSize(int) noexcept override;
    void clearScreen() noexcept override;
    void flush() noexcept override;
};

#endif // _WIN32

} // namespace tvision

#endif // WIN32CON_H

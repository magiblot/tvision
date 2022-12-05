#ifdef _WIN32

#define Uses_TEvent
#define Uses_TKeys
#define Uses_THardwareInfo
#define Uses_TScreen
#include <tvision/tv.h>
#include <internal/win32con.h>
#include <internal/stdioctl.h>
#include <internal/winwidth.h>
#include <internal/codepage.h>
#include <internal/ansidisp.h>
#include <internal/terminal.h>
#include <locale.h>

namespace tvision
{

Win32ConsoleStrategy &Win32ConsoleStrategy::create() noexcept
{
    auto &io = *new StdioCtl;
    // Set the input mode.
    {
        DWORD consoleMode = 0;
        GetConsoleMode(io.in(), &consoleMode);
        consoleMode |= ENABLE_WINDOW_INPUT; // Report changes in buffer size
        consoleMode &= ~ENABLE_PROCESSED_INPUT; // Report CTRL+C and SHIFT+Arrow events.
        consoleMode |= ENABLE_EXTENDED_FLAGS;   /* Disable the Quick Edit mode, */
        consoleMode &= ~ENABLE_QUICK_EDIT_MODE; /* which inhibits the mouse.    */
        SetConsoleMode(io.in(), consoleMode);
    }
    // Set the output mode.
    bool supportsVT;
    {
        DWORD consoleMode = 0;
        GetConsoleMode(io.out(), &consoleMode);
        consoleMode &= ~ENABLE_WRAP_AT_EOL_OUTPUT; // Avoid scrolling when reaching end of line.
        SetConsoleMode(io.out(), consoleMode);
        // Try enabling VT sequences.
        consoleMode |= DISABLE_NEWLINE_AUTO_RETURN; // Do not do CR on LF.
        consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING; // Allow ANSI escape sequences.
        SetConsoleMode(io.out(), consoleMode);
        GetConsoleMode(io.out(), &consoleMode);
        supportsVT = consoleMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    }

    // Set the console and the environment in UTF-8 mode.
    UINT cpInput = GetConsoleCP();
    UINT cpOutput = GetConsoleOutputCP();
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, ".utf8"); // Note that this must be done again after SetConsoleCP().
    if (!supportsVT)
    {
        // Disable bitmap font in legacy console because multibyte characters
        // are not displayed correctly.
        CONSOLE_FONT_INFOEX fontInfo {};
        fontInfo.cbSize = sizeof(fontInfo);
        auto isBitmap = [](UINT family)
        {
            // https://docs.microsoft.com/en-us/windows/console/console-font-infoex
            // "FontFamily: see the description of the tmPitchAndFamily member
            //  of the TEXTMETRIC structure."
            // https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-textmetricw
            // "A monospace bitmap font has all of these low-order bits clear".
            return !(family & (TMPF_FIXED_PITCH | TMPF_VECTOR | TMPF_TRUETYPE | TMPF_DEVICE));
        };
        if ( GetCurrentConsoleFontEx(io.out(), FALSE, &fontInfo)
             && isBitmap(fontInfo.FontFamily) )
        {
            // Compute the new font height based on the bitmap font size.
            short fontY = 2*min(fontInfo.dwFontSize.X, fontInfo.dwFontSize.Y);
            for (auto *name : {L"Consolas", L"Lucida Console"})
            {
                fontInfo.nFont = 0;
                fontInfo.FontFamily = FF_DONTCARE;
                fontInfo.FontWeight = FW_NORMAL;
                fontInfo.dwFontSize = {0, fontY}; // Width estimated automatically, it seems.
                wcscpy(fontInfo.FaceName, name);
                // SetCurrentConsoleFontEx succeeds even if the font is not available.
                // We need to check whether the font has actually been set.
                SetCurrentConsoleFontEx(io.out(), FALSE, &fontInfo);
                GetCurrentConsoleFontEx(io.out(), FALSE, &fontInfo);
                if (wcscmp(fontInfo.FaceName, name) == 0)
                    break;
            }
        }
    }
    WinWidth::reset();
    auto &display = supportsVT ? *new AnsiDisplay<Win32Display>(io)
                               : *new Win32Display(io);
    auto &input = *new Win32Input(io);
    return *new Win32ConsoleStrategy(io, cpInput, cpOutput, display, input);
}

Win32ConsoleStrategy::~Win32ConsoleStrategy()
{
    delete &display;
    delete &input;
    SetConsoleCP(cpInput);
    SetConsoleOutputCP(cpOutput);
    delete &io;
}

bool Win32ConsoleStrategy::isAlive() noexcept
{
    DWORD events = 0;
    return GetNumberOfConsoleInputEvents(io.in(), &events);
}

void Win32ConsoleStrategy::forEachSource(void *args, void (&action)(void *, EventSource &)) noexcept
{
    action(args, input);
}

/////////////////////////////////////////////////////////////////////////
// Win32Input

int Win32Input::getButtonCount() noexcept
{
    DWORD num;
    GetNumberOfConsoleMouseButtons(&num);
    return num;
}

void Win32Input::cursorOn() noexcept
{
    DWORD consoleMode = 0;
    GetConsoleMode(io.in(), &consoleMode);
    SetConsoleMode(io.in(), consoleMode | ENABLE_MOUSE_INPUT);
}

void Win32Input::cursorOff() noexcept
{
    DWORD consoleMode = 0;
    GetConsoleMode(io.in(), &consoleMode);
    SetConsoleMode(io.in(), consoleMode & ~ENABLE_MOUSE_INPUT);
}

bool Win32Input::getEvent(TEvent &ev) noexcept
{
    // ReadConsoleInput can sleep the process, so we first check the number
    // of available input events.
    DWORD events;
    while (GetNumberOfConsoleInputEvents(io.in(), &events) && events)
    {
        // getEvent(ir, ev) often returns false due to discarded events. But this
        // function should not return false if there are pending events, as that
        // defeats the event queue in THardwareInfo.
        while (events--)
        {
            INPUT_RECORD ir;
            DWORD ok;
            if (!ReadConsoleInputW(io.in(), &ir, 1, &ok) || !ok)
                return false;
            if (getEvent(ir, ev))
                return true;
        }
    }
    return false;
}

bool Win32Input::getEvent(const INPUT_RECORD &ir, TEvent &ev) noexcept
{
    switch (ir.EventType)
    {
    case KEY_EVENT:
        if ( ir.Event.KeyEvent.bKeyDown || // KeyDown
             (ir.Event.KeyEvent.wVirtualKeyCode == VK_MENU && // Pasted surrogate character
                ir.Event.KeyEvent.uChar.UnicodeChar) )
            return TermIO::getWin32Key(ir.Event.KeyEvent, ev, state);
        break;
    case MOUSE_EVENT:
        return getMouseEvent(ir.Event.MouseEvent, ev);
    case WINDOW_BUFFER_SIZE_EVENT:
        ev.what = evCommand;
        ev.message.command = cmScreenChanged;
        ev.message.infoPtr = 0;
        return True;
    }
    return false;
}

bool Win32Input::getMouseEvent(MOUSE_EVENT_RECORD MouseEvent, TEvent &ev) noexcept
{
    ev.what = evMouse;
    ev.mouse.where.x = MouseEvent.dwMousePosition.X;
    ev.mouse.where.y = MouseEvent.dwMousePosition.Y;
    ev.mouse.buttons = MouseEvent.dwButtonState;
    ev.mouse.eventFlags = MouseEvent.dwEventFlags;
    ev.mouse.controlKeyState = MouseEvent.dwControlKeyState;

    // Rotation sense is represented by the sign of dwButtonState's high word
    Boolean positive = !(MouseEvent.dwButtonState & 0x80000000);
    if( MouseEvent.dwEventFlags & MOUSE_WHEELED )
        ev.mouse.wheel = positive ? mwUp : mwDown;
    else if( MouseEvent.dwEventFlags & MOUSE_HWHEELED )
        ev.mouse.wheel = positive ? mwRight : mwLeft;
    else
        ev.mouse.wheel = 0;

    return true;
}

/////////////////////////////////////////////////////////////////////////
// Win32Display

void Win32Display::reloadScreenInfo() noexcept
{
    size = io.getSize();
    // Set the buffer size to the viewport size so that the scrollbars
    // do not become visible after resizing.
    CONSOLE_SCREEN_BUFFER_INFO sbInfo {};
    GetConsoleScreenBufferInfo(io.out(), &sbInfo);
    // Set the cursor temporally to (0, 0) to prevent the console from crashing due to a bug.
    auto curPos = sbInfo.dwCursorPosition;
    SetConsoleCursorPosition(io.out(), {0, 0});
    // Resize the buffer.
    SetConsoleScreenBufferSize(io.out(), {(short) size.x, (short) size.y});
    // Restore the cursor position (it does not matter if it is out of bounds).
    SetConsoleCursorPosition(io.out(), curPos);
}

bool Win32Display::screenChanged() noexcept
{
    bool changed = TerminalDisplay::screenChanged();
    CONSOLE_FONT_INFO fontInfo;
    if ( GetCurrentConsoleFont(io.out(), FALSE, &fontInfo)
         && memcmp(&fontInfo, &lastFontInfo, sizeof(fontInfo)) != 0 )
    {
        changed = true;
        WinWidth::reset();
        lastFontInfo = fontInfo;
    }
    return changed;
}

TPoint Win32Display::getScreenSize() noexcept
{
    return size;
}

int Win32Display::getCaretSize() noexcept
{
    CONSOLE_CURSOR_INFO crInfo {};
    GetConsoleCursorInfo(io.out(), &crInfo);
    return crInfo.bVisible ? crInfo.dwSize : 0;
}

int Win32Display::getColorCount() noexcept
{
    // Conhost has had high color support for some time:
    // https://devblogs.microsoft.com/commandline/24-bit-color-in-the-windows-console/
    DWORD consoleMode = 0;
    GetConsoleMode(io.out(), &consoleMode);
    if (consoleMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)
        return 256*256*256;
    return 16;
}

void Win32Display::lowlevelCursorSize(int size) noexcept
{
    CONSOLE_CURSOR_INFO crInfo;
    if (size) {
        crInfo.bVisible = TRUE;
        crInfo.dwSize = size;
    } else {
        crInfo.bVisible = FALSE;
        crInfo.dwSize = 1;
    }
    SetConsoleCursorInfo(io.out(), &crInfo);
}

void Win32Display::clearScreen() noexcept
{
    COORD coord = {0, 0};
    DWORD length = size.x * size.y;
    BYTE attr = 0x07;
    DWORD read;
    FillConsoleOutputAttribute(io.out(), attr, length, coord, &read);
    FillConsoleOutputCharacter(io.out(), ' ', length, coord, &read);
    lastAttr = attr;
}

// Fallback display support with rudimentary buffering.

void Win32Display::lowlevelWriteChars(TStringView chars, TColorAttr attr) noexcept
{
    uchar bios = attr.toBIOS();
    if (bios != lastAttr)
    {
        lowlevelFlush();
        SetConsoleTextAttribute(io.out(), bios);
        lastAttr = bios;
    }
    buf.insert(buf.end(), chars.data(), chars.data()+chars.size());
}

void Win32Display::lowlevelMoveCursor(uint x, uint y) noexcept
{
    lowlevelFlush();
    SetConsoleCursorPosition(io.out(), {(short) x, (short) y});
}

void Win32Display::lowlevelFlush() noexcept
{
    io.write(buf.data(), buf.size());
    buf.resize(0);
}

} // namespace tvision

#endif // _WIN32

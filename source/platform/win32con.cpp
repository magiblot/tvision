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
#include <clocale>

Win32ConsoleStrategy* Win32ConsoleStrategy::create() noexcept
{
    UINT cpInput, cpOutput;
    std::unique_ptr<DisplayStrategy> display;
    std::unique_ptr<InputStrategy> input;
    if (initConsole(cpInput, cpOutput, display, input))
    {
        return new Win32ConsoleStrategy( cpInput, cpOutput,
                                         display.release(),
                                         input.release() );
    }
    return nullptr;
}

Win32ConsoleStrategy::Win32ConsoleStrategy( UINT cpInput, UINT cpOutput,
                                            DisplayStrategy *display,
                                            InputStrategy *input ) noexcept :
    PlatformStrategy(display, input),
    cpInput(cpInput),
    cpOutput(cpOutput)
{
    waiter.addSource(*input);
}

Win32ConsoleStrategy::~Win32ConsoleStrategy()
{
    flushScreen();
    restoreConsole();
}

bool Win32ConsoleStrategy::initConsole( UINT &cpInput, UINT &cpOutput,
                                        std::unique_ptr<DisplayStrategy> &display,
                                        std::unique_ptr<InputStrategy> &input ) noexcept
{
    StdioCtl::instance.setUp();
    // Set the input mode.
    {
        DWORD consoleMode = 0;
        if (!GetConsoleMode(StdioCtl::in(), &consoleMode)) return false;
        consoleMode |= ENABLE_WINDOW_INPUT; // Report changes in buffer size
        consoleMode &= ~ENABLE_PROCESSED_INPUT; // Report CTRL+C and SHIFT+Arrow events.
        consoleMode |= ENABLE_EXTENDED_FLAGS;   /* Disable the Quick Edit mode, */
        consoleMode &= ~ENABLE_QUICK_EDIT_MODE; /* which inhibits the mouse.    */
        SetConsoleMode(StdioCtl::in(), consoleMode);
    }
    // Set the output mode.
    bool supportsVT;
    {
        DWORD consoleMode = 0;
        if (!GetConsoleMode(StdioCtl::out(), &consoleMode)) return false;
        consoleMode &= ~ENABLE_WRAP_AT_EOL_OUTPUT; // Avoid scrolling when reaching end of line.
        SetConsoleMode(StdioCtl::out(), consoleMode);
        // Try enabling VT sequences.
        consoleMode |= DISABLE_NEWLINE_AUTO_RETURN; // Do not do CR on LF.
        consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING; // Allow ANSI escape sequences.
        SetConsoleMode(StdioCtl::out(), consoleMode);
        GetConsoleMode(StdioCtl::out(), &consoleMode);
        supportsVT = consoleMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    }

    // Set the console and the environment in UTF-8 mode.
    cpInput = GetConsoleCP();
    cpOutput = GetConsoleOutputCP();
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, ".utf8"); // Note that this must be done after SetConsoleCP().
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
        if ( GetCurrentConsoleFontEx(StdioCtl::out(), FALSE, &fontInfo)
             && isBitmap(fontInfo.FontFamily) )
        {
            // Compute the new font height based on the bitmap font size.
            auto &oldSize = fontInfo.dwFontSize;
            short fontY = 2*min(oldSize.X, oldSize.Y);
            for (auto *name : {L"Consolas", L"Lucida Console"})
            {
                fontInfo.nFont = 0;
                fontInfo.FontFamily = FF_DONTCARE;
                fontInfo.FontWeight = FW_NORMAL;
                fontInfo.dwFontSize = {0, fontY}; // Width estimated automatically, it seems.
                wcscpy(fontInfo.FaceName, name);
                // SetCurrentConsoleFontEx succeeds even if the font is not available.
                // We need to check whether the font has actually been set.
                SetCurrentConsoleFontEx(StdioCtl::out(), FALSE, &fontInfo);
                GetCurrentConsoleFontEx(StdioCtl::out(), FALSE, &fontInfo);
                if (wcscmp(fontInfo.FaceName, name) == 0)
                    break;
            }
        }
    }
    // Initialize the input and display strategies.
    display = supportsVT ? std::make_unique<AnsiDisplay<Win32Display>>()
                         : std::make_unique<Win32Display>();
    input = std::make_unique<Win32Input>();
    return true;
}

void Win32ConsoleStrategy::restoreConsole() noexcept
{
    // Restore the startup codepages and screen buffer.
    SetConsoleCP(cpInput);
    SetConsoleOutputCP(cpOutput);
    WinWidth::clearState();
    StdioCtl::instance.tearDown();
    waiter.removeSource(*input);
}

void Win32ConsoleStrategy::resetConsole() noexcept
{
    // Attach the application to a new console with no data loss.
    restoreConsole();
    initConsole(cpInput, cpOutput, display, input);
    waiter.addSource(*input);
}

bool Win32ConsoleStrategy::getEvent(TEvent &ev) noexcept
{
    return waiter.getEvent(ev);
}

void Win32ConsoleStrategy::waitForEvents(int ms) noexcept
{
    DWORD events = 0;
    if (!GetNumberOfConsoleInputEvents(StdioCtl::in(), &events))
        // The console likely crashed.
        resetConsole();
    waiter.waitForEvents(ms);
}

void Win32ConsoleStrategy::stopEventWait() noexcept
{
    waiter.stopEventWait();
}

/////////////////////////////////////////////////////////////////////////
// Win32Input

Win32Input::Win32Input() noexcept :
    InputStrategy(StdioCtl::in()),
    insertState(true),
    surrogate(0)
{
}

int Win32Input::getButtonCount() noexcept
{
    DWORD num;
    GetNumberOfConsoleMouseButtons(&num);
    return num;
}

void Win32Input::cursorOn() noexcept
{
    DWORD consoleMode = 0;
    GetConsoleMode(StdioCtl::in(), &consoleMode);
    SetConsoleMode(StdioCtl::in(), consoleMode | ENABLE_MOUSE_INPUT);
}

void Win32Input::cursorOff() noexcept
{
    DWORD consoleMode = 0;
    GetConsoleMode(StdioCtl::in(), &consoleMode);
    SetConsoleMode(StdioCtl::in(), consoleMode & ~ENABLE_MOUSE_INPUT);
}

bool Win32Input::getEvent(TEvent &ev) noexcept
{
    // ReadConsoleInput can sleep the process, so we first check the number
    // of available input events.
    DWORD events;
    if (!GetNumberOfConsoleInputEvents(StdioCtl::in(), &events))
        return false;
    // getEvent(ir, ev) often returns false due to discarded events. But this
    // function should not return false if there are pending events, as that
    // defeats the event queue in THardwareInfo.
    while (events--)
    {
        INPUT_RECORD ir;
        DWORD ok;
        if (!ReadConsoleInputW(StdioCtl::in(), &ir, 1, &ok) || !ok)
            return false;
        if (getEvent(ir, ev))
            return true;
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
            return getKeyEvent(ir.Event.KeyEvent, ev);
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

bool Win32Input::getKeyEvent(KEY_EVENT_RECORD KeyEventW, TEvent &ev) noexcept
{
    if (getUnicodeEvent(KeyEventW, ev)) {
        ev.what = evKeyDown;
        ev.keyDown.charScan.scanCode = KeyEventW.wVirtualScanCode;
        if (ev.keyDown.textLength) {
            ev.keyDown.charScan.charCode = CpTranslator::fromUtf8(ev.keyDown.getText());
            if (KeyEventW.wVirtualKeyCode == VK_MENU)
                // This is enabled when pasting certain characters, and it confuses
                // applications. Clear it.
                ev.keyDown.charScan.scanCode = 0;
            if (!ev.keyDown.charScan.charCode || ev.keyDown.keyCode <= kbCtrlZ) {
                // If the character cannot be represented in the current codepage,
                // or if it would accidentally trigger a Ctrl+Key combination,
                // make the whole keyCode zero to avoid side effects.
                ev.keyDown.keyCode = kbNoKey;
            }
        } else
            ev.keyDown.charScan.charCode = KeyEventW.uChar.AsciiChar;
        ev.keyDown.controlKeyState = KeyEventW.dwControlKeyState;
        // Convert NT style virtual scan codes to PC BIOS codes.
        if ( (ev.keyDown.controlKeyState & kbCtrlShift) &&
             (ev.keyDown.controlKeyState & kbAltShift) ) // Ctrl+Alt is AltGr
        {
            // When AltGr+Key does not produce a character, a
            // keyCode with unwanted effects may be read instead.
            if (!ev.keyDown.charScan.charCode)
                ev.keyDown.keyCode = kbNoKey;
        } else if (KeyEventW.wVirtualScanCode < 89) {
            uchar index = KeyEventW.wVirtualScanCode;
            if ((ev.keyDown.controlKeyState & kbShift) && THardwareInfo::ShiftCvt[index])
                ev.keyDown.keyCode = THardwareInfo::ShiftCvt[index];
            else if ((ev.keyDown.controlKeyState & kbCtrlShift) && THardwareInfo::CtrlCvt[index])
                ev.keyDown.keyCode = THardwareInfo::CtrlCvt[index];
            else if ((ev.keyDown.controlKeyState & kbAltShift) && THardwareInfo::AltCvt[index])
                ev.keyDown.keyCode = THardwareInfo::AltCvt[index];
            else if (THardwareInfo::NormalCvt[index])
                ev.keyDown.keyCode = THardwareInfo::NormalCvt[index];
        }

        // Set/reset insert flag.
        if (ev.keyDown.keyCode == kbIns)
            insertState = !insertState;
        if (insertState)
            ev.keyDown.controlKeyState |= kbInsState;

        return ev.keyDown.keyCode != kbNoKey || ev.keyDown.textLength;
    }
    return false;
}

bool Win32Input::getUnicodeEvent(KEY_EVENT_RECORD KeyEventW, TEvent &ev) noexcept
// Returns true unless the event contains a UTF-16 surrogate,
// in which case we need the next event.
{
    ushort utf16[2] = {KeyEventW.uChar.UnicodeChar, 0};
    ev.keyDown.textLength = 0;
    // Do not treat non-printable characters as text.
    if (' ' <= utf16[0] && utf16[0] != 0x7F) {
        if (0xD800 <= utf16[0] && utf16[0] <= 0xDBFF) {
            surrogate = utf16[0];
            return false;
        } else {
            if (surrogate) {
                if (0xDC00 <= utf16[0] && utf16[0] <= 0xDFFF) {
                    utf16[1] = utf16[0];
                    utf16[0] = surrogate;
                }
                surrogate = 0;
            }
            ev.keyDown.textLength = WideCharToMultiByte(
                CP_UTF8, 0,
                (wchar_t*) utf16, utf16[1] ? 2 : 1,
                ev.keyDown.text, sizeof(ev.keyDown.text),
                nullptr, nullptr );
        }
    }
    return true;
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

Win32Display::Win32Display() noexcept :
    dwSize {},
    lastAttr('\x00')
{
}

void Win32Display::reloadScreenInfo() noexcept
{
    CONSOLE_SCREEN_BUFFER_INFO sbInfo {};
    // Set the buffer size to the viewport size so that the scrollbars
    // do not become visible after resizing.
    GetConsoleScreenBufferInfo(StdioCtl::out(), &sbInfo);
    sbInfo.dwSize.X = sbInfo.srWindow.Right - sbInfo.srWindow.Left + 1;
    sbInfo.dwSize.Y = sbInfo.srWindow.Bottom - sbInfo.srWindow.Top + 1;
    // Set the cursor temporally to (0, 0) to prevent the console from crashing due to a bug.
    auto curPos = sbInfo.dwCursorPosition;
    SetConsoleCursorPosition(StdioCtl::out(), {0, 0});
    // Resize the buffer.
    SetConsoleScreenBufferSize(StdioCtl::out(), sbInfo.dwSize);
    // Restore the cursor position (it does not matter if it is out of bounds).
    SetConsoleCursorPosition(StdioCtl::out(), curPos);
    // Update internal state.
    dwSize = sbInfo.dwSize;
    TerminalDisplay::reloadScreenInfo();
}

TPoint Win32Display::getScreenSize() noexcept
{
    return {dwSize.X, dwSize.Y};
}

int Win32Display::getCaretSize() noexcept
{
    CONSOLE_CURSOR_INFO crInfo {};
    GetConsoleCursorInfo(StdioCtl::out(), &crInfo);
    return crInfo.bVisible ? crInfo.dwSize : 0;
}

int Win32Display::getColorCount() noexcept
{
    // Conhost has had high color support for some time:
    // https://devblogs.microsoft.com/commandline/24-bit-color-in-the-windows-console/
    DWORD consoleMode = 0;
    GetConsoleMode(StdioCtl::out(), &consoleMode);
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
    SetConsoleCursorInfo(StdioCtl::out(), &crInfo);
}

void Win32Display::clearScreen() noexcept
{
    COORD coord = {0, 0};
    DWORD length = dwSize.X * dwSize.Y;
    BYTE attr = 0x07;
    DWORD read;
    FillConsoleOutputAttribute(StdioCtl::out(), attr, length, coord, &read);
    FillConsoleOutputCharacter(StdioCtl::out(), ' ', length, coord, &read);
    lastAttr = attr;
}

// Fallback display support with rudimentary buffering.

void Win32Display::lowlevelWriteChars(TStringView chars, TColorAttr attr) noexcept
{
    uchar bios = attr.toBIOS();
    if (bios != lastAttr)
    {
        lowlevelFlush();
        SetConsoleTextAttribute(StdioCtl::out(), bios);
        lastAttr = bios;
    }
    buf.insert(buf.end(), chars.data(), chars.data()+chars.size());
}

void Win32Display::lowlevelMoveCursor(uint x, uint y) noexcept
{
    lowlevelFlush();
    SetConsoleCursorPosition(StdioCtl::out(), {(short) x, (short) y});
}

void Win32Display::lowlevelFlush() noexcept
{
    TermIO::consoleWrite(buf.data(), buf.size());
    buf.resize(0);
}
#endif // _WIN32

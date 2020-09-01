#ifdef _WIN32

#define Uses_TEvent
#define Uses_TKeys
#define Uses_THardwareInfo
#define Uses_TScreen
#include <tvision/tv.h>
#include <internal/win32con.h>
#include <internal/winwidth.h>
#include <internal/codepage.h>
#include <internal/ansidisp.h>

Win32ConsoleStrategy* Win32ConsoleStrategy::instance = 0;

Win32ConsoleStrategy::Win32ConsoleStrategy()
{
    initConsole();
    instance = this;
}

Win32ConsoleStrategy::~Win32ConsoleStrategy()
{
    flushScreen();
    // Restore the startup codepages and screen buffer.
    SetConsoleCP(consoleCp[cnInput]);
    SetConsoleOutputCP(consoleCp[cnOutput]);
    SetConsoleActiveScreenBuffer(consoleHandle[cnStartup]);
    instance = 0;
}

void Win32ConsoleStrategy::initConsole()
{
    consoleHandle[cnInput] = GetStdHandle(STD_INPUT_HANDLE);
    consoleHandle[cnStartup] = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleCursorInfo(consoleHandle[cnStartup], &crInfo);
    GetConsoleScreenBufferInfo(consoleHandle[cnStartup], &sbInfo);
    // Create a new buffer to draw into.
    consoleHandle[cnOutput] = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE,
        0,
        0,
        CONSOLE_TEXTMODE_BUFFER,
        0);
    // Force the screen buffer size to match the window size.
    // The Console API guarantees this, but some implementations
    // are not compliant (e.g. Wine).
    sbInfo.dwSize.X = sbInfo.srWindow.Right - sbInfo.srWindow.Left + 1;
    sbInfo.dwSize.Y = sbInfo.srWindow.Bottom - sbInfo.srWindow.Top + 1;
    SetConsoleScreenBufferSize(consoleHandle[cnOutput], sbInfo.dwSize);
    SetConsoleActiveScreenBuffer(consoleHandle[cnOutput]);
    // Set the input mode.
    GetConsoleMode(consoleHandle[cnInput], &consoleMode[cnInput]);
    consoleMode[cnInput] |= ENABLE_WINDOW_INPUT; // Report changes in buffer size
    consoleMode[cnInput] &= ~ENABLE_PROCESSED_INPUT; // Report CTRL+C and SHIFT+Arrow events.
    SetConsoleMode(consoleHandle[cnInput], consoleMode[cnInput]);
    // Set the output mode.
    GetConsoleMode(consoleHandle[cnOutput], &consoleMode[cnOutput]);
    consoleMode[cnOutput] &= ~ENABLE_WRAP_AT_EOL_OUTPUT; // Avoid scrolling when reaching end of line.
    SetConsoleMode(consoleHandle[cnOutput], consoleMode[cnOutput]);
    // Try enabling VT sequences.
    consoleMode[cnOutput] |= DISABLE_NEWLINE_AUTO_RETURN; // Do not do CR on LF.
    consoleMode[cnOutput] |= ENABLE_VIRTUAL_TERMINAL_PROCESSING; // Allow ANSI escape sequences.
    bool supportsVT = SetConsoleMode(consoleHandle[cnOutput], consoleMode[cnOutput]);
    // Set the console and the environment in UTF-8 mode.
    consoleCp[cnInput] = GetConsoleCP();
    consoleCp[cnOutput] = GetConsoleOutputCP();
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, ".utf8"); // Note that this must be done after SetConsoleCP().
    // Initialize the character width meter, which depends on the console.
    WinWidth::resetState();
    // Initialize the input and display strategies.
    display = supportsVT ? std::make_unique<AnsiDisplay<Win32Display>>(*this)
                         : std::make_unique<Win32Display>(*this);
    input = std::make_unique<Win32Input>(*this);
}

void Win32ConsoleStrategy::resetConsole()
{
    // Open the application in a new console with no data loss.
    FreeConsole();
    AllocConsole();
    initConsole();
}

void Win32ConsoleStrategy::reloadScreenBufferInfo()
{
    // Set the buffer size to the viewport size so that the scrollbars
    // do not become visible after resizing.
    GetConsoleScreenBufferInfo(consoleHandle[cnOutput], &sbInfo);
    sbInfo.dwSize.X = sbInfo.srWindow.Right - sbInfo.srWindow.Left + 1;
    sbInfo.dwSize.Y = sbInfo.srWindow.Bottom - sbInfo.srWindow.Top + 1;
    // Note that this also causes the console to crash often while resizing,
    // hence the console recovery feature.
    SetConsoleScreenBufferSize(consoleHandle[cnOutput], sbInfo.dwSize);
}

bool Win32ConsoleStrategy::waitForEvent(long ms, TEvent &ev)
{
    DWORD events = 0;
    if (!GetNumberOfConsoleInputEvents(consoleHandle[cnInput], &events))
    {
        // The console likely crashed.
        resetConsole();
    }
    if (!events && WaitForSingleObject(consoleHandle[cnInput], ms) == WAIT_OBJECT_0)
    {
        GetNumberOfConsoleInputEvents(consoleHandle[cnInput], &events);
    }
    // getEvent() often return false due to discarded events. But this
    // function should not return false if there are pending events, as that
    // defeats the event queue in THardwareInfo.
    while (events--)
        if (input->getEvent(ev))
            return true;
    return false;

}

/////////////////////////////////////////////////////////////////////////
// Win32Input

Win32Input::Win32Input(Win32ConsoleStrategy &cnState) :
    cnState(cnState),
    insertState(true),
    surrogate(0)
{
}

int Win32Input::getButtonCount()
{
    DWORD num;
    GetNumberOfConsoleMouseButtons(&num);
    return num;
}

void Win32Input::cursorOn()
{
    SetConsoleMode(cnHandle(), cnState.consoleMode[0] | ENABLE_MOUSE_INPUT);
}

void Win32Input::cursorOff() {
    SetConsoleMode(cnHandle(), cnState.consoleMode[0] & ~ENABLE_MOUSE_INPUT);
}

bool Win32Input::getEvent(TEvent &ev)
{
    INPUT_RECORD irBufferW = {};
    DWORD ok = 0;
    if (ReadConsoleInputW(cnHandle(), &irBufferW, 1, &ok) && ok)
    {
        switch (irBufferW.EventType)
        {
        case KEY_EVENT:
            if ( irBufferW.Event.KeyEvent.bKeyDown || // KeyDown
                (irBufferW.Event.KeyEvent.wVirtualKeyCode == VK_MENU && // Pasted surrogate character
                 irBufferW.Event.KeyEvent.uChar.UnicodeChar) )
                return getKeyEvent( irBufferW.Event.KeyEvent,
                                    ev);
            break;
        case MOUSE_EVENT:
            return getMouseEvent(irBufferW.Event.MouseEvent, ev);
        case WINDOW_BUFFER_SIZE_EVENT:
            {
                cnState.reloadScreenBufferInfo();
                ev.what = evCommand;
                ev.message.command = cmScreenChanged;
                ev.message.infoPtr = 0;
                return True;
            }
        }
    }
    return false;
}

bool Win32Input::getKeyEvent( KEY_EVENT_RECORD KeyEventW,
                              TEvent &ev )
{
    if (getUnicodeEvent(KeyEventW, ev)) {
        ev.what = evKeyDown;
        ev.keyDown.charScan.scanCode = KeyEventW.wVirtualScanCode;
        if (ev.keyDown.textLength) {
            ev.keyDown.charScan.charCode = CpTranslator::fromUtf8(ev.keyDown);
            if (!ev.keyDown.charScan.charCode)
                // If the character cannot be represented in the current codepage,
                // make the whole keyCode zero, or else we may trigger an unexpected
                // special key.
                ev.keyDown.keyCode = kbNoKey;
        } else
            ev.keyDown.charScan.charCode = KeyEventW.uChar.AsciiChar;
        ev.keyDown.controlKeyState = KeyEventW.dwControlKeyState;
        // Convert NT style virtual scan codes to PC BIOS codes.
        if (ev.keyDown.controlKeyState & (kbShift | kbAltShift | kbCtrlShift)) {
            uchar index = KeyEventW.wVirtualScanCode;
            if ( (ev.keyDown.controlKeyState & kbCtrlShift) &&
                 (ev.keyDown.controlKeyState & kbAltShift) ) // Ctrl+Alt is AltGr
            {
                // When AltGr+Key does not produce a character, a
                // keyCode with unwanted effects may be read instead.
                if (!ev.keyDown.charScan.charCode)
                    ev.keyDown.keyCode = kbNoKey;
            }
            else if ((ev.keyDown.controlKeyState & kbShift) && THardwareInfo::ShiftCvt[index])
                ev.keyDown.keyCode = THardwareInfo::ShiftCvt[index];
            else if ((ev.keyDown.controlKeyState & kbCtrlShift) && THardwareInfo::CtrlCvt[index])
                ev.keyDown.keyCode = THardwareInfo::CtrlCvt[index];
            else if ((ev.keyDown.controlKeyState & kbAltShift) && THardwareInfo::AltCvt[index])
                ev.keyDown.keyCode = THardwareInfo::AltCvt[index];
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

bool Win32Input::getUnicodeEvent(KEY_EVENT_RECORD KeyEventW, TEvent &ev)
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

bool Win32Input::getMouseEvent(MOUSE_EVENT_RECORD MouseEvent, TEvent &ev)
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

Win32Display::Win32Display(Win32ConsoleStrategy &cnState) :
    cnState(cnState),
    lastAttr('\x00')
{
    BufferedDisplay::init();
}

void Win32Display::setCaretSize(int size)
{
    if (size) {
        cnState.crInfo.bVisible = TRUE;
        cnState.crInfo.dwSize = size;
    } else {
        cnState.crInfo.bVisible = FALSE;
        cnState.crInfo.dwSize = 1;
    }
    SetConsoleCursorInfo(cnHandle(), &cnState.crInfo);
}

int Win32Display::getCaretSize()
{
    return cnState.crInfo.dwSize;
}

bool Win32Display::isCaretVisible()
{
    return cnState.crInfo.bVisible;
}

void Win32Display::clearScreen()
{
    COORD coord = {0, 0};
    int chars = getScreenCols()*getScreenRows();
    DWORD read;
    FillConsoleOutputAttribute(cnHandle(), 0x07, chars, coord, &read);
    FillConsoleOutputCharacter(cnHandle(), ' ', chars, coord, &read);
}

int Win32Display::getScreenRows()
{
    return cnState.sbInfo.dwSize.Y;
}

int Win32Display::getScreenCols()
{
    return cnState.sbInfo.dwSize.X;
}

ushort Win32Display::getScreenMode()
{
    return TDisplay::smCO80;
}

// Fallback display support with rudimentary buffering.

void Win32Display::lowlevelWriteChars(const uchar chars[4], TCellAttribs attr)
{
    if (attr != lastAttr) {
        lowlevelFlush();
        SetConsoleTextAttribute(cnHandle(), (uchar) attr);
        lastAttr = attr;
    }
    uint i = 0;
    while (++i < 4 && chars[i]);
    buf.insert(buf.end(), &chars[0], &chars[i]);
}

void Win32Display::lowlevelMoveCursor(uint x, uint y)
{
    lowlevelFlush();
    SetConsoleCursorPosition(cnHandle(), {(short) x, (short) y});
}

void Win32Display::lowlevelFlush()
{
    WriteConsole(cnHandle(), buf.data(), buf.size(), nullptr, nullptr);
    buf.resize(0);
}
#endif // _WIN32

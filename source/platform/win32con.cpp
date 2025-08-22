#define Uses_TEvent
#define Uses_TKeys
#define Uses_THardwareInfo
#include <tvision/tv.h>
#include <internal/win32con.h>
#include <internal/conctl.h>
#include <internal/winwidth.h>
#include <internal/codepage.h>
#include <internal/termio.h>
#include <internal/utf8.h>
#include <locale.h>

namespace tvision
{

#ifdef _WIN32

#if defined( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

template <class FuncPtr = FARPROC>
static FuncPtr getModuleProc(const char *moduleName, const char *symbolName)
{
    if (HMODULE mod = GetModuleHandleA(moduleName))
        return (FuncPtr) GetProcAddress(mod, symbolName);
    return nullptr;
}

#if defined( __GNUC__ )
#pragma GCC diagnostic pop
#endif

static bool isWine() noexcept
{
    return getModuleProc("NTDLL", "wine_get_version") != nullptr;
}

Win32ConsoleAdapter &Win32ConsoleAdapter::create() noexcept
{
    auto &con = ConsoleCtl::getInstance();
    DWORD startupMode = initInputMode(con);
    bool isLegacyConsole = initOutputMode(con);
    if (isLegacyConsole)
        disableBitmapFont(con);
    UINT cpInput, cpOutput;
    initEncoding(isLegacyConsole, cpInput, cpOutput);
    WinWidth::reset(isLegacyConsole);
    auto &display = *new Win32Display(con, isLegacyConsole);
    auto &input = *new Win32Input(con);
    return *new Win32ConsoleAdapter(con, startupMode, cpInput, cpOutput, display, input);
}

Win32ConsoleAdapter::~Win32ConsoleAdapter()
{
    delete &display;
    delete &input;
    SetConsoleCP(cpInput);
    SetConsoleOutputCP(cpOutput);
    SetConsoleMode(con.in(), startupMode);
}

DWORD Win32ConsoleAdapter::initInputMode(ConsoleCtl &con) noexcept
{
    DWORD consoleMode = 0;
    GetConsoleMode(con.in(), &consoleMode);
    DWORD startupMode = consoleMode;
    consoleMode |= ENABLE_WINDOW_INPUT; // Report changes in buffer size
    consoleMode |= ENABLE_MOUSE_INPUT; // Report mouse events.
    consoleMode &= ~ENABLE_PROCESSED_INPUT; // Report CTRL+C and SHIFT+Arrow events.
    consoleMode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT); // Report Ctrl+S.
    consoleMode |= ENABLE_EXTENDED_FLAGS;   /* Disable the Quick Edit mode, */
    consoleMode &= ~ENABLE_QUICK_EDIT_MODE; /* which inhibits the mouse.    */
    SetConsoleMode(con.in(), consoleMode);
    return startupMode;
}

bool Win32ConsoleAdapter::initOutputMode(ConsoleCtl &con) noexcept
{
    DWORD consoleMode = 0;
    GetConsoleMode(con.out(), &consoleMode);
    consoleMode &= ~ENABLE_WRAP_AT_EOL_OUTPUT; // Avoid scrolling when reaching end of line.
    SetConsoleMode(con.out(), consoleMode);

    bool isLegacyConsole;
    if (isWine())
        // Wine is not exactly a legacy console, but it does not support VT
        // sequences even though it returns no errors when setting the
        // ENABLE_VIRTUAL_TERMINAL_PROCESSING flag.
        // Therefore, we have to handle this case separately.
        isLegacyConsole = true;
    else
    {
        consoleMode |= DISABLE_NEWLINE_AUTO_RETURN; // Do not do CR on LF.
        consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING; // Allow ANSI escape sequences.
        SetConsoleMode(con.out(), consoleMode);
        GetConsoleMode(con.out(), &consoleMode);
        // The legacy console does not support VT sequences.
        isLegacyConsole = (consoleMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) == 0;
    }
    return isLegacyConsole;
}

void Win32ConsoleAdapter::initEncoding(bool isLegacyConsole, UINT &cpInput, UINT &cpOutput) noexcept
{
    cpInput = GetConsoleCP();
    cpOutput = GetConsoleOutputCP();
    // We would like to set all console codepages to UTF-8, but the legacy
    // console with bitmap font is unable to display UTF-8 text properly.
    // However, when using the OEM codepage (which is usually the one supported
    // by the default bitmap font) in the legacy console, unsupported characters
    // may be replaced automatically with a similar one (e.g. '╪' gets replaced
    // with '╬' when the OEM codepage is 850, which doesn't support '╪').
    // If the legacy console is used with a non-bitmap font (such as Lucida
    // Console), then the output codepage does not make a difference.
    SetConsoleCP(CP_UTF8);
    if (isLegacyConsole)
        SetConsoleOutputCP(GetOEMCP());
    else
        SetConsoleOutputCP(CP_UTF8);
    // This only affects the C runtime functions, but it has to be invoked again
    // after calling SetConsoleCP().
    setlocale(LC_ALL, ".utf8");
}

bool Win32ConsoleAdapter::isBitmapFont(UINT fontFamily) noexcept
{
    // https://docs.microsoft.com/en-us/windows/console/console-font-infoex
    // "FontFamily: see the description of the tmPitchAndFamily member
    //  of the TEXTMETRIC structure."
    // https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-textmetricw
    // "A monospace bitmap font has all of these low-order bits clear".
    return (fontFamily & (TMPF_FIXED_PITCH | TMPF_VECTOR | TMPF_TRUETYPE | TMPF_DEVICE)) == 0;
}

void Win32ConsoleAdapter::disableBitmapFont(ConsoleCtl &con) noexcept
// When in the legacy console, try to set a non-bitmap font in order to reduce
// the chances of display issues.
{
    // These functions only exist since Windows Vista, so check whether they
    // are present at runtime.
    static auto pGetCurrentConsoleFontEx =
        getModuleProc<decltype(&GetCurrentConsoleFontEx)>("KERNEL32", "GetCurrentConsoleFontEx");
    static auto pSetCurrentConsoleFontEx =
        getModuleProc<decltype(&SetCurrentConsoleFontEx)>("KERNEL32", "SetCurrentConsoleFontEx");

    if (!pGetCurrentConsoleFontEx || !pSetCurrentConsoleFontEx)
        // The functions are not present, there's nothing we can do.
        return;

    CONSOLE_FONT_INFOEX fontInfo {};
    fontInfo.cbSize = sizeof(fontInfo);
    if ( !pGetCurrentConsoleFontEx(con.out(), FALSE, &fontInfo) ||
         !isBitmapFont(fontInfo.FontFamily) )
        // The bitmap font is already not in use, we don't have to do anything.
        return;

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
        pSetCurrentConsoleFontEx(con.out(), FALSE, &fontInfo);
        pGetCurrentConsoleFontEx(con.out(), FALSE, &fontInfo);
        if (wcscmp(fontInfo.FaceName, name) == 0)
            // We succeeded in changing the font.
            return;
    }
}

bool Win32ConsoleAdapter::isAlive() noexcept
{
    DWORD events = 0;
    return GetNumberOfConsoleInputEvents(con.in(), &events);
}

static bool openClipboard() noexcept
{
    for (int i = 0; i < 5; ++i)
    {
        if (OpenClipboard(nullptr))
            return true;
        Sleep(5);
    }
    return false;
}

bool Win32ConsoleAdapter::setClipboardText(TStringView text) noexcept
{
    bool result = false;
    if (openClipboard())
    {
        HGLOBAL hData = NULL;
        wchar_t *pData;
        int dataLen;
        if ( EmptyClipboard() &&
             !(result = text.empty()) &&
             (dataLen = MultiByteToWideChar(CP_UTF8, 0, text.data(), text.size(), nullptr, 0)) &&
             (hData = GlobalAlloc(GMEM_MOVEABLE, (dataLen + 1)*sizeof(wchar_t))) &&
             (pData = (wchar_t *) GlobalLock(hData))
           )
        {
            MultiByteToWideChar(CP_UTF8, 0, text.data(), text.size(), pData, dataLen);
            pData[dataLen] = L'\0';
            GlobalUnlock(hData);
            result = SetClipboardData(CF_UNICODETEXT, hData);
        }
        CloseClipboard();
        if (hData && !result)
            GlobalFree(hData);
    }
    return result;
}

bool Win32ConsoleAdapter::requestClipboardText(void (&accept)(TStringView)) noexcept
{
    bool result = false;
    if (openClipboard())
    {
        HGLOBAL hData;
        wchar_t *pData;
        if ( (hData = GetClipboardData(CF_UNICODETEXT)) &&
             (result = (pData = (wchar_t *) GlobalLock(hData))) )
        {
            size_t dataLen = wcslen(pData);
            int textLen = WideCharToMultiByte(CP_UTF8, 0, pData, dataLen, nullptr, 0, nullptr, nullptr);
            char *text = new char[textLen];
            WideCharToMultiByte(CP_UTF8, 0, pData, dataLen, text, textLen, nullptr, nullptr);
            GlobalUnlock(hData);
            accept({text, size_t(textLen)});
            delete[] text;
        }
        CloseClipboard();
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////
// Win32Input

Win32Input::Win32Input(ConsoleCtl &aCon) noexcept :
    InputAdapter(aCon.in()),
    con(aCon)
{
}

bool Win32Input::getEvent(TEvent &ev) noexcept
{
    // ReadConsoleInput can sleep the process, so we first check the number
    // of available input events.
    DWORD events;
    while (GetNumberOfConsoleInputEvents(con.in(), &events) && events)
    {
        // getEvent(ir, ev) often returns false due to discarded events. But this
        // function should not return false if there are pending events, as that
        // defeats the event queue in THardwareInfo.
        while (events--)
        {
            INPUT_RECORD ir;
            DWORD ok;
            if (!ReadConsoleInputW(con.in(), &ir, 1, &ok) || !ok)
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
            return getWin32Key(ir.Event.KeyEvent, ev, state);
        break;
    case MOUSE_EVENT:
        getWin32Mouse(ir.Event.MouseEvent, ev, state);
        return true;
    case WINDOW_BUFFER_SIZE_EVENT:
        ev.what = evCommand;
        ev.message.command = cmScreenChanged;
        ev.message.infoPtr = 0;
        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////
// Win32Display

Win32Display::Win32Display(ConsoleCtl &aCon, bool isLegacyConsole) noexcept :
    con(aCon)
{
    if (!isLegacyConsole)
        ansiScreenWriter = new AnsiScreenWriter(con, TermCap::getDisplayCapabilities(con, *this));
}

Win32Display::~Win32Display()
{
    delete ansiScreenWriter;
}

TPoint Win32Display::reloadScreenInfo() noexcept
{
    TPoint lastSize = size;
    size = con.getSize();

    if (lastSize != size)
    {
        CONSOLE_SCREEN_BUFFER_INFO sbInfo {};
        GetConsoleScreenBufferInfo(con.out(), &sbInfo);
        // Set the cursor temporarily to (0, 0) to prevent the console from
        // crashing due to https://github.com/microsoft/terminal/issues/7511.
        auto curPos = sbInfo.dwCursorPosition;
        SetConsoleCursorPosition(con.out(), {0, 0});
        // Make sure the buffer size matches the viewport size so that the
        // scrollbars are not shown.
        // This must be done only when the viewport size has changed. Otherwise,
        // we may keep triggering WINDOW_BUFFER_SIZE_EVENT events all the time,
        // at least on Wine.
        SetConsoleScreenBufferSize(con.out(), {(short) size.x, (short) size.y});
        // Restore the cursor position (it does not matter if it is out of bounds).
        SetConsoleCursorPosition(con.out(), curPos);
    }

    CONSOLE_FONT_INFO fontInfo;
    if ( GetCurrentConsoleFont(con.out(), FALSE, &fontInfo)
         && memcmp(&fontInfo, &lastFontInfo, sizeof(fontInfo)) != 0 )
    {
        // Character width depends on the font and the font size being used.
        WinWidth::reset(ansiScreenWriter == nullptr);
        lastFontInfo = fontInfo;
    }

    if (ansiScreenWriter)
        ansiScreenWriter->reset();
    else
    {
        caretPos = {-1, -1};
        lastAttr = '\x00';
    }

    return size;
}

int Win32Display::getColorCount() noexcept
{
    // https://devblogs.microsoft.com/commandline/24-bit-color-in-the-windows-console/
    // Assume high color support when not in the legacy console.
    if (ansiScreenWriter)
        return 256*256*256;
    return 16;
}

TPoint Win32Display::getFontSize() noexcept
{
    return con.getFontSize();
}

void Win32Display::setCaretSize(int size) noexcept
{
    CONSOLE_CURSOR_INFO crInfo;
    if (size) {
        crInfo.bVisible = TRUE;
        crInfo.dwSize = size;
    } else {
        crInfo.bVisible = FALSE;
        crInfo.dwSize = 1;
    }
    SetConsoleCursorInfo(con.out(), &crInfo);
}

void Win32Display::clearScreen() noexcept
{
    if (ansiScreenWriter)
        ansiScreenWriter->clearScreen();
    else
    {
        COORD coord = {0, 0};
        DWORD length = size.x * size.y;
        BYTE attr = 0x07;
        DWORD read;
        FillConsoleOutputAttribute(con.out(), attr, length, coord, &read);
        FillConsoleOutputCharacterA(con.out(), ' ', length, coord, &read);
        lastAttr = attr;
    }
}

void Win32Display::writeCell( TPoint pos, TStringView text, TColorAttr attr,
                              bool doubleWidth ) noexcept
{
    if (ansiScreenWriter)
        ansiScreenWriter->writeCell(pos, text, attr, doubleWidth);
    else
    {
        if (pos != caretPos)
        {
            flush();
            SetConsoleCursorPosition(con.out(), {(short) pos.x, (short) pos.y});
        }

        uchar biosAttr = attr.toBIOS();
        if (biosAttr != lastAttr)
        {
            flush();
            SetConsoleTextAttribute(con.out(), biosAttr);
        }

        buf.insert(buf.end(), text.begin(), text.end());

        caretPos = {pos.x + 1 + doubleWidth, pos.y};
        lastAttr = biosAttr;
    }
}

void Win32Display::setCaretPosition(TPoint pos) noexcept
{
    if (ansiScreenWriter)
        ansiScreenWriter->setCaretPosition(pos);
    else
    {
        flush();
        SetConsoleCursorPosition(con.out(), {(short) pos.x, (short) pos.y});
        caretPos = pos;
    }
}

void Win32Display::flush() noexcept
{
    if (ansiScreenWriter)
        ansiScreenWriter->flush();
    else if (!buf.empty())
    {
        // When in the legacy console:
        // * If using a bitmap font, the OEM codepage must be used (see
        //   'initEncoding()'). WriteConsoleW does not work properly with
        //   UTF-16 characters which map to control characters in the OEM
        //   codepage (e.g. '○' translates into 0x09, so it's a tabulator),
        //   so we use WriteConsoleOutputW instead.
        // * If not using a bitmap font, there is no need to use the OEM codepage
        //   and both WriteConsoleW and WriteConsoleOutputW work the same, but
        //   we don't use a specific logic for this case since it's not necessary.
        // * Every wchar_t (UTF-16 code unit) takes one screen cell.
        int wCharCount = MultiByteToWideChar(CP_UTF8, 0, &buf[0], buf.size(), nullptr, 0);
        std::vector<wchar_t> wChars(wCharCount);
        MultiByteToWideChar(CP_UTF8, 0, &buf[0], buf.size(), &wChars[0], wCharCount);

        std::vector<CHAR_INFO> cells(wCharCount);
        for (int i = 0; i < wCharCount; ++i)
        {
            cells[i].Char.UnicodeChar = wChars[i];
            cells[i].Attributes = lastAttr;
        }

        SMALL_RECT to = {
            short(caretPos.x - wCharCount),
            short(caretPos.y),
            short(caretPos.x - 1),
            short(caretPos.y),
        };
        WriteConsoleOutputW(con.out(), &cells[0], {(short) wCharCount, 1}, {0, 0}, &to);

        buf.resize(0);
    }
}

#endif // _WIN32

/////////////////////////////////////////////////////////////////////////
// Global functions

static bool getWin32KeyText(const KEY_EVENT_RECORD &KeyEvent, TEvent &ev, InputState &state) noexcept
// Returns true unless the event contains a UTF-16 surrogate (Windows only),
// in which case we need the next event.
{
    uint32_t ch = KeyEvent.uChar.UnicodeChar;
    ev.keyDown.textLength = 0;

    // Do not treat non-printable characters as text.
    if (' ' <= ch && ch != 0x7F)
    {
#ifdef _WIN32
        if (0xD800 <= ch && ch <= 0xDBFF)
        {
            state.surrogate = ch;
            return false;
        }

        wchar_t utf16[2] = {(wchar_t) ch, 0};
        if (state.surrogate)
        {
            if (0xDC00 <= ch && ch <= 0xDFFF)
            {
                utf16[1] = (wchar_t) ch;
                utf16[0] = state.surrogate;
            }
            state.surrogate = 0;
        }

        ev.keyDown.textLength = WideCharToMultiByte(
            CP_UTF8, 0,
            utf16, utf16[1] ? 2 : 1,
            ev.keyDown.text, sizeof(ev.keyDown.text),
            nullptr, nullptr
        );
#else
        (void) state;

        if (ch < 0xD800 || (0xDFFF < ch && ch < 0x10FFFF))
            ev.keyDown.textLength = (uchar) utf32To8(ch, ev.keyDown.text);
#endif // _WIN32
    }
    return true;
}

bool getWin32Key(const KEY_EVENT_RECORD &KeyEvent, TEvent &ev, InputState &state) noexcept
{
    if (!getWin32KeyText(KeyEvent, ev, state))
        return false;

    ev.what = evKeyDown;
    ev.keyDown.charScan.scanCode = KeyEvent.wVirtualScanCode;
    ev.keyDown.charScan.charCode = KeyEvent.uChar.AsciiChar;
    ev.keyDown.controlKeyState = KeyEvent.dwControlKeyState & (
        kbShift | kbCtrlShift | kbAltShift |
        kbScrollState | kbNumState | kbCapsState | kbEnhanced
    );

    if (ev.keyDown.textLength != 0)
    {
        ev.keyDown.charScan.charCode = CpTranslator::fromUtf8(ev.keyDown.getText());
        if (KeyEvent.wVirtualKeyCode == VK_MENU)
            // This is enabled when pasting certain characters, and it confuses
            // applications. Clear it.
            ev.keyDown.charScan.scanCode = 0;
        if (ev.keyDown.charScan.charCode == '\0' || ev.keyDown.keyCode <= kbCtrlZ)
            // If the character cannot be represented in the current codepage,
            // or if it would accidentally trigger a Ctrl+Key combination,
            // make the whole keyCode zero to avoid side effects.
            ev.keyDown.keyCode = kbNoKey;
    }

    if ( ev.keyDown.keyCode == 0x2A00 || ev.keyDown.keyCode == 0x1D00 ||
         ev.keyDown.keyCode == 0x3600 || ev.keyDown.keyCode == 0x3800 ||
         ev.keyDown.keyCode == 0x3A00 || ev.keyDown.keyCode == 0x5B00 ||
         ev.keyDown.keyCode == 0x5C00 )
        // Discard standalone Shift, Ctrl, Alt, Caps Lock, Windows keys.
        ev.keyDown.keyCode = kbNoKey;
    else if ( (ev.keyDown.controlKeyState & kbLeftCtrl) &&
              (ev.keyDown.controlKeyState & kbRightAlt) &&
              ev.keyDown.textLength == 0 )
        // We cannot tell for sure if the right Alt key is AltGr, since
        // that depends on the keyboard layout, but it is certain that
        // AltGr automatically adds the left Ctrl flag.
        // If both of these are set but no text is produced, discard the
        // whole event since we don't want AltGr to be handled as Ctrl+Alt.
        ev.keyDown.keyCode = kbNoKey;
    else if ( (ev.keyDown.controlKeyState & kbCtrlShift) &&
              (ev.keyDown.controlKeyState & kbAltShift) &&
              ev.keyDown.textLength != 0 )
        // If Ctrl+Alt produces text, we are dealing with AltGr. In this case,
        // discard the Ctrl and Alt modifiers.
        ev.keyDown.controlKeyState &= ~(kbCtrlShift | kbAltShift);
    else if (KeyEvent.wVirtualScanCode < 89)
    {
        // Convert NT style virtual scan codes to PC BIOS codes.
        uchar index = KeyEvent.wVirtualScanCode;
        ushort keyCode = 0;
        if ((ev.keyDown.controlKeyState & kbAltShift) && THardwareInfo::AltCvt[index])
            keyCode = THardwareInfo::AltCvt[index];
        else if ((ev.keyDown.controlKeyState & kbCtrlShift) && THardwareInfo::CtrlCvt[index])
            keyCode = THardwareInfo::CtrlCvt[index];
        else if ((ev.keyDown.controlKeyState & kbShift) && THardwareInfo::ShiftCvt[index])
            keyCode = THardwareInfo::ShiftCvt[index];
        else if ( !(ev.keyDown.controlKeyState & (kbShift | kbCtrlShift | kbAltShift)) &&
                  THardwareInfo::NormalCvt[index] )
            keyCode = THardwareInfo::NormalCvt[index];

        if (keyCode != 0)
        {
            ev.keyDown.keyCode = keyCode;
            if (ev.keyDown.charScan.charCode < ' ')
                ev.keyDown.textLength = 0;
            else if (ev.keyDown.charScan.charCode < 0x7F && !ev.keyDown.textLength)
            {
                ev.keyDown.text[0] = ev.keyDown.charScan.charCode;
                ev.keyDown.textLength = 1;
            }
        }
    }

    return ev.keyDown.keyCode != kbNoKey || ev.keyDown.textLength;
}

void getWin32Mouse(const MOUSE_EVENT_RECORD &MouseEvent, TEvent &ev, InputState &state) noexcept
{
    ev.what = evMouse;
    ev.mouse.where.x = MouseEvent.dwMousePosition.X;
    ev.mouse.where.y = MouseEvent.dwMousePosition.Y;
    ev.mouse.buttons = state.buttons = MouseEvent.dwButtonState;
    ev.mouse.eventFlags = MouseEvent.dwEventFlags;
    ev.mouse.controlKeyState = MouseEvent.dwControlKeyState & (
        kbShift | kbCtrlShift | kbAltShift |
        kbScrollState | kbNumState | kbCapsState | kbEnhanced
    );

    // Rotation sense is represented by the sign of dwButtonState's high word
    Boolean positive = !(MouseEvent.dwButtonState & 0x80000000);
    if( MouseEvent.dwEventFlags & MOUSE_WHEELED )
        ev.mouse.wheel = positive ? mwUp : mwDown;
    else if( MouseEvent.dwEventFlags & MOUSE_HWHEELED )
        ev.mouse.wheel = positive ? mwRight : mwLeft;
    else
        ev.mouse.wheel = 0;
}

} // namespace tvision


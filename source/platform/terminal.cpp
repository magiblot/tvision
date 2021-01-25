#define Uses_TKeys
#include <tvision/tv.h>

#include <internal/terminal.h>
#include <internal/stdioctl.h>
#include <internal/constmap.h>
#include <cctype>

static const const_unordered_map<ushort, KeyDownEvent> AltKeyCode = {
    { ' ', {{kbAltSpace}} },
    { 'Q', {{kbAltQ}} }, { 'W', {{kbAltW}} }, { 'E', {{kbAltE}} }, { 'R', {{kbAltR}} },
    { 'T', {{kbAltT}} }, { 'Y', {{kbAltY}} }, { 'U', {{kbAltU}} }, { 'I', {{kbAltI}} },
    { 'O', {{kbAltO}} }, { 'P', {{kbAltP}} }, { 'A', {{kbAltA}} }, { 'S', {{kbAltS}} },
    { 'D', {{kbAltD}} }, { 'F', {{kbAltF}} }, { 'G', {{kbAltG}} }, { 'H', {{kbAltH}} },
    { 'J', {{kbAltJ}} }, { 'K', {{kbAltK}} }, { 'L', {{kbAltL}} }, { 'Z', {{kbAltZ}} },
    { 'X', {{kbAltX}} }, { 'C', {{kbAltC}} }, { 'V', {{kbAltV}} }, { 'B', {{kbAltB}} },
    { 'N', {{kbAltN}} }, { 'M', {{kbAltM}} }, { '1', {{kbAlt1}} }, { '2', {{kbAlt2}} },
    { '3', {{kbAlt3}} }, { '4', {{kbAlt4}} }, { '5', {{kbAlt5}} }, { '6', {{kbAlt6}} },
    { '7', {{kbAlt7}} }, { '8', {{kbAlt8}} }, { '9', {{kbAlt9}} }, { '0', {{kbAlt0}} },
    { '-', {{kbAltMinus}} }, { '=', {{kbAltEqual}} }, { '\x08', {{kbAltBack}} },
    { kbF1,             {{kbAltF1},     kbAltShift}                 },
    { kbF2,             {{kbAltF2},     kbAltShift}                 },
    { kbF3,             {{kbAltF3},     kbAltShift}                 },
    { kbF4,             {{kbAltF4},     kbAltShift}                 },
    { kbF5,             {{kbAltF5},     kbAltShift}                 },
    { kbF6,             {{kbAltF6},     kbAltShift}                 },
    { kbF7,             {{kbAltF7},     kbAltShift}                 },
    { kbF8,             {{kbAltF8},     kbAltShift}                 },
    { kbF9,             {{kbAltF9},     kbAltShift}                 },
    { kbF10,            {{kbAltF10},    kbAltShift}                 },
    { kbF11,            {{kbAltF11},    kbAltShift}                 },
    { kbF12,            {{kbAltF11},    kbAltShift}                 },
    { kbDown,           {{kbAltDown},   kbAltShift}                 },
    { kbUp,             {{kbAltUp},     kbAltShift}                 },
    { kbLeft,           {{kbAltLeft},   kbAltShift}                 },
    { kbRight,          {{kbAltRight},  kbAltShift}                 },
    { kbIns,            {{kbAltIns},    kbAltShift}                 },
    { kbDel,            {{kbAltDel},    kbAltShift}                 },
    { kbHome,           {{kbAltHome},   kbAltShift}                 },
    { kbEnd,            {{kbAltEnd},    kbAltShift}                 },
    { kbPgUp,           {{kbAltPgUp},   kbAltShift}                 },
    { kbPgDn,           {{kbAltPgDn},   kbAltShift}                 },
    { kbCtrlDown,       {{kbDown},      kbCtrlShift | kbAltShift}   },
    { kbCtrlUp,         {{kbUp},        kbCtrlShift | kbAltShift}   },
    { kbCtrlLeft,       {{kbLeft},      kbCtrlShift | kbAltShift}   },
    { kbCtrlRight,      {{kbRight},     kbCtrlShift | kbAltShift}   },
};

// The default mouse experience with Ncurses is not always good. To work around
// some issues, we request and parse mouse events manually.

void TermIO::mouseOn()
{
    TStringView seq = "\x1B[?1001s" // Save old highlight mouse reporting.
                      "\x1B[?1000h" // Enable mouse reporting.
                      "\x1B[?1002h" // Enable mouse drag reporting.
                      "\x1B[?1006h" // Enable SGR extended mouse reporting.
                    ;
    consoleWrite(seq.data(), seq.size());
}

void TermIO::mouseOff()
{
    TStringView seq = "\x1B[?1006l" // Disable SGR extended mouse reporting.
                      "\x1B[?1002l" // Disable mouse drag reporting.
                      "\x1B[?1000l" // Disable mouse reporting.
                      "\x1B[?1001r" // Restore old highlight mouse reporting.
                    ;
    consoleWrite(seq.data(), seq.size());
}

bool TermIO::acceptMouseEvent(TEvent &ev, MouseState &oldm, const MouseState &newm)
{
    // Some terminal emulators send a mouse event every pixel the graphical
    // mouse cursor moves over the window. Filter out those unnecessary
    // events.
    if (newm.buttons != oldm.buttons || newm.wheel || newm.where != oldm.where)
    {
        ev.what = evMouse;
        ev.mouse = {};
        ev.mouse.buttons = newm.buttons;
        ev.mouse.where = newm.where;
        ev.mouse.wheel = newm.wheel;
        oldm = newm;
        return true;
    }
    return false;
}

void TermIO::setAltModifier(TEvent &ev)
{
    ev.keyDown.controlKeyState |= kbAltShift;
    // Set the proper key code if Turbo Vision supports the combination.
    if (!ev.keyDown.charScan.scanCode && ev.keyDown.charScan.charCode <= '\x7F')
        ev.keyDown.charScan.charCode = toupper(ev.keyDown.charScan.charCode);
    KeyDownEvent keyDown = AltKeyCode[ev.keyDown.keyCode];
    if (keyDown.keyCode)
        ev.keyDown = keyDown;
}

ParseResult TermIO::parseEscapeSeq(GetChBuf &buf, TEvent &ev, MouseState &oldm)
// Pre: "\x1B" has just been read.
{
    ParseResult res = Rejected;
    switch (buf.get())
    {
        case '[':
            switch (buf.get())
            {
                // Note: mouse events are usually detected in 'NcursesInput::parseCursesMouse'.
                case 'M':
                    return parseX10Mouse(buf, ev, oldm) == Accepted ? Accepted : Ignored;
                case '<':
                    return parseSGRMouse(buf, ev, oldm) == Accepted ? Accepted : Ignored;
                case '1':
                case '2':
                case '3':
                case '4':
                    if (buf.get() == '~')
                        res = parseHomeEndA(buf, ev);
                    else
                    {
                        buf.unget();
                        res = parseFKeyA(buf, ev);
                    }
                    break;
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                    res = parseArrowKeyA(buf, ev); break;
            }
            break;
        case 'O':
            res = parseFKeyB(buf, ev); break;
        case '\x1B':
            res = parseEscapeSeq(buf, ev, oldm);
            if (res == Accepted)
                setAltModifier(ev);
            break;
    }
    if (res == Rejected)
        buf.reject();
    return res;
}

ParseResult TermIO::parseX10Mouse(GetChBuf &buf, TEvent &ev, MouseState &oldm)
// Pre: "\x1B[M" has just been read.
// The complete sequence looks like "\x1B[Mabc", where:
// * 'a' is the button number plus 32.
// * 'b' is the column number (one-based) plus 32.
// * 'c' is the row number (one-based) plus 32.
{
    int but, col, row;
    but = buf.get();
    if (but < 32 || 255 < but) return Rejected;
    but -= 32;
    for (int *i : {&col, &row})
    {
        *i = buf.get();
        if (*i < 0 || 255 < *i)
            return Rejected;
        // In theory, this encoding only supports coordinates in the range [0, 222].
        // However, some terminal emulators (e.g. urxvt) keep increasing the
        // counters, causing an overflow. We can take advantage of this to support
        // more coordinates, but we definitely don't want to reject the sequence,
        // as that will cause Ctrl+key events to be generated.
        if (*i > 32)
            *i -= 32;
        else
            *i += (256 - 32);
        // Make it zero-based.
        --*i;
    }

    MouseState newm = {};
    newm.where = {col, row};
    newm.buttons = oldm.buttons;
    switch (but)
    {
        case 0: // Press.
        case 32: // Drag.
            newm.buttons |= mbLeftButton; break;
        case 1:
        case 33:
            newm.buttons |= mbMiddleButton; break;
        case 2:
        case 34:
            newm.buttons |= mbRightButton; break;
        case 3: newm.buttons = 0; break; // Release.
        case 64: newm.wheel = mwUp; break;
        case 65: newm.wheel = mwDown; break;
    }
    return acceptMouseEvent(ev, oldm, newm) ? Accepted : Ignored;
}

ParseResult TermIO::parseSGRMouse(GetChBuf &buf, TEvent &ev, MouseState &oldm)
// https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Extended-coordinates
// Pre: "\x1B[<" has just been read.
// The complete sequence looks like "\x1B[<a;b;cM" or "\x1B[<a;b;cm", where:
// * 'a' is a sequence of digits representing the button number in decimal.
// * 'b' is a sequence of digits representing the column number (one-based) in decimal.
// * 'c' is a sequence of digits representing the row number (one-based) in decimal.
// The sequence ends with 'M' on button press and on 'm' on button release.
{
    int but, col, row, state;
    for (int *i : {&but, &col, &row})
        if ((*i = buf.getNum()) == -1)
            return Rejected;
    // Make the coordinates zero-based.
    --row, --col;
    // Finally, the press/release state.
    state = buf.last();
    if (!(state == 'M' || state == 'm')) return Rejected;

    MouseState newm = {};
    newm.where = {col, row};
    newm.buttons = oldm.buttons;
    if (state == 'M') // Press, wheel or drag.
    {
        switch (but)
        {
            case 0:
            case 32:
                newm.buttons |= mbLeftButton; break;
            case 1:
            case 33:
                newm.buttons |= mbMiddleButton; break;
            case 2:
            case 34:
                newm.buttons |= mbRightButton; break;
            case 64: newm.wheel = mwUp; break;
            case 65: newm.wheel = mwDown; break;
        }
    }
    else // Release.
    {
        switch (but)
        {
            case 0: newm.buttons &= ~mbLeftButton; break;
            case 1: newm.buttons &= ~mbMiddleButton; break;
            case 2: newm.buttons &= ~mbRightButton; break;
        }
    }
    return acceptMouseEvent(ev, oldm, newm) ? Accepted : Ignored;
}

// The functions below are meant to parse a few sequences emitted
// by terminals that do not match their terminfo / termcap entries, e.g.
// Shift F1-4 on Konsole and F1-4 on Putty. It's easier than fixing the
// application or updating the terminal database.

ParseResult TermIO::parseFKeyA(GetChBuf &buf, TEvent &ev)
// https://invisible-island.net/xterm/xterm-function-keys.html
// Pre: "\x1B[n" has just been read, where 'n' is '1', '2' or '3'.
{
    KeyDownEvent keyDown = {};
    switch (buf.last())
    {
        case '1':
            switch (buf.get())
            {
                case '1': keyDown.keyCode = kbF1; break;
                case '2': keyDown.keyCode = kbF2; break;
                case '3': keyDown.keyCode = kbF3; break;
                case '4': keyDown.keyCode = kbF4; break;
                default: return Rejected;
            }
            break;
        case '2':
            switch (buf.get())
            {
                case '5': keyDown = {{kbShiftF1}, kbShift}; break;
                case '6': keyDown = {{kbShiftF2}, kbShift}; break;
                case '8': keyDown = {{kbShiftF3}, kbShift}; break;
                case '9': keyDown = {{kbShiftF4}, kbShift}; break;
                default: return Rejected;
            }
            break;
        case '3':
            switch (buf.get())
            {
                case '1': keyDown = {{kbShiftF5}, kbShift}; break;
                case '2': keyDown = {{kbShiftF6}, kbShift}; break;
                case '3': keyDown = {{kbShiftF7}, kbShift}; break;
                case '4': keyDown = {{kbShiftF8}, kbShift}; break;
                default: return Rejected;
            }
            break;
        default: return Rejected;
    }
    if (buf.get() != '~')
        return Rejected;
    ev.what = evKeyDown;
    ev.keyDown = keyDown;
    return Accepted;
}

ParseResult TermIO::parseFKeyB(GetChBuf &buf, TEvent &ev)
// https://invisible-island.net/xterm/xterm-function-keys.html
// Pre: "\x1BO" has just been read.
{
    KeyDownEvent keyDown = {};
    switch (buf.get())
    {
        case '2':
            switch (buf.get())
            {
                case 'P': keyDown = {{kbShiftF1}, kbShift}; break;
                case 'Q': keyDown = {{kbShiftF2}, kbShift}; break;
                case 'R': keyDown = {{kbShiftF3}, kbShift}; break;
                case 'S': keyDown = {{kbShiftF4}, kbShift}; break;
                default: return Rejected;
            }
            break;
        case '3':
            switch (buf.get())
            {
                case 'P': keyDown = {{kbAltF1}, kbAltShift}; break;
                case 'Q': keyDown = {{kbAltF2}, kbAltShift}; break;
                case 'R': keyDown = {{kbAltF3}, kbAltShift}; break;
                case 'S': keyDown = {{kbAltF4}, kbAltShift}; break;
                default: return Rejected;
            }
            break;
        case '5':
            switch (buf.get())
            {
                case 'P': keyDown = {{kbCtrlF1}, kbCtrlShift}; break;
                case 'Q': keyDown = {{kbCtrlF2}, kbCtrlShift}; break;
                case 'R': keyDown = {{kbCtrlF3}, kbCtrlShift}; break;
                case 'S': keyDown = {{kbCtrlF4}, kbCtrlShift}; break;
                default: return Rejected;
            }
            break;
        case '4':
        case '6':
        case '7':
        case '8':
            switch (buf.get())
            {
                // Multiple modifiers not supported by Turbo Vision.
                case 'P': return Ignored;
                case 'Q': return Ignored;
                case 'R': return Ignored;
                case 'S': return Ignored;
                default: return Rejected;
            }
            break;
        default: return Rejected;
    }
    ev.what = evKeyDown;
    ev.keyDown = keyDown;
    return Accepted;
}

ParseResult TermIO::parseArrowKeyA(GetChBuf &buf, TEvent &ev)
// Putty.
{
    KeyDownEvent keyDown = {};
    switch (buf.last())
    {
        case 'A': keyDown = {{kbCtrlUp}, kbCtrlShift}; break;
        case 'B': keyDown = {{kbCtrlDown}, kbCtrlShift}; break;
        case 'C': keyDown = {{kbCtrlRight}, kbCtrlShift}; break;
        case 'D': keyDown = {{kbCtrlLeft}, kbCtrlShift}; break;
        default: return Rejected;
    }
    ev.what = evKeyDown;
    ev.keyDown = keyDown;
    return Accepted;
}

ParseResult TermIO::parseHomeEndA(GetChBuf &buf, TEvent &ev)
// Putty.
{
    if (buf.last() != '~')
        return Rejected;
    KeyDownEvent keyDown = {};
    switch (buf.last(1))
    {
        case '1': keyDown = {{kbHome}}; break;
        case '4': keyDown = {{kbEnd}}; break;
        default: return Rejected;
    }
    ev.what = evKeyDown;
    ev.keyDown = keyDown;
    return Accepted;
}

#ifdef _TV_UNIX
#include <unistd.h>
#include <sys/ioctl.h>

TPoint TermIO::Unix::getSize()
{
    struct winsize w;
    if ( ioctl(StdioCtl::in(), TIOCGWINSZ, &w) != -1 ||
         ioctl(StdioCtl::out(), TIOCGWINSZ, &w) != -1 )
    {
        return {w.ws_col, w.ws_row};
    }
    return {0, 0};
}

void TermIO::Unix::consoleWrite(const void *data, size_t bytes)
{
    fflush(StdioCtl::fout());
    int rr = ::write(StdioCtl::out(), data, bytes);
    (void) rr;
}

#elif defined(_WIN32)

void TermIO::Win32::consoleWrite(const void *data, size_t bytes)
{
    WriteConsole(StdioCtl::out(), data, bytes, nullptr, nullptr);
}

#endif // _TV_UNIX

bool TermIO::isLinuxConsole()
{
#ifdef __linux__
    /* This is the same function used to get the Shift/Ctrl/Alt modifiers
     * on the console. It fails if stdin is not a console file descriptor. */
    for (int fd : {StdioCtl::in(), StdioCtl::out()})
    {
        char subcode[] = {6, 0}; // Null-terminate so that valgrind doesn't complain.
        if (ioctl(fd, TIOCLINUX, subcode) != -1)
            return true;
    }
#endif
    return false;
}

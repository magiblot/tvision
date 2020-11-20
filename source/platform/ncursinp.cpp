#ifdef HAVE_NCURSES

#define Uses_THardwareInfo
#define Uses_TKeys
#define Uses_TEvent
#include <tvision/tv.h>

#include <ncurses.h>
#include <internal/ncursinp.h>
#include <internal/getenv.h>
#include <internal/utf8.h>
#include <internal/codepage.h>
#include <internal/constmap.h>
#include <internal/stdioctl.h>
#include <string>

/* Turbo Vision is designed to work with BIOS key codes. Mnemonics for some
 * key codes are defined in tkeys.h. Unless those are changed, it is
 * necessary to translate ncurses keys to key codes. */

/* Turbo Vision stores key events in a KeyDownEvent struct, defined in
 * system.h. Its first field is a key code (16 bit), which can be decomposed
 * into the ASCII equivalent (lower byte) and a scan code (higher byte).
 * It has a second field with the state of the modifier keys, which can be
 * retrieved by performing a bit-wise AND with the kbShift, kbCtrlShift and
 * kbAltShift bitmasks. Turbo Vision expects this field to be filled even
 * if the key code is already named Shift/Ctrl/Alt+something. */

/* The support for key combinations is the following:
   - PrintScreen, Break are not likely to be captured by the terminal, but
     Ctrl+C could be used as a replacement of the Ctrl+Break interrupt.
   - Ctrl/Alt+F(n) don't work on the linux console and I strongly advice against
     using them.
   - Ctrl+Letter works, except for ^H, ^I, ^J and ^M, which have a special
     meaning.
   - Alt+Letter/Number seem to work quite well.
   - Ctrl+Backspace/Enter can't be recognized on terminal emulators.
   - Shift/Ctrl+Ins/Del/Home/End/PgDn/PgUp seem to work, too.
   - Arrow keys work, as well as combined with Shift, but Turbo Vision doesn't
     support Ctrl+Up/Down (EDIT: it now does).
   - Tab and Backtab are supported too, although the linux console confuses the
     latter with Alt+Tab.
   - Some other key combinations are supported on terminal but not in Turbo Vision.
 * Still, it's up to your luck that ncurses manages to grab any of these
 * combinations from your terminal application. */

static constexpr KeyDownEvent fromNonPrintableAscii[32] = {
    {{kbNoKey},     0}          , // ^@, Null
    {{kbCtrlA},     kbCtrlShift},
    {{kbCtrlB},     kbCtrlShift},
    {{kbCtrlC},     kbCtrlShift},
    {{kbCtrlD},     kbCtrlShift},
    {{kbCtrlE},     kbCtrlShift},
    {{kbCtrlF},     kbCtrlShift},
    {{kbCtrlG},     kbCtrlShift},
    {{kbBack},      0}          , // ^H, Backspace
    {{kbTab},       0}          , // ^I, Tab
    {{kbEnter},     0}          , // ^J, Line Feed
    {{kbCtrlK},     kbCtrlShift},
    {{kbCtrlL},     kbCtrlShift},
    {{kbEnter},     0}          , // ^M, Carriage Return
    {{kbCtrlN},     kbCtrlShift},
    {{kbCtrlO},     kbCtrlShift},
    {{kbCtrlP},     kbCtrlShift},
    {{kbCtrlQ},     kbCtrlShift},
    {{kbCtrlR},     kbCtrlShift},
    {{kbCtrlS},     kbCtrlShift},
    {{kbCtrlT},     kbCtrlShift},
    {{kbCtrlU},     kbCtrlShift},
    {{kbCtrlV},     kbCtrlShift},
    {{kbCtrlW},     kbCtrlShift},
    {{kbCtrlX},     kbCtrlShift},
    {{kbCtrlY},     kbCtrlShift},
    {{kbCtrlZ},     kbCtrlShift},
    {{kbEsc},       0}          , // ^[, Escape
    {{0x1C},        0}          , // ^\, ?
    {{0x1D},        0}          , // ^], ?
    {{0x1E},        0}          , // ^^, ?
    {{0x1F},        0}          , // ^_, ?
};

static const const_unordered_map<char, ushort> AltKeyCode = {
    { ' ', kbAltSpace },
    { 'Q', kbAltQ }, { 'W', kbAltW }, { 'E', kbAltE }, { 'R', kbAltR },
    { 'T', kbAltT }, { 'Y', kbAltY }, { 'U', kbAltU }, { 'I', kbAltI },
    { 'O', kbAltO }, { 'P', kbAltP }, { 'A', kbAltA }, { 'S', kbAltS },
    { 'D', kbAltD }, { 'F', kbAltF }, { 'G', kbAltG }, { 'H', kbAltH },
    { 'J', kbAltJ }, { 'K', kbAltK }, { 'L', kbAltL }, { 'Z', kbAltZ },
    { 'X', kbAltX }, { 'C', kbAltC }, { 'V', kbAltV }, { 'B', kbAltB },
    { 'N', kbAltN }, { 'M', kbAltM }, { '1', kbAlt1 }, { '2', kbAlt2 },
    { '3', kbAlt3 }, { '4', kbAlt4 }, { '5', kbAlt5 }, { '6', kbAlt6 },
    { '7', kbAlt7 }, { '8', kbAlt8 }, { '9', kbAlt9 }, { '0', kbAlt0 },
    { '-', kbAltMinus }, { '=', kbAltEqual }, { '\x08', kbAltBack }
};

static const const_unordered_map<int, KeyDownEvent> fromCursesKeyCode = {
    { KEY_DOWN,         {{kbDown},      0}          },
    { KEY_UP,           {{kbUp},        0}          },
    { KEY_LEFT,         {{kbLeft},      0}          },
    { KEY_RIGHT,        {{kbRight},     0}          },
    { KEY_HOME,         {{kbHome},      0}          },
    { KEY_BACKSPACE,    {{kbBack},      0}          },
    { KEY_DC,           {{kbDel},       0}          },
    { KEY_IC,           {{kbIns},       0}          },
    { KEY_SF,           {{kbDown},      kbShift}    },
    { KEY_SR,           {{kbUp},        kbShift}    },
    { KEY_NPAGE,        {{kbPgDn},      0}          },
    { KEY_PPAGE,        {{kbPgUp},      0}          },
    { KEY_ENTER,        {{kbEnter},     0}          },
    { KEY_BTAB,         {{kbShiftTab},  kbShift}    },
    { KEY_END,          {{kbEnd},       0}          },
    { KEY_SDC,          {{kbShiftDel},  kbShift}    },
    { KEY_SEND,         {{kbEnd},       kbShift}    },
    { KEY_SHOME,        {{kbHome},      kbShift}    },
    { KEY_SIC,          {{kbShiftIns},  kbShift}    },
    { KEY_SLEFT,        {{kbLeft},      kbShift}    },
    { KEY_SRIGHT,       {{kbRight},     kbShift}    },
    { KEY_SUSPEND,      {{kbCtrlZ},     kbCtrlShift}},
    // Avoid using these, as they are reserved by the Linux console.
    { KEY_SPREVIOUS,    {{kbPgUp},      kbShift}    },
    { KEY_SNEXT,        {{kbPgDn},      kbShift}    },
    // Function keys F1-F12
    { KEY_F0 + 1,       {{kbF1},        0}          },
    { KEY_F0 + 2,       {{kbF2},        0}          },
    { KEY_F0 + 3,       {{kbF3},        0}          },
    { KEY_F0 + 4,       {{kbF4},        0}          },
    { KEY_F0 + 5,       {{kbF5},        0}          },
    { KEY_F0 + 6,       {{kbF6},        0}          },
    { KEY_F0 + 7,       {{kbF7},        0}          },
    { KEY_F0 + 8,       {{kbF8},        0}          },
    { KEY_F0 + 9,       {{kbF9},        0}          },
    { KEY_F0 + 10,      {{kbF10},       0}          },
    { KEY_F0 + 11,      {{kbF11},       0}          },
    { KEY_F0 + 12,      {{kbF12},       0}          },
    // Shift+F1-F12
    { KEY_F0 + 13,      {{kbShiftF1},   kbShift}    },
    { KEY_F0 + 14,      {{kbShiftF2},   kbShift}    },
    { KEY_F0 + 15,      {{kbShiftF3},   kbShift}    },
    { KEY_F0 + 16,      {{kbShiftF4},   kbShift}    },
    { KEY_F0 + 17,      {{kbShiftF5},   kbShift}    },
    { KEY_F0 + 18,      {{kbShiftF6},   kbShift}    },
    { KEY_F0 + 19,      {{kbShiftF7},   kbShift}    },
    { KEY_F0 + 20,      {{kbShiftF8},   kbShift}    },
    { KEY_F0 + 21,      {{kbShiftF9},   kbShift}    },
    { KEY_F0 + 22,      {{kbShiftF10},  kbShift}    },
    { KEY_F0 + 23,      {{kbShiftF11},  kbShift}    },
    { KEY_F0 + 24,      {{kbShiftF12},  kbShift}    },
    /* Linux console support for function keys ends here, so please
     * avoid using any of the following: */
    // Ctrl+F1-F12
    { KEY_F0 + 25,      {{kbCtrlF1},    kbCtrlShift}},
    { KEY_F0 + 26,      {{kbCtrlF2},    kbCtrlShift}},
    { KEY_F0 + 27,      {{kbCtrlF3},    kbCtrlShift}},
    { KEY_F0 + 28,      {{kbCtrlF4},    kbCtrlShift}},
    { KEY_F0 + 29,      {{kbCtrlF5},    kbCtrlShift}},
    { KEY_F0 + 30,      {{kbCtrlF6},    kbCtrlShift}},
    { KEY_F0 + 31,      {{kbCtrlF7},    kbCtrlShift}},
    { KEY_F0 + 32,      {{kbCtrlF8},    kbCtrlShift}},
    { KEY_F0 + 33,      {{kbCtrlF9},    kbCtrlShift}},
    { KEY_F0 + 34,      {{kbCtrlF10},   kbCtrlShift}},
    { KEY_F0 + 35,      {{kbCtrlF11},   kbCtrlShift}},
    { KEY_F0 + 36,      {{kbCtrlF12},   kbCtrlShift}},
    // Ctrl+Shift+F(n) supported by ncurses but not Turbo Vision
    // Alt+F1-F12
    { KEY_F0 + 49,      {{kbAltF1},     kbAltShift} },
    { KEY_F0 + 50,      {{kbAltF2},     kbAltShift} },
    { KEY_F0 + 51,      {{kbAltF3},     kbAltShift} },
    { KEY_F0 + 52,      {{kbAltF4},     kbAltShift} },
    { KEY_F0 + 53,      {{kbAltF5},     kbAltShift} },
    { KEY_F0 + 54,      {{kbAltF6},     kbAltShift} },
    { KEY_F0 + 55,      {{kbAltF7},     kbAltShift} },
    { KEY_F0 + 56,      {{kbAltF8},     kbAltShift} },
    { KEY_F0 + 57,      {{kbAltF9},     kbAltShift} },
    { KEY_F0 + 58,      {{kbAltF10},    kbAltShift} },
    { KEY_F0 + 59,      {{kbAltF11},    kbAltShift} },
    { KEY_F0 + 60,      {{kbAltF12},    kbAltShift} }
};

static const const_unordered_map<TStringView, KeyDownEvent> fromCursesHighKey = {
    /* These keys are identified by name. The int value is not known
     * at compilation time. */
    { "kDC3",       {{kbAltDel},        kbAltShift}},
    { "kEND3",      {{kbAltEnd},        kbAltShift}},
    { "kHOM3",      {{kbAltHome},       kbAltShift}},
    { "kIC3",       {{kbAltIns},        kbAltShift}},
    { "kLFT3",      {{kbAltLeft},       kbAltShift}},
    { "kNXT3",      {{kbAltPgDn},       kbAltShift}},
    { "kPRV3",      {{kbAltPgUp},       kbAltShift}},
    { "kRIT3",      {{kbAltRight},      kbAltShift}},
    { "kUP3",       {{kbAltUp},         kbAltShift}},
    { "kDN3",       {{kbAltDown},       kbAltShift}},
    { "kDC4",       {{kbShiftDel},      kbShift | kbAltShift}},
    { "kEND4",      {{kbEnd},           kbShift | kbAltShift}},
    { "kHOM4",      {{kbHome},          kbShift | kbAltShift}},
    { "kIC4",       {{kbShiftIns},      kbShift | kbAltShift}},
    { "kLFT4",      {{kbLeft},          kbShift | kbAltShift}},
    { "kNXT4",      {{kbPgDn},          kbShift | kbAltShift}},
    { "kPRV4",      {{kbPgUp},          kbShift | kbAltShift}},
    { "kRIT4",      {{kbRight},         kbShift | kbAltShift}},
    { "kUP4",       {{kbUp},            kbShift | kbAltShift}},
    { "kDN4",       {{kbDown},          kbShift | kbAltShift}},
    { "kDC5",       {{kbCtrlDel},       kbCtrlShift}},
    { "kEND5",      {{kbCtrlEnd},       kbCtrlShift}},
    { "kHOM5",      {{kbCtrlHome},      kbCtrlShift}},
    { "kIC5",       {{kbCtrlIns},       kbCtrlShift}},
    { "kLFT5",      {{kbCtrlLeft},      kbCtrlShift}},
    { "kNXT5",      {{kbCtrlPgDn},      kbCtrlShift}},
    { "kPRV5",      {{kbCtrlPgUp},      kbCtrlShift}},
    { "kRIT5",      {{kbCtrlRight},     kbCtrlShift}},
    { "kUP5",       {{kbCtrlUp},        kbCtrlShift}},
    { "kDN5",       {{kbCtrlDown},      kbCtrlShift}},
    { "kDC6",       {{kbCtrlDel},       kbCtrlShift | kbShift}},
    { "kEND6",      {{kbCtrlEnd},       kbCtrlShift | kbShift}},
    { "kHOM6",      {{kbCtrlHome},      kbCtrlShift | kbShift}},
    { "kIC6",       {{kbCtrlIns},       kbCtrlShift | kbShift}},
    { "kLFT6",      {{kbCtrlLeft},      kbCtrlShift | kbShift}},
    { "kNXT6",      {{kbCtrlPgDn},      kbCtrlShift | kbShift}},
    { "kPRV6",      {{kbCtrlPgUp},      kbCtrlShift | kbShift}},
    { "kRIT6",      {{kbCtrlRight},     kbCtrlShift | kbShift}},
    { "kUP6",       {{kbCtrlUp},        kbCtrlShift | kbShift}},
    { "kDN6",       {{kbCtrlDown},      kbCtrlShift | kbShift}},
    { "kDC7",       {{kbDel},           kbCtrlShift | kbAltShift}}, // Please do not attempt this one
    { "kEND7",      {{kbEnd},           kbCtrlShift | kbAltShift}},
    { "kHOM7",      {{kbHome},          kbCtrlShift | kbAltShift}},
    { "kIC7",       {{kbIns},           kbCtrlShift | kbAltShift}},
    { "kLFT7",      {{kbLeft},          kbCtrlShift | kbAltShift}},
    { "kNXT7",      {{kbPgDn},          kbCtrlShift | kbAltShift}},
    { "kPRV7",      {{kbPgUp},          kbCtrlShift | kbAltShift}},
    { "kRIT7",      {{kbRight},         kbCtrlShift | kbAltShift}},
    { "kUP7",       {{kbUp},            kbCtrlShift | kbAltShift}},
    { "kDN7",       {{kbDown},          kbCtrlShift | kbAltShift}},
};

NcursesInput::NcursesInput(bool mouse) :
    mouseEnabled(mouse),
    buttonState(0),
    buttonCount(0)
{
    // Capture all keyboard input.
    raw();
    // Disable echoing of pressed keys.
    noecho();
    // No need for ncurses to translate CR into LF.
    nonl();
    // Allow capturing function keys.
    keypad(stdscr, true);
    // Make getch non-blocking.
    nodelay(stdscr, true);
    /* Do not delay too much on ESC key presses, as the Alt modifier works well
     * in most modern terminals. Still, this delay helps ncurses distinguish
     * special key sequences, I believe. */
    set_escdelay(getEnv<int>("TVISION_ESCDELAY", 10));

    if (mouseEnabled)
    {
        lastMousePos = {-1, -1};
        // The button count is not really important. Turbo Vision only checks
        // if it is non-zero.
        buttonCount = 2;
        setMouse(true);
    }

    addListener(this, StdioCtl::in());
    addListener(this, winchFd());
}

NcursesInput::~NcursesInput()
{
    if (mouseEnabled)
        setMouse(false);
}

void NcursesInput::setMouse(bool enable)
{
    // Mouse support in Ncurses is not always good. To work around some issues,
    // we request and parse mouse events manually.
    TStringView seq;
    if (enable)
        seq = "\x1B[?1001s" // Save old highlight mouse reporting.
              "\x1B[?1000h" // Enable mouse reporting.
              "\x1B[?1002h" // Enable mouse drag reporting.
              "\x1B[?1006h" // Enable SGR extended mouse reporting.
            ;
    else
        seq = "\x1B[?1006l" // Disable SGR extended mouse reporting.
              "\x1B[?1002l" // Disable mouse drag reporting.
              "\x1B[?1000l" // Disable mouse reporting.
              "\x1B[?1001r" // Restore old highlight mouse reporting.
            ;
    THardwareInfo::consoleWrite(seq.data(), seq.size());
}

int NcursesInput::getButtonCount()
{
    return buttonCount;
}


bool NcursesInput::getEvent(TEvent &ev)
{
    int k = wgetch(stdscr);

    switch (k)
    {
        case KEY_ESC:
            switch (parseEscapeSeq(ev))
            {
                case Rejected: break;
                case Accepted: return true;
                case Ignored: return false;
            }
            break;
        case KEY_MOUSE:
            return parseCursesMouse(ev);
        case KEY_RESIZE:
            return winchEvent(ev);
    }
    // We have to invoke this when we believe there are no more resize events
    // left. See the SigwinchAware class.
    winchClear();

    if (k != ERR)
    {
        // A Unicode character might be composed of up to 4 UTF-8 bytes.
        int keys[4] = {k}, num_keys = 1;

        ev.what = evKeyDown;
        bool Alt = false;

        if (keys[0] == KEY_ESC)
            detectAlt(keys, Alt);

        if ((uint) keys[0] < 32)
            ev.keyDown = fromNonPrintableAscii[keys[0]];
        else if (keys[0] == 127)
            ev.keyDown = {{kbBack}, 0}; // ^?, Delete
        else if (KEY_MIN < keys[0] && keys[0] < KEY_MAX)
            ev.keyDown = fromCursesKeyCode[keys[0]];
        else if (KEY_MAX < keys[0])
            ev.keyDown = fromCursesHighKey[keyname(keys[0])];

        /* If it hasn't been transformed by any of the previous tables, treat it
         * like a printable character. */
        if (ev.keyDown.keyCode == kbNoKey)
            parsePrintableChar(ev, keys, num_keys);

        if (Alt)
            setAltModifier(ev);

        return ev.keyDown.keyCode != kbNoKey || ev.keyDown.textLength;
    }
    return false;
}

void NcursesInput::detectAlt(int keys[4], bool &Alt)
{
/* Alt+Key combinations begin with the character ESC. To tell the difference,
 * we check if another character has been received. If it has, we consider this
 * an Alt+Key combination. Of course, many other things sent by the terminal
 * begin with ESC, but ncurses already identifies most of them. */
    int k = wgetch(stdscr);
    if (k != ERR)
    {
        keys[0] = k;
        Alt = true;
    }
}

void NcursesInput::parsePrintableChar(TEvent &ev, int keys[4], int &num_keys)
{
    // Read any possible remaining bytes.
    readUtf8Char(keys, num_keys);
    for (int i = 0; i < num_keys; ++i)
        ev.keyDown.text[i] = (char) keys[i];
    ev.keyDown.textLength = (uchar) num_keys;
    // If we are lucky enough, the character will be representable in
    // the active codepage.
    ev.keyDown.charScan.charCode = CpTranslator::fromUtf8({ev.keyDown.text, size_t(num_keys)});
    // Prevent text from triggering Ctrl+Key shortcuts.
    if (ev.keyDown.keyCode <= kbCtrlZ)
        ev.keyDown.keyCode = kbNoKey;
}

void NcursesInput::setAltModifier(TEvent &ev)
{
    ev.keyDown.controlKeyState |= kbAltShift;
    // Set the proper key code if Turbo Vision supports the combination.
    ushort candidate = AltKeyCode[toupper(ev.keyDown.charScan.charCode)];
    if (candidate)
    {
        ev.keyDown.keyCode = candidate;
        ev.keyDown.textLength = 0;
    }
}

void NcursesInput::readUtf8Char(int keys[4], int &num_keys)
{
/* Unicode characters are sent by the terminal byte by byte. To read one, we
 * have to predict the number of bytes it is composed of, then read as many. */
    num_keys += Utf8BytesLeft((char) keys[0]);
    for (int i = 1; i < num_keys; ++i)
        if ( ERR == (keys[i] = wgetch(stdscr)) )
        {
            num_keys = i;
            break;
        }
}

bool NcursesInput::parseCursesMouse(TEvent &ev)
{
    MEVENT mevent;
    if (getmouse(&mevent) == OK)
    {
        uchar newButtons = buttonState;
        if (mevent.bstate & BUTTON1_PRESSED)
            newButtons |= mbLeftButton;
        if (mevent.bstate & BUTTON1_RELEASED)
            newButtons &= ~mbLeftButton;
        if (mevent.bstate & BUTTON2_PRESSED)
            newButtons |= mbMiddleButton;
        if (mevent.bstate & BUTTON2_RELEASED)
            newButtons &= ~mbMiddleButton;
        if (mevent.bstate & BUTTON3_PRESSED)
            newButtons |= mbRightButton;
        if (mevent.bstate & BUTTON3_RELEASED)
            newButtons &= ~mbRightButton;

        uchar mouseWheel = 0;
#if NCURSES_MOUSE_VERSION > 1
        // Mouse wheel support was added in Ncurses v6. Before that, only
        // scroll up would work. It's better not to support wheel scrolling
        // in that case.
        if (mevent.bstate & BUTTON4_PRESSED)
            mouseWheel = mwUp;
        else if (mevent.bstate & BUTTON5_PRESSED)
            mouseWheel = mwDown;
#endif
        return acceptMouseEvent(ev, {mevent.x, mevent.y}, newButtons, mouseWheel);
    }
    else
    {
        // Ncurses sends KEY_MOUSE when reading "\x1B[M" or "\x1B[<" even if mouse support
        // is not enabled. We don't know which one was read. We could query terminal
        // capabilities to deduce it, but it is also possible to just follow
        // a trial and error approach. 'parseSGRMouse' is more likely to fail, so try it first.
        for (auto parseMouse : {&NcursesInput::parseSGRMouse,
                                &NcursesInput::parseX10Mouse})
        {
            GetChBuf buf;
            switch ((this->*parseMouse)(buf, ev))
            {
                case Rejected: buf.reject(); break;
                case Accepted: return true;
                case Ignored: return false;
            }
        }
        return false;
    }
}

bool NcursesInput::acceptMouseEvent(TEvent &ev, TPoint where, uchar buttons, uchar wheel)
{
    // Some terminal emulators send a mouse event every pixel the graphical
    // mouse cursor moves over the window. Filter out those unnecessary
    // events.
    if ( buttons != buttonState || wheel ||
         where.x != lastMousePos.x || where.y != lastMousePos.y )
    {
        ev.what = evMouse;
        ev.mouse = {};
        ev.mouse.buttons = buttonState = buttons;
        ev.mouse.where = lastMousePos = where;
        ev.mouse.wheel = wheel;
        return true;
    }
    return false;
}

NcursesInput::ParseResult NcursesInput::parseEscapeSeq(TEvent &ev)
// Pre: "\x1B" has just been read.
{
    GetChBuf buf;
    if (buf.get() == '[')
    {
        switch (buf.get())
        {
            // Mouse events are usually detected in 'parseCursesMouse'.
            case 'M':
                return parseX10Mouse(buf, ev) == Accepted ? Accepted : Ignored;
            case '<':
                return parseSGRMouse(buf, ev) == Accepted ? Accepted : Ignored;
        }
    }
    buf.reject();
    return Rejected;
}

NcursesInput::ParseResult NcursesInput::parseX10Mouse(GetChBuf &buf, TEvent &ev)
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

    TPoint where = {col, row};
    uchar wheel = 0;
    uchar newButtons = buttonState;
    switch (but)
    {
        case 0: // Press.
        case 32: // Drag.
            newButtons |= mbLeftButton; break;
        case 1:
        case 33:
            newButtons |= mbMiddleButton; break;
        case 2:
        case 34:
            newButtons |= mbRightButton; break;
        case 3: newButtons = 0; break; // Release.
        case 64: wheel = mwUp; break;
        case 65: wheel = mwDown; break;
    }
    return acceptMouseEvent(ev, where, newButtons, wheel) ? Accepted : Ignored;
}

NcursesInput::ParseResult NcursesInput::parseSGRMouse(GetChBuf &buf, TEvent &ev)
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
        if ((*i = buf.getNum()) == ERR)
            return Rejected;
    // Make the coordinates zero-based.
    --row, --col;
    // Finally, the press/release state.
    state = buf.last();
    if (!(state == 'M' || state == 'm')) return Rejected;

    TPoint where = {col, row};
    uchar wheel = 0;
    uchar newButtons = buttonState;
    if (state == 'M') // Press, wheel or drag.
    {
        switch (but)
        {
            case 0:
            case 32:
                newButtons |= mbLeftButton; break;
            case 1:
            case 33:
                newButtons |= mbMiddleButton; break;
            case 2:
            case 34:
                newButtons |= mbRightButton; break;
            case 64: wheel = mwUp; break;
            case 65: wheel = mwDown; break;
        }
    }
    else // Release.
    {
        switch (but)
        {
            case 0: newButtons &= ~mbLeftButton; break;
            case 1: newButtons &= ~mbMiddleButton; break;
            case 2: newButtons &= ~mbRightButton; break;
        }
    }
    return acceptMouseEvent(ev, where, newButtons, wheel) ? Accepted : Ignored;
}

#endif // HAVE_NCURSES

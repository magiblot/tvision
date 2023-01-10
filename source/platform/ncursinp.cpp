#ifdef HAVE_NCURSES

#define Uses_THardwareInfo
#define Uses_TKeys
#define Uses_TEvent
#include <tvision/tv.h>

#include <ncurses.h>
#include <internal/ncursinp.h>
#include <internal/terminal.h>
#include <internal/getenv.h>
#include <internal/utf8.h>
#include <internal/stdioctl.h>
#include <internal/codepage.h>
#include <internal/constmap.h>
#include <string>
#include <chrono>

using std::chrono::milliseconds;
using std::chrono::steady_clock;

namespace tvision
{

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

static const const_unordered_map<ushort, KeyDownEvent> fromCursesKeyCode = {
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
    // Keypad
    { KEY_A1,           {{kbHome},      0}          },
    { KEY_A3,           {{kbPgUp},      0}          },
    { KEY_C1,           {{kbEnd},       0}          },
    { KEY_C3,           {{kbPgDn},      0}          },
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
    // Ctrl+Shift+F1-12
    { KEY_F0 + 37,      {{kbCtrlF1},    kbShift | kbCtrlShift}},
    { KEY_F0 + 38,      {{kbCtrlF2},    kbShift | kbCtrlShift}},
    { KEY_F0 + 39,      {{kbCtrlF3},    kbShift | kbCtrlShift}},
    { KEY_F0 + 40,      {{kbCtrlF4},    kbShift | kbCtrlShift}},
    { KEY_F0 + 41,      {{kbCtrlF5},    kbShift | kbCtrlShift}},
    { KEY_F0 + 42,      {{kbCtrlF6},    kbShift | kbCtrlShift}},
    { KEY_F0 + 43,      {{kbCtrlF7},    kbShift | kbCtrlShift}},
    { KEY_F0 + 44,      {{kbCtrlF8},    kbShift | kbCtrlShift}},
    { KEY_F0 + 45,      {{kbCtrlF9},    kbShift | kbCtrlShift}},
    { KEY_F0 + 46,      {{kbCtrlF10},   kbShift | kbCtrlShift}},
    { KEY_F0 + 47,      {{kbCtrlF11},   kbShift | kbCtrlShift}},
    { KEY_F0 + 48,      {{kbCtrlF12},   kbShift | kbCtrlShift}},
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

static const auto fromCursesHighKey =
    const_unordered_map<uint64_t, KeyDownEvent>::with_string_keys({
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
    { "kDC4",       {{kbAltDel},        kbShift | kbAltShift}},
    { "kEND4",      {{kbAltEnd},        kbShift | kbAltShift}},
    { "kHOM4",      {{kbAltHome},       kbShift | kbAltShift}},
    { "kIC4",       {{kbAltIns},        kbShift | kbAltShift}},
    { "kLFT4",      {{kbAltLeft},       kbShift | kbAltShift}},
    { "kNXT4",      {{kbAltPgDn},       kbShift | kbAltShift}},
    { "kPRV4",      {{kbAltPgUp},       kbShift | kbAltShift}},
    { "kRIT4",      {{kbAltRight},      kbShift | kbAltShift}},
    { "kUP4",       {{kbAltUp},         kbShift | kbAltShift}},
    { "kDN4",       {{kbAltDown},       kbShift | kbAltShift}},
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
    { "kDC7",       {{kbAltDel},        kbCtrlShift | kbAltShift}}, // Please do not attempt this one
    { "kEND7",      {{kbAltEnd},        kbCtrlShift | kbAltShift}},
    { "kHOM7",      {{kbAltHome},       kbCtrlShift | kbAltShift}},
    { "kIC7",       {{kbAltIns},        kbCtrlShift | kbAltShift}},
    { "kLFT7",      {{kbAltLeft},       kbCtrlShift | kbAltShift}},
    { "kNXT7",      {{kbAltPgDn},       kbCtrlShift | kbAltShift}},
    { "kPRV7",      {{kbAltPgUp},       kbCtrlShift | kbAltShift}},
    { "kRIT7",      {{kbAltRight},      kbCtrlShift | kbAltShift}},
    { "kUP7",       {{kbAltUp},         kbCtrlShift | kbAltShift}},
    { "kDN7",       {{kbAltDown},       kbCtrlShift | kbAltShift}},
    { "kpCMA",      {{'+'},             0, {'+'}, 1}},
    { "kpADD",      {{'+'},             0, {'+'}, 1}},
    { "kpSUB",      {{'-'},             0, {'-'}, 1}},
    { "kpMUL",      {{'*'},             0, {'*'}, 1}},
    { "kpDIV",      {{'/'},             0, {'/'}, 1}},
    { "kpZRO",      {{'0'},             0, {'0'}, 1}},
    { "kpDOT",      {{'.'},             0, {'.'}, 1}},
    { "ka2",        {{kbUp},            0}},
    { "kb1",        {{kbLeft},          0}},
    { "kb3",        {{kbRight},         0}},
    { "kc2",        {{kbDown},          0}},
});

static class NcursesInputGetter : public InputGetter
{
    int get() noexcept override
    {
        int k = wgetch(stdscr);
        return k != ERR ? k : -1;
    }

    void unget(int k) noexcept override
    {
        ungetch(k);
    }
} ncInputGetter;

NcursesInput::NcursesInput( StdioCtl &aIo, NcursesDisplay &,
                            InputState &aState, bool mouse ) noexcept :
    InputStrategy(aIo.in()),
    io(aIo),
    state(aState),
    mouseEnabled(mouse)
{
    // Capture all keyboard input.
    raw();
    // Disable echoing of pressed keys.
    noecho();
    // No need for ncurses to translate CR into LF.
    nonl();
    // Allow capturing function keys.
    keypad(stdscr, true);
    // Make getch practically non-blocking. Some terminals may feed input slowly.
    // Note that we only risk blocking when reading multibyte characters
    // or parsing escape sequences.
    wtimeout(stdscr, readTimeout);
    /* Do not delay too much on ESC key presses, as the Alt modifier works well
     * in most modern terminals. Still, this delay helps ncurses distinguish
     * special key sequences, I believe. */
    set_escdelay(getEnv<int>("TVISION_ESCDELAY", 10));

    TermIO::keyModsOn(io);
    if (mouseEnabled)
        TermIO::mouseOn(io);
}

NcursesInput::~NcursesInput()
{
    if (mouseEnabled)
        TermIO::mouseOff(io);
    TermIO::keyModsOff(io, *this, state);
    consumeUnprocessedInput();
}

int NcursesInput::getButtonCount() noexcept
{
    // The exact button count is not really important. Turbo Vision
    // only checks whether it is non-zero.
    return mouseEnabled ? 2 : 0;
}

int NcursesInput::getch_nb() noexcept
{
    wtimeout(stdscr, 0);
    int k = wgetch(stdscr);
    wtimeout(stdscr, readTimeout);
    return k;
}

bool NcursesInput::hasPendingEvents() noexcept
{
    int k = getch_nb();
    if (k != ERR)
    {
        ungetch(k);
        return true;
    }
    return false;
}

bool NcursesInput::getEvent(TEvent &ev) noexcept
{
    GetChBuf buf(ncInputGetter);
    switch (TermIO::parseEvent(buf, ev, state))
    {
        case Rejected: buf.reject(); break;
        case Accepted: return true;
        case Ignored: return false;
    }

    int k = wgetch(stdscr);

    if (k == KEY_RESIZE)
        return false; // Handled by SigwinchHandler.
    else if (k == KEY_MOUSE)
        return parseCursesMouse(ev);

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

        // If it hasn't been transformed by any of the previous tables,
        // and it's not a curses key, treat it like a printable character.
        if (ev.keyDown.keyCode == kbNoKey && keys[0] < KEY_MIN)
            parsePrintableChar(ev, keys, num_keys);

        if (Alt)
        {
            ev.keyDown.controlKeyState |= kbAltShift;
            TermIO::normalizeKey(ev.keyDown);
        }
        if (state.bracketedPaste)
            ev.keyDown.controlKeyState |= kbPaste;

        return ev.keyDown.keyCode != kbNoKey || ev.keyDown.textLength;
    }
    return false;
}

void NcursesInput::detectAlt(int keys[4], bool &Alt) noexcept
{
/* Alt+Key combinations begin with the character ESC. To tell the difference,
 * we check if another character has been received. If it has, we consider this
 * an Alt+Key combination. Of course, many other things sent by the terminal
 * begin with ESC, but ncurses already identifies most of them. */
    int k = getch_nb();
    if (k != ERR)
    {
        keys[0] = k;
        Alt = true;
    }
}

void NcursesInput::parsePrintableChar(TEvent &ev, int keys[4], int &num_keys) noexcept
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

void NcursesInput::readUtf8Char(int keys[4], int &num_keys) noexcept
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

bool NcursesInput::parseCursesMouse(TEvent &ev) noexcept
{
    MEVENT mevent;
    if (getmouse(&mevent) == OK)
    {
        ev.what = evMouse;
        ev.mouse = {};
        ev.mouse.where = {mevent.x, mevent.y};
        if (mevent.bstate & BUTTON1_PRESSED)
            state.buttons |= mbLeftButton;
        if (mevent.bstate & BUTTON1_RELEASED)
            state.buttons &= ~mbLeftButton;
        if (mevent.bstate & BUTTON2_PRESSED)
            state.buttons |= mbMiddleButton;
        if (mevent.bstate & BUTTON2_RELEASED)
            state.buttons &= ~mbMiddleButton;
        if (mevent.bstate & BUTTON3_PRESSED)
            state.buttons |= mbRightButton;
        if (mevent.bstate & BUTTON3_RELEASED)
            state.buttons &= ~mbRightButton;
        ev.mouse.buttons = state.buttons;

#if NCURSES_MOUSE_VERSION > 1
        // Mouse wheel support was added in Ncurses v6. Before that, only
        // scroll up would work. It's better not to support wheel scrolling
        // in that case.
        if (mevent.bstate & BUTTON4_PRESSED)
            ev.mouse.wheel = mwUp;
        else if (mevent.bstate & BUTTON5_PRESSED)
            ev.mouse.wheel = mwDown;
#endif
        return true;
    }
    else
    {
        // Ncurses sends KEY_MOUSE when reading "\x1B[M" or "\x1B[<" even if mouse support
        // is not enabled. We don't know which one was read. We could query terminal
        // capabilities to deduce it, but it is also possible to just follow
        // a trial and error approach. 'parseSGRMouse' is more likely to fail, so try it first.
        for (auto &parseMouse : {TermIO::parseSGRMouse,
                                 TermIO::parseX10Mouse})
        {
            GetChBuf buf(ncInputGetter);
            switch (parseMouse(buf, ev, state))
            {
                case Rejected: buf.reject(); break;
                case Accepted: return true;
                case Ignored: return false;
            }
        }
        return false;
    }
}

void NcursesInput::consumeUnprocessedInput() noexcept
{
    // This may be useful if the terminal has sent us events right before we
    // disabled keyboard and mouse extensions, or if we have been killed by
    // a signal.
    TEvent ev;
    wtimeout(stdscr, 0);
    auto begin = steady_clock::now();
    while ( getEvent(ev) &&
            steady_clock::now() - begin <= milliseconds(readTimeout) )
        ;
    wtimeout(stdscr, readTimeout);
}

} // namespace tvision

#endif // HAVE_NCURSES

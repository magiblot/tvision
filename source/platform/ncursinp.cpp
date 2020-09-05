#ifdef HAVE_NCURSES

#define Uses_THardwareInfo
#define Uses_TKeys
#define Uses_TEvent
#include <tvision/tv.h>

#include <ncurses.h>
#include <unordered_map>
#include <internal/ncursinp.h>
#include <internal/getenv.h>
#include <internal/utf8.h>
#include <internal/codepage.h>
using std::unordered_map;

// tables.cpp
extern unordered_map<char, KeyDownEvent> fromNonPrintableAscii;
extern unordered_map<char, ushort> AltKeyCode;

NcursesInput::NcursesInput(bool mouse) :
    mouseForced(false)
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

    if (mouse)
    {
        buttonState = 0;
        lastMousePos = {-1, -1};
        mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, 0);
        /* ncurses has the ability to recognize double/triple mouse clicks,
         * but this results in a delay before reporting the event. We don't
         * want this to happen. */
        mouseinterval(0);
        // This will do the trick for now.
        buttonCount = 2;
        // Force enable mouse drag support.
        auto TERM = getEnv<std::string_view>("TERM");
        if (TERM.find("xterm") == 0 || TERM.find("rxvt") == 0)
        {
            mouseForced = true;
            printEscapeSeq("\x1B[?1002h");
        }
    }
    else buttonCount = 0;

    addListener(this, 0);
    addListener(this, winchFd());
}

NcursesInput::~NcursesInput()
{
    if (mouseForced)
        printEscapeSeq("\x1B[?1002l");
    // Disable mouse mode.
    mousemask(0, 0);
}

void NcursesInput::printEscapeSeq(const char* seq)
{
    fputs(seq, stdout);
    fflush(stdout);
}

int NcursesInput::getButtonCount()
{
    return buttonCount;
}

#define KEY_ESC '\x1B'

bool NcursesInput::getEvent(TEvent &ev)
{
    // A Unicode character might be composed of up to 4 UTF-8 bytes.
    int keys[4], num_keys = 1;
    keys[0] = wgetch(stdscr);

    if (keys[0] == KEY_RESIZE)
        return winchEvent(ev);
    else winchClear();

    if (keys[0] != ERR)
    {
        if (keys[0] == KEY_MOUSE)
            return parseMouseEvent(ev);

        ev.what = evKeyDown;
        bool Alt = false;

        if ((char) keys[0] == KEY_ESC)
            detectAlt(keys, Alt);

        if (keys[0] <= 127)
            ev.keyDown = fromNonPrintableAscii[(char) keys[0]];
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

        return ev.keyDown.keyCode != kbNoKey || *(uint32_t *) ev.keyDown.text;
    }
    return false;
}

void NcursesInput::detectAlt(int keys[4], bool &Alt)
{
/* Alt+Key combinations begin with the character ESC. To tell the difference,
 * we check if another character has been received. If it has, we consider this
 * an Alt+Key combination. Of course, many other things sent by the terminal
 * begin with ESC, but ncurses already identifies most of them. */
    keys[1] = wgetch(stdscr);
    if (keys[1] != ERR)
    {
        keys[0] = keys[1];
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
}

void NcursesInput::setAltModifier(TEvent &ev)
{
    ev.keyDown.controlKeyState |= kbAltShift;
    // Set the proper key code if Turbo Vision supports the combination.
    ushort candidate = AltKeyCode[toupper(ev.keyDown.charScan.charCode)];
    if (candidate)
        ev.keyDown.keyCode = candidate;
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

bool NcursesInput::parseMouseEvent(TEvent &ev)
{
    MEVENT mevent;
    if (getmouse(&mevent) == OK)
    {
        ev.what = evMouse;
        uchar oldButtons = buttonState;

        if (mevent.bstate & BUTTON1_PRESSED)
            buttonState |= mbLeftButton;
        if (mevent.bstate & BUTTON1_RELEASED)
            buttonState &= ~mbLeftButton;
        if (mevent.bstate & BUTTON2_PRESSED)
            buttonState |= mbMiddleButton;
        if (mevent.bstate & BUTTON2_RELEASED)
            buttonState &= ~mbMiddleButton;
        if (mevent.bstate & BUTTON3_PRESSED)
            buttonState |= mbRightButton;
        if (mevent.bstate & BUTTON3_RELEASED)
            buttonState &= ~mbRightButton;

        int mouseWheel = 0;
#if NCURSES_MOUSE_VERSION > 1
        // Mouse wheel support was added in Ncurses v6. Before that, only
        // scroll up would work. It's better not to support wheel scrolling
        // in that case.
        if (mevent.bstate & BUTTON4_PRESSED)
            mouseWheel = mwUp;
        else if (mevent.bstate & BUTTON5_PRESSED)
            mouseWheel = mwDown;
#endif
        /* Some terminal emulators send a mouse event every pixel the graphical
         * mouse cursor moves over the window. Filter out those unnecessary
         * events. */
        if ( oldButtons != buttonState || mouseWheel ||
             mevent.x != lastMousePos.x || mevent.y != lastMousePos.y )
        {
            ev.mouse.buttons = buttonState;
            ev.mouse.where.x = lastMousePos.x = mevent.x;
            ev.mouse.where.y = lastMousePos.y = mevent.y;
            ev.mouse.wheel = mouseWheel;
            return true;
        }
    }
    return false;
}

#endif // HAVE_NCURSES

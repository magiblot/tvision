#define Uses_THardwareInfo
#define Uses_TKeys
#define Uses_TEvent
#include <tvision/tv.h>

#include <ncurses.h>
#include <thread>
#include <string>
#include <unordered_map>
#include <platform.h>
#include <utf8.h>
using std::unordered_map;
using std::string;
using std::thread;
using waiter = AsyncInputStrategy::waiter;

// tables.cpp
extern unordered_map<char, KeyDownEvent> fromNonPrintableAscii;
extern unordered_map<char, ushort> AltKeyCode;

NcursesInput::NcursesInput(bool mouse)
{
    // Capture keyboard input, but allow exiting with Ctrl+C.
    cbreak();
    // Disable echoing of pressed keys.
    noecho();
    // No need for ncurses to translate CR into LF.
    nonl();
    // Allow capturing function keys.
    keypad(stdscr, true);
    /* Do not delay too much on ESC key presses. This delay helps ncurses
     * distinguish special key sequences. */
    set_escdelay(10);

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
    }
    else buttonCount = 0;
}

NcursesInput::~NcursesInput()
{
    // Disable mouse mode.
    mousemask(0, 0);
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
    if (keys[0] != ERR)
    {
        if (keys[0] == KEY_MOUSE)
            return parseMouseEvent(ev);

        ev.what = evKeyDown;
        bool Alt = false;
        // Make getch non-blocking, because we might have to read more bytes.
        nodelay(stdscr, true);

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

        nodelay(stdscr, false);
        return ev.keyDown.keyCode != kbNoKey;
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
    // Reserve enough space for a zero-terminated char array.
    char Utf8str[5];
    for (int i = 0; i < num_keys; ++i)
        Utf8str[i] = (char) keys[i];
    Utf8str[num_keys] = '\0';
    // If we are lucky enough, the character will be representable in CP437.
    ev.keyDown.charScan.charCode = Utf8toCp437[Utf8str];
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
        keys[i] = wgetch(stdscr);
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
        if (mevent.bstate & BUTTON3_PRESSED)
            buttonState |= mbRightButton;
        if (mevent.bstate & BUTTON3_RELEASED)
            buttonState &= ~mbRightButton;

        /* Some terminal emulators send a mouse event every pixel the graphical
         * mouse cursor moves over the window. Filter out those unnecessary
         * events. */
        if ( oldButtons != buttonState ||
             mevent.x != lastMousePos.x || mevent.y != lastMousePos.y )
        {
            ev.mouse.buttons = buttonState;
            ev.mouse.where.x = lastMousePos.x = mevent.x;
            ev.mouse.where.y = lastMousePos.y = mevent.y;
            return true;
        }
    }
    return false;
}

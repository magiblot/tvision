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
extern const char* cp437toUtf8[256];
extern unordered_map<char, KeyDownEvent> fromNonPrintableAscii;
extern unordered_map<char, ushort> AltKeyCode;

void NcursesInput::startInput()
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

    /* Fill the table that translates UTF-8 sequences into CP437 characters.
     * Of course this is better than doing it by hand!
     * The reason why this variable is not static is that it's otherwise
     * affected by some sort of static initialization fiasco. */
    for (int i = 0; i < 256; ++i)
        Utf8toCp437[cp437toUtf8[i]] = i;

    inputThread = thread([this] {
        waiter w;
        while (true)
        {
            TEvent ev = {};
            if (getEvent(ev))
                notifyEvent(ev, w);
        }
    });

}

void NcursesInput::endInput()
{
    /* There's no way yet to terminate the input thread, so it has to be
     * detached before becoming out of scope. */
    inputThread.detach();
}

#define KEY_ESC '\x1B'

bool NcursesInput::getEvent(TEvent &ev)
{
    // A Unicode character might be composed of up to 4 UTF-8 bytes.
    int keys[4], num_keys = 1;
    keys[0] = wgetch(stdscr);
    if (keys[0] != ERR)
    {
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

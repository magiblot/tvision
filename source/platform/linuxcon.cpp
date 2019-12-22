#include <internal/linuxcon.h>
#include <linux/keyboard.h>
#include <linux/vt.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstring>
#include <unordered_map>
using std::unordered_map;

LinuxConsoleStrategy::LinuxConsoleStrategy(DisplayStrategy *d, FdInputStrategy *i) :
    PlatformStrategy(d, i), gpm(new GpmInput())
{
    /* The FdInputStrategy instance which reads key events is stored in
     * the 'input' attribute of PlatformStrategy, while the GpmInput instance
     * is stored in the 'gpm' attribute of this class. */
    if (input)
        input->overrideEventGetter([&] (TEvent &ev) {
            return patchKeyEvent(ev);
        });
}

int LinuxConsoleStrategy::getButtonCount()
{
    return gpm->getButtonCount();
}

void LinuxConsoleStrategy::flushScreen()
{
    PlatformStrategy::flushScreen();
    gpm->drawPointer();
}

// tables.cpp
extern unordered_map<ulong, unordered_map<ushort, ushort>> keyCodeWithModifiers;

bool LinuxConsoleStrategy::patchKeyEvent(TEvent &ev)
{
    // Prevent pointer from disappearing on key press.
    gpm->drawPointer();
    /* The keyboard event getter is usually unaware of key modifiers in the
     * console, so we add them on top of the previous translation. */
    if (input->getEvent(ev))
    {
        applyKeyboardModifiers(ev.keyDown);
        ushort keyCode = keyCodeWithModifiers[ev.keyDown.controlKeyState][ev.keyDown.keyCode];
        if (keyCode)
            ev.keyDown.keyCode = keyCode;
        return true;
    }
    return false;
}

void LinuxConsoleStrategy::applyKeyboardModifiers(KeyDownEvent &key)
{
    char res = 6;
    ulong actualModifiers = 0;
    if (ioctl(0, TIOCLINUX, &res) != -1)
    {
        /* Turbo Vision uses the Shift flag only on special key combinations,
         * not on printable characters, where scanCode is 0. */
        if ((res & (1 << KG_SHIFT)) && key.charScan.scanCode)
            actualModifiers |= kbShift;
        if (res & (1 << KG_CTRL))
            actualModifiers |= kbCtrlShift;
        if (res & (1 << KG_ALT))
            actualModifiers |= kbAltShift;
        /* Modifiers must be replaced and not OR'd. For instance, Shift+Tab is
         * captured as Alt+Tab by ncurses, and discarding the current Alt flag
         * makes it possible for it to be processed properly by Turbo Vision. */
        key.controlKeyState = actualModifiers;
    }
}

bool LinuxConsoleStrategy::ttyActive()
{
    // Returns true if the currently active tty
    // is the same where this application draws to.
    static const int startupTTY = getStartupTTY();
    return startupTTY == getActiveTTY();
}

int LinuxConsoleStrategy::getStartupTTY()
{
    int tty = getTTY(1); // tty where the program is being drawn.
    if (tty < 0)
        tty = getTTY(0); // fallback: tty the program grabs input from.
    return tty;
}

int LinuxConsoleStrategy::getActiveTTY()
{
    struct vt_stat vtstat;
    if (ioctl(0, VT_GETSTATE, &vtstat) != -1)
        return vtstat.v_active;
    // This should never fail. If stdin wasn't a console file descriptor,
    // LinuxConsoleStrategy wouldn't have been chosen.
    return -2;
}

int LinuxConsoleStrategy::getTTY(int fd)
{
    char *name = ttyname(fd);
    if (name)
    {
        int n = 0, length = strlen(name);
        int i = length - 1;
        // Read the tty name (e.g. '/dev/tty2') backwards to decode the tty number.
        while (i >= 0 && uchar(name[i] - '0') <= uchar('9' - '0')) --i;
        while (++i < length)
        {
            n *= 10;
            n += name[i] - '0';
        }
        return n;
    }
    return -1;
}

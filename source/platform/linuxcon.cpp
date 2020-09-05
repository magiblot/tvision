#ifdef __linux__

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
        ((FdInputStrategy &) *input).overrideEventGetter([&] (TEvent &ev) {
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
}

// tables.cpp
extern unordered_map<ulong, unordered_map<ushort, ushort>> keyCodeWithModifiers;

bool LinuxConsoleStrategy::patchKeyEvent(TEvent &ev)
{
    /* The keyboard event getter is usually unaware of key modifiers in the
     * console, so we add them on top of the previous translation. */
    if (input->getEvent(ev))
    {
        applyKeyboardModifiers(ev.keyDown);
        ushort keyCode = keyCodeWithModifiers[ev.keyDown.controlKeyState][ev.keyDown.keyCode];
        if (keyCode)
            ev.keyDown.keyCode = keyCode;
        // Do not set the Ctrl flag on these, as that would alter their meaning.
        // Note that there already exist kbCtrlBack and kbCtrlEnter.
        else if (ev.keyDown.keyCode == kbBack || ev.keyDown.keyCode == kbTab || ev.keyDown.keyCode == kbEnter)
            ev.keyDown.controlKeyState &= ~kbCtrlShift;
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
        if ((res & (1 << KG_SHIFT)) && !key.textLength)
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

#endif // __linux__

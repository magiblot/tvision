#include <platform.h>
#include <linux/keyboard.h>
#include <sys/ioctl.h>
#include <unordered_map>
using std::unordered_map;

LinuxConsoleStrategy::LinuxConsoleStrategy(DisplayStrategy *d, AsyncInputStrategy *i) :
    PlatformStrategy(d, new GpmInput()), keyboard(i)
{
    /* The GpmInput instance is stored in the 'input' attribute of
     * PlatformStrategy, while the AsyncInputStrategy instance which reads
     * key events is stored in this class, so that we can override its
     * getEvent method. */
    if (keyboard)
        keyboard->startInputThread([&] (TEvent &ev) {
            return patchKeyEvent(ev);
        });
}

LinuxConsoleStrategy::~LinuxConsoleStrategy()
{
    if (keyboard) keyboard->endInputThread();
}

void LinuxConsoleStrategy::flushScreen()
{
    PlatformStrategy::flushScreen();
    ((GpmInput*) input.get())->drawPointer();
}

// tables.cpp
extern unordered_map<ulong, unordered_map<ushort, ushort>> keyCodeWithModifiers;

bool LinuxConsoleStrategy::patchKeyEvent(TEvent &ev)
{
    /* The keyboard event getter is usually unaware of key modifiers in the
     * console, so we add them on top of the previous translation. */
    if (keyboard->getEvent(ev))
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

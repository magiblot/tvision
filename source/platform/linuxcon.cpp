#ifdef __linux__

#include <internal/linuxcon.h>
#include <internal/stdioctl.h>
#include <internal/gpminput.h>
#include <internal/terminal.h>
#include <linux/keyboard.h>
#include <linux/vt.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

namespace tvision
{

LinuxConsoleStrategy &LinuxConsoleStrategy::create( const StdioCtl &io, ScreenLifetime &scrl,
                                                    DisplayStrategy &display,
                                                    InputStrategy &input ) noexcept
{
    return *new LinuxConsoleStrategy(io, scrl, display, input, GpmInput::create());
}

LinuxConsoleStrategy::~LinuxConsoleStrategy()
{
    // The superclass always deletes 'input'. If we have a different object
    // in 'wrapper.input', we delete it too.
    if (&wrapper.input != &input)
        delete &wrapper.input;
}

void LinuxConsoleStrategy::forEachSource(void *args, void (&action)(void *, EventSource &)) noexcept
{
    action(args, wrapper);
    if (&wrapper.input != &input)
        action(args, input);
    UnixConsoleStrategy::forEachPrivateSource(args, action);
}

bool LinuxConsoleInput::getEvent(TEvent &ev) noexcept
{
    // The keyboard event getter is usually unaware of key modifiers in the
    // console, so we add them on top of the previous translation.
    if (input.getEvent(ev))
    {
        auto &keyCode = ev.keyDown.keyCode;
        ev.keyDown.controlKeyState = getKeyboardModifiers(io);
        // Prevent Ctrl+H/Ctrl+I/Ctrl+J/Ctrl+M from being interpreted as
        // Ctrl+Back/Ctrl+Tab/Ctrl+Enter.
        if (keyCode == kbBack || keyCode == kbTab || keyCode == kbEnter)
            ev.keyDown.controlKeyState &= ~kbCtrlShift;
        // Special cases for Ctrl+Back and Shift+Tab.
        if (keyCode == 0x001F && (ev.keyDown.controlKeyState & kbCtrlShift))
            keyCode = kbCtrlBack;
        else if (keyCode == kbAltTab && ((ev.keyDown.controlKeyState & (kbShift | kbCtrlShift | kbAltShift)) == kbShift))
            keyCode = kbShiftTab;
        TermIO::fixKey(ev.keyDown);
        return true;
    }
    return false;
}

bool LinuxConsoleInput::hasPendingEvents() noexcept
{
    return input.hasPendingEvents();
}

ushort LinuxConsoleInput::getKeyboardModifiers(const StdioCtl &io) noexcept
{
    char res = 6;
    ulong actualModifiers = 0;
    if (ioctl(io.in(), TIOCLINUX, &res) != -1)
    {
        if (res & (1 << KG_SHIFT))
            actualModifiers |= kbShift;
        if (res & (1 << KG_CTRL))
            actualModifiers |= kbCtrlShift;
        if (res & (1 << KG_ALT))
            actualModifiers |= kbAltShift;
    }
    return actualModifiers;
}

} // namespace tvision

#endif // __linux__

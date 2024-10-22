#ifdef __linux__

#define Uses_TEvent
#define Uses_TKeys
#include <tvision/tv.h>

#include <internal/linuxcon.h>
#include <internal/stdioctl.h>
#include <internal/gpminput.h>
#include <internal/termio.h>
#include <internal/sigwinch.h>
#include <linux/keyboard.h>
#include <linux/vt.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

namespace tvision
{

inline LinuxConsoleStrategy::LinuxConsoleStrategy( DisplayStrategy &aDisplay,
                                                   LinuxConsoleInput &aWrapper,
                                                   InputState &aInputState,
                                                   SigwinchHandler *aSigwinch,
                                                   GpmInput *aGpm ) noexcept :
    ConsoleStrategy( aDisplay,
                     aGpm ? *aGpm : aWrapper.input,
                     {&aWrapper, aGpm, aSigwinch} ),
    inputState(aInputState),
    sigwinch(aSigwinch),
    wrapper(aWrapper),
    gpm(aGpm)
{
}

LinuxConsoleStrategy &LinuxConsoleStrategy::create( StdioCtl &io,
                                                    DisplayBuffer &displayBuf,
                                                    InputState &inputState,
                                                    DisplayStrategy &display,
                                                    InputStrategy &input ) noexcept
{
    auto *sigwinch = SigwinchHandler::create();
    auto &wrapper = *new LinuxConsoleInput(io, input);
    auto *gpm = GpmInput::create(displayBuf);
    return *new LinuxConsoleStrategy(display, wrapper, inputState, sigwinch, gpm);
}

LinuxConsoleStrategy::~LinuxConsoleStrategy()
{
    delete sigwinch;
    delete gpm;
    delete &wrapper.input;
    delete &wrapper;
    delete &display;
    delete &inputState;
}

bool LinuxConsoleInput::getEvent(TEvent &ev) noexcept
{
    // The keyboard event getter is usually unaware of key modifiers in the
    // console, so we add them on top of the previous translation.
    if (input.getEvent(ev))
    {
        auto &keyCode = ev.keyDown.keyCode;
        ev.keyDown.controlKeyState = getKeyboardModifiers();
        // Prevent Ctrl+H/Ctrl+I/Ctrl+J/Ctrl+M from being interpreted as
        // Ctrl+Back/Ctrl+Tab/Ctrl+Enter.
        if (keyCode == kbBack || keyCode == kbTab || keyCode == kbEnter)
            ev.keyDown.controlKeyState &= ~kbCtrlShift;
        // Special cases for Ctrl+Back and Shift+Tab.
        if (keyCode == 0x001F && (ev.keyDown.controlKeyState & kbCtrlShift))
            keyCode = kbCtrlBack;
        else if (keyCode == kbAltTab && ((ev.keyDown.controlKeyState & (kbShift | kbCtrlShift | kbAltShift)) == kbShift))
            keyCode = kbShiftTab;
        TermIO::normalizeKey(ev.keyDown);
        return true;
    }
    return false;
}

bool LinuxConsoleInput::hasPendingEvents() noexcept
{
    return input.hasPendingEvents();
}

ushort LinuxConsoleInput::getKeyboardModifiers() noexcept
{
    char res = 6;
    if (ioctl(io.in(), TIOCLINUX, &res) != -1)
        return convertLinuxKeyModifiers(res);
    return 0;
}

ushort LinuxConsoleInput::convertLinuxKeyModifiers(ushort linuxShiftState) noexcept
{
    constexpr struct
    {
        uchar kb;
        uchar tv;
    } linuxModifierFlags[] =
    {
        {1 << KG_SHIFT, kbShift},
        {1 << KG_CTRLL, kbLeftCtrl},
        {1 << KG_CTRLR, kbRightCtrl},
        {1 << KG_ALT, kbLeftAlt},
        {1 << KG_ALTGR, kbRightAlt},
    };

    ushort modifiers = 0;
    for (const auto &flags : linuxModifierFlags)
        if (linuxShiftState & flags.kb)
            modifiers |= flags.tv;
    // It may be that KG_CTRL was detected, but not KG_CTRLL or KG_CTRLR.
    if ((linuxShiftState & (1 << KG_CTRL)) && !(modifiers & kbCtrlShift))
        modifiers |= kbLeftCtrl;
    return modifiers;
}

} // namespace tvision

#endif // __linux__

#ifdef HAVE_GPM

#define Uses_TEvent
#define Uses_TKeys
#include <tvision/tv.h>

#include <internal/gpminput.h>
#include <internal/dispbuff.h>
#include <internal/linuxcon.h>
#include <linux/keyboard.h>
#include <stdlib.h>
#include <gpm.h>

namespace tvision
{

GpmInput *GpmInput::create(DisplayBuffer &displayBuf) noexcept
{
    // Let coordinates begin at zero instead of one.
    gpm_zerobased = 1;
    Gpm_Connect conn = {
        .eventMask = GPM_DOWN | GPM_UP | GPM_DRAG | GPM_MOVE,
        .defaultMask = 0, // Disable cursor drawing by the server.
        // Report keyboard modifiers except Shift, which terminal emulators
        // usually reserve for selecting text. Here GPM will do the same.
        .minMod = 0,
        .maxMod = (ushort) ~(1 << KG_SHIFT) };
    // Because we only instantiate GPM in the Linux console, unset the
    // TERM variable during Gpm_Open so that GPM won't assume it is being
    // ran under xterm (e.g. if TERM=xterm), and 'gpm_fd' won't be -2.
    char *term = newStr(getenv("TERM"));
    if (term) unsetenv("TERM");
    Gpm_Open(&conn, 0);
    if (term) setenv("TERM", term, 1);
    delete[] term;

    if (gpm_fd != -1)
        return new GpmInput(displayBuf);
    return nullptr;
}

inline GpmInput::GpmInput(DisplayBuffer &aDisplayBuf) noexcept :
    InputAdapter(gpm_fd),
    displayBuf(aDisplayBuf)
{
}

GpmInput::~GpmInput()
{
    displayBuf.setCursorVisibility(false);
    Gpm_Close();
}

void GpmInput::fitEvent(Gpm_Event &gpmEvent) noexcept
{
    short &x = gpmEvent.x, &y = gpmEvent.y;
    x = (short) min(max(x, 0), displayBuf.size.x - 1);
    y = (short) min(max(y, 0), displayBuf.size.y - 1);
}

static constexpr struct { uchar gpm, mb; } gpmButtonFlags[] =
{
    {GPM_B_LEFT, mbLeftButton},
    {GPM_B_RIGHT, mbRightButton},
    {GPM_B_MIDDLE, mbMiddleButton},
};

bool GpmInput::getEvent(TEvent &ev) noexcept
{
    Gpm_Event gpmEvent;
    if (Gpm_GetEvent(&gpmEvent) == 1)
    {
        fitEvent(gpmEvent);
        displayBuf.setCursorPosition(gpmEvent.x, gpmEvent.y);
        // Do not show the cursor until we receive at least one mouse event.
        displayBuf.setCursorVisibility(true);

        ev.what = evMouse;
        ev.mouse.where.x = gpmEvent.x;
        ev.mouse.where.y = gpmEvent.y;

        for (const auto &flag : gpmButtonFlags)
            if (gpmEvent.buttons & flag.gpm)
            {
                if (gpmEvent.type & GPM_DOWN)
                    buttonState |= flag.mb;
                if (gpmEvent.type & GPM_UP)
                    buttonState &= ~flag.mb;
            }
        ev.mouse.buttons = buttonState;

        if (gpmEvent.wdy)
            ev.mouse.wheel = gpmEvent.wdy > 0 ? mwUp : mwDown;
        else if (gpmEvent.wdx)
            ev.mouse.wheel = gpmEvent.wdx > 0 ? mwRight : mwLeft;
        else
            ev.mouse.wheel = 0;

        ev.mouse.controlKeyState =
            LinuxConsoleInput::convertLinuxKeyModifiers(gpmEvent.modifiers);

        return true;
    }
    return false;
}

} // namespace tvision

#endif // HAVE_GPM

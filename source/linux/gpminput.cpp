#ifdef HAVE_GPM

#define Uses_TPoint
#define Uses_TEvent
#define Uses_TKeys
#define Uses_TScreen
#include <tvision/tv.h>

#include <internal/gpminput.h>
#include <internal/linuxcon.h>
#include <algorithm>
#include <gpm.h>

GpmInput::GpmInput() : cursor(ScreenCursor::Negative), buttonState(0)
{
    // Let coordinates begin at zero instead of one.
    gpm_zerobased = 1;
    Gpm_Connect conn = {
        .eventMask = GPM_DOWN | GPM_UP | GPM_DRAG | GPM_MOVE,
        .defaultMask = 0, // Disable cursor drawing by the server.
        /* Disable mouse event reporting when keyboard modifiers are active.
         * In such case, GPM text selection and copy/paste will be active. */
        .minMod = 0,
        .maxMod = 0 };
    Gpm_Open(&conn, 0);
    addListener(this, gpm_fd);
}

GpmInput::~GpmInput()
{
    /* gpm_fd is -1 if the connection failed or -2 if it's just translating
     * xterm events. It's greater or equal than 0 if a direct connection
     * succeeded. */
    if (0 <= gpm_fd)
    {
        Gpm_Close();
    }
}

int GpmInput::getButtonCount()
{
    return gpm_fd < 0 ? 0 : 2;
}

void GpmInput::fitEvent(Gpm_Event &gpmEvent)
{
    short &x = gpmEvent.x, &y = gpmEvent.y;
    x = std::clamp<short>(x, 0, TScreen::screenWidth - 1);
    y = std::clamp<short>(y, 0, TScreen::screenHeight - 1);
}


bool GpmInput::getEvent(TEvent &ev)
{
    Gpm_Event gpmEvent;
    if (Gpm_GetEvent(&gpmEvent) == 1)
    {
        fitEvent(gpmEvent);
        cursor.setPos({gpmEvent.x, gpmEvent.y});
        cursor.show();
        if (gpmEvent.type != GPM_MOVE || gpmEvent.dx || gpmEvent.dy || gpmEvent.wdy)
        {
            ev.what = evMouse;
            ev.mouse.where.x = gpmEvent.x;
            ev.mouse.where.y = gpmEvent.y;
            if ((gpmEvent.buttons & GPM_B_LEFT) && (gpmEvent.type & GPM_DOWN))
                buttonState |= mbLeftButton;
            if ((gpmEvent.buttons & GPM_B_LEFT) && (gpmEvent.type & GPM_UP))
                buttonState &= ~mbLeftButton;
            if ((gpmEvent.buttons & GPM_B_RIGHT) && (gpmEvent.type & GPM_DOWN))
                buttonState |= mbRightButton;
            if ((gpmEvent.buttons & GPM_B_RIGHT) && (gpmEvent.type & GPM_UP))
                buttonState &= ~mbRightButton;
            if ((gpmEvent.buttons & GPM_B_MIDDLE) && (gpmEvent.type & GPM_DOWN))
                buttonState |= mbMiddleButton;
            if ((gpmEvent.buttons & GPM_B_MIDDLE) && (gpmEvent.type & GPM_UP))
                buttonState &= ~mbMiddleButton;
            ev.mouse.buttons = buttonState;
            if ( gpmEvent.wdy )
                ev.mouse.wheel = gpmEvent.wdy > 0 ? mwUp : mwDown;
            else if ( gpmEvent.wdx )
                ev.mouse.wheel = gpmEvent.wdx > 0 ? mwRight : mwLeft;
            else
                ev.mouse.wheel = 0;
            return true;
        }
    }
    return false;
}

#endif // HAVE_GPM

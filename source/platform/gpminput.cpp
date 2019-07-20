#define Uses_TPoint
#define Uses_TEvent
#define Uses_TKeys
#include <tvision/tv.h>

#include <internal/gpminput.h>
#include <unordered_map>
using std::unordered_map;

#ifdef HAVE_GPM

#include <gpm.h>

GpmInput::GpmInput() : mousePos({-1, -1})
{
    // Let coordinates begin at zero instead of one.
    gpm_zerobased = 1;
    Gpm_Connect conn = {
        .eventMask = GPM_DOWN | GPM_UP | GPM_DRAG | GPM_MOVE,
        // I do not fully understand the point of this, but it works.
        .defaultMask = GPM_MOVE | GPM_HARD,
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

void GpmInput::drawPointer()
{
    // Do not draw the pointer unless we captured its position at least once.
    if (mousePos.x >= 0)
        Gpm_DrawPointer(mousePos.x, mousePos.y, 0);
}

bool GpmInput::getEvent(TEvent &ev)
{
    Gpm_Event gpmEvent;
    if (Gpm_GetEvent(&gpmEvent) == 1)
    {
        Gpm_FitEvent(&gpmEvent);
        mousePos = { gpmEvent.x, gpmEvent.y };
        drawPointer();
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

#else

#include <unistd.h>

GpmInput::GpmInput() {}
GpmInput::~GpmInput() {}
int GpmInput::getButtonCount() { return 0; }
void GpmInput::drawPointer() {}
bool GpmInput::getEvent(TEvent &ev) { while (true) pause(); }

#endif

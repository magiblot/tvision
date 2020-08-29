/*------------------------------------------------------------*/
/* filename -       popupmnu.cpp                              */
/*                                                            */
/* function(s)                                                */
/*          popupMenu                                         */
/*------------------------------------------------------------*/

#define Uses_TMenuPopup
#define Uses_TProgram
#define Uses_TDeskTop
#include <tvision/tv.h>

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  popupMenu                                                             */
/*                                                                        */
/*  Spawns and executes a TMenuPopup on the desktop.                      */
/*                                                                        */
/*  arguments:                                                            */
/*                                                                        */
/*      where   - Reference position, in absolute coordinates.            */
/*                The top left corner of the popup will be placed         */
/*                at (where.x, where.y+1).                                */
/*                                                                        */
/*      aMenu   - Chain of menu items. This function takes ownership      */
/*                over the items and the reference becomes dangling       */
/*                after the invocation.                                   */
/*                                                                        */
/*      receiver- If not null, an evCommand event is generated with       */
/*                the selected command in the menu and put into it        */
/*                with putEvent.                                          */
/*                                                                        */
/*  returns:                                                              */
/*                                                                        */
/*      The selected command, if any.                                     */
/*                                                                        */
/*------------------------------------------------------------------------*/

ushort popupMenu(TPoint where, TMenuItem &aMenu, TGroup *receiver)
{
    ushort res = 0;
    TGroup *deskTop = TProgram::deskTop;
    if (deskTop)
    {
        TPoint p = deskTop->makeLocal(where);
        TMenuPopup *m = new TMenuPopup(TRect(p, p), aMenu);
        // Initially, the menu is placed above 'p'. So we need to move it.
        TRect r = m->getBounds();
        // But we must ensure that the popup does not go beyond the desktop's
        // bottom right corner, for usability.
        TPoint d = deskTop->size - p;
        r.move(min(m->size.x, d.x),
               min(m->size.y + 1, d.y));
        m->setBounds(r);
        // Execute and dispose the menu.
        res = deskTop->execView(m);
        TObject::destroy(m);
        // Generate an event.
        if (res && receiver)
        {
            TEvent event = {};
            event.what = evCommand;
            event.message.command = res;
            receiver->putEvent(event);
        }
    }
    return res;
}

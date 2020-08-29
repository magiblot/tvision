/*------------------------------------------------------------*/
/* filename -       tmenupop.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TMenuPopup member functions               */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TEvent
#define Uses_TRect
#define Uses_TMenu
#define Uses_TMenuItem
#define Uses_TMenuPopup
#define Uses_TProgram
#define Uses_TDeskTop
#include <tvision/tv.h>

TMenuPopup::TMenuPopup(const TRect& bounds, TMenu* aMenu, TMenuView *aParentMenu) :
    TMenuBox( bounds, aMenu, aParentMenu )
{
    putClickEventOnExit = False;
}

TMenuPopup::TMenuPopup(const TRect& bounds, TMenuItem &aMenu, TMenuView *aParentMenu) :
    TMenuBox( bounds, new TMenu(aMenu), aParentMenu )
{
    putClickEventOnExit = False;
}

ushort TMenuPopup::execute()
{
    // Do not highlight the default entry, because it would look ugly.
    menu->deflt = 0;
    return TMenuBox::execute();
}

void TMenuPopup::handleEvent(TEvent& event)
{
    switch (event.what)
    {
    case evKeyDown:
        TMenuItem* p = findItem(getCtrlChar(event.keyDown.keyCode));
        if (!p)
            p = hotKey(event.keyDown.keyCode);
        if (p && commandEnabled(p->command))
        {
            event.what = evCommand;
            event.message.command = p->command;
            event.message.infoPtr = NULL;
            putEvent(event);
            clearEvent(event);
        }
        else
            if (getAltChar(event.keyDown.keyCode))
                clearEvent(event);
        break;
    }
    TMenuBox::handleEvent(event);
}

#if !defined(NO_STREAMABLE)

TStreamable *TMenuPopup::build()
{
    return new TMenuPopup( streamableInit );
}

TMenuPopup::TMenuPopup( StreamableInit ) : TMenuBox( streamableInit )
{
}

#endif

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

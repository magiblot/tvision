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
    closeOnBorderClick = True;
}

TMenuPopup::TMenuPopup(const TRect& bounds, TMenuItem &aMenu, TMenuView *aParentMenu) :
    TMenuBox( bounds, new TMenu(aMenu), aParentMenu )
{
    putClickEventOnExit = False;
    closeOnBorderClick = True;
}

ushort TMenuPopup::execute()
{
    // Do not highlight the default entry, because it would look ugly.
    menu->deflt = 0;
    ushort res = TMenuBox::execute();
    if (res)
    {
        TEvent event = {};
        event.what = evCommand;
        event.message.command = res;
        event.message.infoPtr = 0;
        putEvent(event);
    }
    return res;
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
/*      where   - The top left corner (in absolute coordinates)           */
/*                of the popup.                                           */
/*                                                                        */
/*      aMenu   - Chain of menu items.                                    */
/*                                                                        */
/*  returns:                                                              */
/*                                                                        */
/*      The selected command, if any. But keep in mind that TMenuPopup    */
/*      already generates an evCommand event with putEvent.               */
/*                                                                        */
/*------------------------------------------------------------------------*/

ushort popupMenu(TPoint where, TMenuItem &aMenu)
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
        r.move(min(m->size.x, d.x), min(m->size.y, d.y));
        m->setBounds(r);
        res = deskTop->execView(m);
        TObject::destroy(m);
    }
    return res;
}

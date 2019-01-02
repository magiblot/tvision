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

#define Uses_TRect
#define Uses_TMenu
#define Uses_TMenuItem
#define Uses_TMenuPopup
#include <tvision\tv.h>

TMenuPopup::TMenuPopup(TRect& bounds, TMenu* aMenu) :
    TMenuBox( bounds, aMenu )
{
}

TMenuPopup::handleEvent(Event& event)
{
    switch (event.what)
    {
    case evKeyDown:
        TMenuItem* p = findItem(getCtrlChar(event.keyCode));
        if (!p)
            p = hotKey(event.keyCode);
        if (p && commandEnabled(p->command))
        {
            event.what = evCommand;
            event.command = p->command;
            event.infoPtr = NULL;
            putEvent(event);
            clearEvent(event);
        }
        else
            if (getAltChar(event.keyCode))
                clearEvent(event);
        break;
    }
    TMenuBox::handleEvent(event);
}

/*------------------------------------------------------------*/
/* filename -       tvcursor.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TView resetCursor member function         */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TView
#define Uses_TGroup
#define Uses_TScreen
#define Uses_THardwareInfo
#include <tvision/tv.h>

#if !defined( __FLAT__ )
#error The 16-bit version of this file is in TVCURSOR.ASM
#else

static int decideCaretSize(TView *p)
{
    if (p->state & sfCursorIns)
        return 100;
    return TScreen::cursorLines & 0xFF;
}

static Boolean caretIsCoveredBySiblings(TView *p, int x, int y)
// Pre: 'p->owner != nullptr'
{
    TView *u = p->owner->last->next;
    for (; u != p; u = u->next)
        if ( (u->state & sfVisible)
             && (u->origin.y <= y && y < u->origin.y + u->size.y)
             && (u->origin.x <= x && x < u->origin.x + u->size.x) )
            return True;
    return False;
}

static int computeCaretSize(TView *p, int &x, int &y)
{
    ushort mask = sfVisible | sfCursorVis | sfFocused;
    if ((p->state & mask) != mask)
        return 0;

    TView *v = p;
    while (0 <= y && y < v->size.y && 0 <= x && x < v->size.x)
    {
        y += v->origin.y;
        x += v->origin.x;

        if (!v->owner)
            return decideCaretSize(p);

        if ( !(v->owner->state & sfVisible)
             || caretIsCoveredBySiblings(v, x, y) )
            break;

        v = v->owner;
    }
    return 0;
}

void TView::resetCursor()
{
    int x = cursor.x, y = cursor.y;
    int caretSize = computeCaretSize(this, x, y);
    if (caretSize)
        THardwareInfo::setCaretPosition(x, y);
    THardwareInfo::setCaretSize(caretSize);
}

#endif // __FLAT__

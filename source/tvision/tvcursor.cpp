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

#ifndef __BORLANDC__

#include <assert.h>

void TView::resetCursor()
{
    TView *v = this, *c;
    int caretSize;
    int x = cursor.x, y = cursor.y;
    if (~state & (sfVisible | sfCursorVis | sfFocused))
        goto L4;
L1:
    if (y < 0 || y >= v->size.y || x < 0 || x >= v->size.x)
        goto L4;
    y += v->origin.y;
    x += v->origin.x;
    c = v;
    if (v->owner == 0)
        goto L5;
    if (!(v->owner->state & sfVisible))
        goto L4;
    v = v->owner->last;
L2:
    v = v->next;
    if (v != c)
        goto L3;
    v = v->owner;
    goto L1;
L3:
    if (!(v->state & sfVisible))
        goto L2;
    if (y < v->origin.y || y >= v->origin.y + v->size.y)
        goto L2;
    if (x < v->origin.x || x >= v->origin.x + v->size.x)
        goto L2;
L4:
    // Cursor is not visible if we get here.
    caretSize = 0;
    goto L6;
L5:
    // Cursor is visible, so let's set its position.
    THardwareInfo::setCaretPosition(x, y);
    // Determine cursor size.
    caretSize = TScreen::cursorLines & 0x0F;
    if (!(state & sfCursorIns))
        goto L6;
    caretSize = 100;
L6:
    THardwareInfo::setCaretSize(caretSize);
}

#endif

/*------------------------------------------------------------*/
/* filename - tgrmv.cpp                                       */
/*                                                            */
/* function(s)                                                */
/*                     TGroup removeView member function      */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TGroup
#define Uses_TView
#include <tvision/tv.h>

void TGroup::removeView( TView *p )
{
    TView *b, *s;
    if (last == 0)
        goto L7;
    s = last;
L1:
    b = s->next;
    goto L5;
L3:
    s = b;
    goto L1;
L4:
    if (last != b)
        goto L3;
    goto L7;
L5:
    if (p != b)
        goto L4;
    s->next = p->next;
    if (p != last)
        goto L7;
    if (p != p->next)
        goto L6;
    s = 0;
L6:
    last = s;
L7:
    ;
}


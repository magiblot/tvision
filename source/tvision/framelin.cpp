/*------------------------------------------------------------*/
/* filename -  framelin.cpp                                   */
/*                                                            */
/* function(s)                                                */
/*             TFrame frameLine member function               */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TFrame
#define Uses_TGroup
#include <tvision/tv.h>

#ifndef __BORLANDC__

#include <assert.h>

void TFrame::frameLine( TDrawBuffer& frameBuf, short y, short n, uchar color )
{
    uchar *FrameMask = new uchar[size.x];
    FrameMask[0] = initFrame[n];
    for (int i = 1; i < size.x - 1; ++i)
        FrameMask[i] = initFrame[n + 1];
    FrameMask[size.x - 1] = initFrame[n + 2];
    TView* v = owner->last;
    int eax, esi, edi;
    uchar al, ah;
L1:
    v = v->next;
    if (v == (TView *) this)
        goto L10;
L2:
    if (!(v->options & ofFramed) || !(v->state & sfVisible))
        goto L1;
    eax = y - v->origin.y;
    if (eax < 0)
        goto L3;
    if (eax > v->size.y)
        goto L1;
    else if (eax < v->size.y)
        eax = 0x0005;
    else
        eax = 0x0A03;
    goto L4;
L3:
    if (eax + 1 != 0)
        goto L1;
    eax = 0x0A06;
L4:
    esi = v->origin.x;
    edi = v->origin.x + v->size.x;
    if (esi > 1)
        goto L5;
    esi = 1;
L5:
    if (edi < size.x - 1)
        goto L6;
    edi = size.x - 1;
L6:
    if (esi >= edi)
        goto L1;
    al = eax & 0x00FF;
    ah = (eax & 0xFF00) >> 8;
    FrameMask[esi - 1] |= al;
    FrameMask[edi] |= al ^ ah;
    if (ah == 0)
        goto L1;
L8:
    for (int i = esi; i < edi; ++i)
        FrameMask[i] |= ah;
    goto L1;
L10:
    for (int i = 0; i < size.x; ++i) {
        frameBuf.putChar(i, frameChars[FrameMask[i]]);
        frameBuf.putAttribute(i, color);
    }
    delete[] FrameMask;
}

#endif

/*------------------------------------------------------------*/
/* filename -       tvexposd.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TView exposed member function             */
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
#include <tvision/tv.h>

struct TVExposd {

    int eax, ebx, ecx, esi;
    TView *target;

    TVExposd();

    Boolean L0( TView* );
    Boolean L1( TView* );
    Boolean L10( TView* );
    Boolean L11( TView* );
    Boolean L12( TGroup* );
    Boolean L13( TGroup* );
    Boolean L20( TView* );
    Boolean L21( TView* );
    Boolean L22( TView* );
    Boolean L23( TView* );

};

Boolean TView::exposed()
{
    return TVExposd().L0(this);
}

TVExposd::TVExposd() :
    eax(0), ebx(0), ecx(0), esi(0), target(0)
{
}

Boolean TVExposd::L0( TView *dest )
{
    if (!(dest->state & sfExposed))
        return False;
    if (0 >= dest->size.x || 0 >= dest->size.y)
        return False;
    return L1(dest);
}

Boolean TVExposd::L1( TView *dest )
{
    int i = 0;
    do {
        eax = i;
        ebx = 0;
        ecx = dest->size.x;
        if (!L11(dest))
            return True;
        ++i;
    } while (i < dest->size.y);
    return False;
}

Boolean TVExposd::L10( TView *dest )
{
    TGroup *owner = dest->owner;
    if (owner->buffer != 0 || owner->lockFlag != 0)
        return False;
    return L11(owner);
}

Boolean TVExposd::L11( TView *dest )
{
    target = dest;
    eax += dest->origin.y;
    ebx += dest->origin.x;
    ecx += dest->origin.x;
    TGroup *owner = dest->owner;
    if (!owner)
        return False;
    if (eax < owner->clip.a.y)
        return True;
    if (eax >= owner->clip.b.y)
        return True;
    if (ebx >= owner->clip.a.x)
        return L12(owner);
    ebx = owner->clip.a.x;
    return L12(owner);
}

Boolean TVExposd::L12( TGroup *owner )
{
    if (ecx <= owner->clip.b.x)
        return L13(owner);
    ecx = owner->clip.b.x;
    return L13(owner);
}

Boolean TVExposd::L13( TGroup *owner )
{
    if (ebx >= ecx)
        return True;
    return L20(owner->last);
}

Boolean TVExposd::L20( TView *dest )
{
    TView *next = dest->next;
    if (next == target)
        return L10(next);
    return L21(next);
}

Boolean TVExposd::L21( TView *next )
{
    if (!(next->state & sfVisible))
        return L20(next);
    esi = next->origin.y;
    if (eax < esi)
        return L20(next);
    esi += next->size.y;
    if (eax >= esi)
        return L20(next);
    esi = next->origin.x;
    if (ebx < esi)
        return L22(next);
    esi += next->size.x;
    if (ebx >= esi)
        return L20(next);
    ebx = esi;
    if (ebx < ecx)
        return L20(next);
    return True;
}

Boolean TVExposd::L22( TView *next )
{
    if (ecx <= esi)
        return L20(next);
    esi += next->size.x;
    if (ecx > esi)
        return L23(next);
    ecx = next->origin.x;
    return L20(next);
}

Boolean TVExposd::L23( TView *next )
{
    TView *_target = target;
    int _esi = esi, _ecx = ecx, _eax = eax;
    ecx = next->origin.x;
    Boolean b = L20(next);
    eax = _eax; ecx = _ecx; ebx = _esi;
    target = _target;
    if (b)
        return L20(next);
    return False;
}

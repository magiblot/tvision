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

#ifdef DEBUG_TVEXPOSD

#include <internal/debugAsm.h>

static int timesRun = 0;
static int iterations;
static int iterationsAsm;

#define CtrlRet(a,b) { pushArrayTrace(a); return b; }

#else

#define CtrlRet(a,b) { return b; }

#endif

static Boolean L0( TView* );

Boolean TView::exposed()
{
#ifdef DEBUG_TVEXPOSD
    startArrayTrace();
#endif
    Boolean b = L0(this);
#ifdef DEBUG_TVEXPOSD
    endArrayTrace();
    Boolean c = exposedAsm(traceAsm, &iterationsAsm);
    if (b != c) {
        cerr << ' ' << timesRun << ": "
             << "CPP = " << b << " (" << iterations << "), "
             << "ASM = " << c << " (" << iterationsAsm << ") " << endl;
        cerr << "CPP: "; printTrace();
        cerr << "ASM: "; printTraceAsm();
    }
    timesRun++;
#endif
    return b;
}

static Boolean L1( TView* );
static Boolean L10( TView* );
static Boolean L11( TView* );
static Boolean L12( TGroup* );
static Boolean L13( TGroup* );
static Boolean L20( TView* );
static Boolean L21( TView* );
static Boolean L22( TView* );
static Boolean L23( TView* );

static int eax = 0, ebx = 0, ecx = 0, esi = 0;
static TView *target = 0;

Boolean L0( TView *dest )
{
    if (!(dest->state & sfExposed))
        CtrlRet("L0.1", False);
    if (0 >= dest->size.x || 0 >= dest->size.y)
        CtrlRet("L0.2", False);
    CtrlRet("L0.3", L1(dest));
}

Boolean L1( TView *dest )
{
    int i = 0;
    do {
#ifdef DEBUG_TVEXPOSD
        int _z = traceIndex;
        pushArrayTrace("L1.1");
#endif
        eax = i;
        ebx = 0;
        ecx = dest->size.x;
        if (!L11(dest))
            return True;
        ++i;
#ifdef DEBUG_TVEXPOSD
        traceIndex = _z;
        iterations = i;
#endif
    } while (i < dest->size.y);
    CtrlRet("L1.2", False);
}

Boolean L10( TView *dest )
{
    TGroup *owner = dest->owner;
    if (owner->buffer != 0 || owner->lockFlag != 0)
        CtrlRet("L10.1", False);
    CtrlRet("L10.2", L11(owner));
}

Boolean L11( TView *dest )
{
    target = dest;
    eax += dest->origin.y;
    ebx += dest->origin.x;
    ecx += dest->origin.x;
    TGroup *owner = dest->owner;
    if (!owner)
        CtrlRet("L11.1", False);
    if (eax < owner->clip.a.y)
        CtrlRet("L11.2", True);
    if (eax >= owner->clip.b.y)
        CtrlRet("L11.3", True);
    if (ebx >= owner->clip.a.x)
        CtrlRet("L11.4", L12(owner));
    ebx = owner->clip.a.x;
    CtrlRet("L11.5", L12(owner));
}

Boolean L12( TGroup *owner )
{
    if (ecx <= owner->clip.b.x)
        CtrlRet("L12.1", L13(owner));
    ecx = owner->clip.b.x;
    CtrlRet("L12.2", L13(owner));
}


Boolean L13( TGroup *owner )
{
    if (ebx >= ecx)
        CtrlRet("L13.1", True);
    CtrlRet("L13.2", L20(owner->last));
}

Boolean L20( TView *dest )
{
    TView *next = dest->next;
    if (next == target)
        CtrlRet("L20.1", L10(next));
    CtrlRet("L20.2", L21(next));
}

Boolean L21( TView *next )
{
    if (!(next->state & sfVisible))
        CtrlRet("L21.1", L20(next));
    esi = next->origin.y;
    if (eax < esi)
        CtrlRet("L21.2", L20(next));
    esi += next->size.y;
    if (eax >= esi)
        CtrlRet("L21.3", L20(next));
    esi = next->origin.x;
    if (ebx < esi)
        CtrlRet("L21.4", L22(next));
    esi += next->size.x;
    if (ebx >= esi)
        CtrlRet("L21.4", L20(next));
    ebx = esi;
    if (ebx < ecx)
        CtrlRet("L21.5", L20(next));
    CtrlRet("L21.6", True);
}

Boolean L22( TView *next )
{
    if (ecx <= esi)
        CtrlRet("L22.1", L20(next));
    esi += next->size.x;
    if (ecx > esi)
        CtrlRet("L22.2", L23(next));
    ecx = next->origin.x;
    CtrlRet("L22.3", L20(next));
}

Boolean L23( TView *next )
{
    TView *_target = target;
    int _esi = esi, _ecx = ecx, _eax = eax;
    ecx = next->origin.x;
    Boolean b = L20(next);
    eax = _eax; ecx = _ecx; ebx = _esi;
    target = _target;
    if (b)
        CtrlRet("L23.1", L20(next));
    CtrlRet("L23.2", False);
}

#if defined( DEBUG_TVEXPOSD ) && defined (__FLAT__)

Boolean TView::exposedAsm( void * _v, int * _i)
{
    ((char *) _v)[0] = *_i = -1;
    return -1;
}

#endif

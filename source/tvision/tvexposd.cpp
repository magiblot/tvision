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

#include <stdio.h>

int timesRun = 0;

const char* trace[100];
char traceAsm[100];

int z = 0;
int ii;

#define CtrlRet(a,b) { trace[z++] = a; return b; }

void printTrace( const char** );
void printTraceAsm( const char* );
void keepCursorBetween(int, int);

#else

#define CtrlRet(a,b) { return b; }

#endif

int i;

Boolean L0( TView* );

Boolean TView::exposed()
{
    Boolean b = L0(this);
#ifdef DEBUG_TVEXPOSD
    trace[z] = 0;
    Boolean c = exposedAsm(traceAsm, &ii);
    if (b != c) {
        keepCursorBetween(7, 22);
        printf(" %d: CPP = %d (%d), ASM = %d (%d) \n", timesRun, b, i - 1, c, ii);
        printf("CPP: "); printTrace(trace);
        printf("ASM: "); printTraceAsm(traceAsm);
    }
    timesRun++;
#endif
    return b;
}

Boolean L1( TView* );
Boolean L10( TView* );
Boolean L11( TView* );
Boolean L12( TGroup* );
Boolean L13( TGroup* );
Boolean L20( TView* );
Boolean L21( TView* );
Boolean L22( TView* );
Boolean L23( TView* );

int eax = 0, ebx = 0, ecx = 0, esi = 0;
TView *target = 0;

Boolean L0( TView *dest )
{
#ifdef DEBUG_TVEXPOSD
    z = 0;
#endif
    if (!(dest->state & sfExposed))
        CtrlRet("L0.1", False);
    if (0 >= dest->size.x || 0 >= dest->size.y)
        CtrlRet("L0.2", False);
    CtrlRet("L0.3", L1(dest));
}

Boolean L1( TView *dest )
{
    i = 0;
    do {
#ifdef DEBUG_TVEXPOSD
        int _z = z;
        trace[z++] = "L1.1";
#endif
        eax = i;
        ebx = 0;
        ecx = dest->size.x;
        if (!L11(dest))
            return True;
        ++i;
#ifdef DEBUG_TVEXPOSD
        z = _z;
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

#ifdef DEBUG_TVEXPOSD

void printTrace(const char** tr)
{
    z = 0;
    while (tr[z] != 0)
    {
        printf("%s ", tr[z]);
        if (++z % 7 == 0)
            printf("\n");
    }
    if (z % 7)
        printf("\n");
}

void printTraceAsm(const char* tr)
{
    z = 0;
    while (tr[z] != -1)
    {
        printf("L%d ", (int) tr[z]);
        if (++z % 10 == 0)
            printf("\n");
    }
    if (z % 10)
        printf("\n");
}

void keepCursorBetween(int firstLine, int lastLine)
{
#ifdef __BORLANDC__
    _AH = 0x0F;
I   INT   0x10;
    _AH = 0x03;
I   INT   0x10;
    if (_DH < firstLine || lastLine <= _DH) {
        _AH = 0x02;
        _DH = firstLine;
I       INT   0x10;
    }
#endif
}

#endif

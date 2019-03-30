/*------------------------------------------------------------*/
/* filename -       tvwrite.cpp                               */
/*                                                            */
/* function(s)                                                */
/*                  TView write member functions              */
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
#define Uses_TEventQueue
#include <tvision/tv.h>

#include <string.h>

#ifdef DEBUG_TVWRITE

#include <debugAsm.h>
#define Call(a,b) { pushTrace(a); b; }

#else

#define Call(a,b) { b; }

#endif

#define Jmp(a,b) { Call(a,b); return; }
#define Ret(a) { Jmp(a,) }

extern TPoint shadowSize;
extern uchar shadowAttr;

static short X, Y, Count, wOffset;
static const void _FAR *Buffer;
static TView *Self, *Target;

static int edx, esi;

static void L0();
static void L10( TView * );
static void L20( TView * );
static void L30( TView * );
static void L40( TView * );
static void L50( TGroup * );
static void L60( const ushort *, ushort * );

void TView::writeView( short x, short y, short count, const void _FAR* b )
{
#ifdef DEBUG_TVWRITE
    startTrace();
#endif
    Self = this;
    X = x; Y = y; Count = count; Buffer = b;
    L0();
#ifdef DEBUG_TVWRITE
    endTrace();
	// cout << "X: " << X << ", Y: " << Y << ", count: " << Count << endl;
    // printf("X: %d, Y: %d, count: %d\n", X, Y, Count);
    // printf("CPP: "); printTrace();
#endif
}

void L0()
{
    wOffset = X;
    Count += X;
    edx = 0;
    if (Y < 0)
        Ret("L0.1");
    if (Y >= Self->size.y)
        Ret("L0.2");
    if (X >= 0)
        Jmp("L0.3", goto L1);
    X = 0;
L1:
    if (Count <= Self->size.x)
        Jmp("L1.1", goto L2);
    Count = Self->size.x;
L2:
    if (X < Count)
        Jmp("L2.1", L10(Self));
    Ret("L2.2");
}

void L10( TView *dest )
{
    TGroup *owner = dest->owner;
    if (!(dest->state & sfVisible) || owner == 0)
        Ret("L10.1");
    Target = dest;
    Y += dest->origin.y;
    X += dest->origin.x;
    Count += dest->origin.x;
    wOffset += dest->origin.x;
    if (Y < owner->clip.a.y || Y >= owner->clip.b.y)
        Ret("L10.2");
    if (X >= owner->clip.a.x)
        Jmp("L10.3", goto L11);
    X = owner->clip.a.x;
L11:
    if (Count <= owner->clip.b.x)
        Jmp("L11.1", goto L12);
    Count = owner->clip.b.x;
L12:
    if (X >= Count)
        Ret("L12.1");
    Jmp("L12.2", L20(owner->last));
}

void L20( TView *dest )
{
    TView *next = dest->next;
    if (next == Target)
        Jmp("L20.1", L40(next));
L21:
    if (!(next->state & sfVisible))
        Jmp("L21.1", L20(next));
    esi = next->origin.y;
    if (Y < esi)
        Jmp("L21.2", L20(next));
    esi += next->size.y;
    if (Y < esi)
        Jmp("L21.3", goto L23);
    if (!(next->state & sfShadow))
        Jmp("L21.4", L20(next));
    esi += shadowSize.y;
    if (Y >= esi)
        Jmp("L21.5", L20(next));
    esi = next->origin.x + shadowSize.x;
    if (X >= esi)
        Jmp("L21.6", goto L22);
    if (Count <= esi)
        Jmp("L21.7", L20(next));
    Call("L21.8", L30(next));
L22:
    esi += next->size.x;
    Jmp("L22.1", goto L26);
L23:
    esi = next->origin.x;
    if (X >= esi)
        Jmp("L23.1", goto L24);
    if (Count <= esi)
        Jmp("L23.2", L20(next));
    Call("L23.3", L30(next));
L24:
    esi += next->size.x;
    if (X >= esi)
        Jmp("L24.1", goto L25);
    if (Count <= esi)
        Ret("L24.2");
    X = esi;
L25:
    if (!(next->state & sfShadow))
        Jmp("L25.1", L20(next));
    if (Y < next->origin.y + shadowSize.y)
        Jmp("L25.2", L20(next));
    esi += shadowSize.x;
L26:
    if (X >= esi)
        Jmp("L26.1", L20(next));
    edx++;
    if (Count <= esi)
        Jmp("L26.2", L20(next));
    Call("L26.3", L30(next));
    edx--;
    Jmp("L26.4", L20(next));
}

void L30( TView *dest )
{
    TView *_Target = Target;
    int _wOffset = wOffset, _esi = esi, _edx = edx,
        _count = Count, _y = Y;
    Count = esi;
    Call("L30.1", L20(dest));
    Y = _y; Count = _count; edx = _edx; esi = _esi;
    wOffset = _wOffset; Target = _Target;
    X = esi;
    Ret("L30.2");
}

void L40( TView *dest )
{
    TGroup *owner = dest->owner;
    if (owner->buffer == 0)
        Jmp("L40.1", goto L44);
    if (owner->buffer == TScreen::screenBuffer)
        Jmp("L40.2", goto L41);
    Call("L40.3", L50(owner));
    Jmp("L40.4", goto L44);
L41:
#ifndef __FLAT__
I   CLI
    if (Y != TEventQueue::curMouse.where.y)
        Jmp("L41.1", goto L42);
    if ((unsigned int) X > (unsigned int) TEventQueue::curMouse.where.x)
        Jmp("L41.2", goto L42);
    if ((unsigned int) Count > (unsigned int) TEventQueue::curMouse.where.x)
        Jmp("L41.2", goto L43);
L42:
    TEventQueue::mouseIntFlag = False;
I   STI
    Call("L42.1", L50(owner));
    if (!TEventQueue::mouseIntFlag)
        Jmp("L42.2", goto L44);
L43:
I   STI
#endif
    Call("L41.1", THWMouse::hide());
    Call("L41.2", L50(owner));
    Call("L41.3", THWMouse::show());
L44:
    if (owner->lockFlag != 0)
        Ret("L44.1");
    Jmp("L44.2", L10(owner));
}

void L50( TGroup *owner )
{
    int i;
#ifndef __FLAT__
    ushort *dst = &owner->buffer[Y*owner->size.x + X];
    ushort al = 0;
    if (owner->buffer != TScreen::screenBuffer)
        Jmp("L50.1", goto L51);
    al = TScreen::checkSnow;
L51:
    const ushort *src = &((const ushort *) Buffer)[X - wOffset];
    if (al != 0)
        Jmp("L51.1", L60(src, dst));
    if (edx != 0)
        Jmp("L51.2", goto L52);
    for (i = 0; i < Count - X; ++i)
    {
        dst[i] = src[i];
    }
    Ret("L51.3");
L52:
    for (i = 0; i < Count - X; ++i)
    {
        dst[i] = (shadowAttr << 8) | ((const uchar *)src)[2*i];
    }
    Ret("L52.1");
#else
    ushort *dst = &owner->buffer[Y*owner->size.x + X];
    const ushort *src = &((const ushort *) Buffer)[X - wOffset];
    if (owner->buffer != TScreen::screenBuffer)
        Jmp("L50.1", L60(src, dst));
    if (edx != 0)
        Jmp("L50.2", goto L52);
L51:
// Expand character/attribute pair
    for (i = 0; i < 2*(Count - X); ++i)
    {
        dst[i] = ((const uchar *)src)[i];
    }
    Jmp("L51.1", goto L54);
L52:
// Mix in shadow attribute
    for (i = 0; i < 2*(Count - X); i += 2)
    {
        dst[i] = ((const uchar *)src)[i];
        dst[i + 1] = shadowAttr;
    }
L54:
    Call("L54.1", THardwareInfo::screenWrite(X, Y, dst, Count - X));
    Ret("L54.2");
#endif
}

void L60(const ushort *src, ushort *dst)
{
    int i;
#ifndef __FLAT__
    if (edx != 0)
        Jmp("L60.1", goto L65);
L61:
    for (i = 0; i < Count - X; ++i)
    {
        ushort load = src[i];
I       MOV     DX, 0x03DA
L62:
I       IN      AL, DX
I       TEST    AL, 1
I       JNE     L62
I       CLI
L63:
I       IN      AL, DX
I       TEST    AL, 1
I       JE      L63
        dst[i] = load;
I       STI
    }
    Ret("L63.1");
L65:
    for (i = 0; i < Count - X; ++i)
    {
        uchar load = ((const uchar *)src)[2*i];
I       MOV     DX, 0x03DA
L66:
I       IN      AL, DX
I       TEST    AL, 1
I       JNE     L66
I       CLI
L67:
I       IN      AL, DX
I       TEST    AL, 1
I       JE      L67
        dst[i] = (shadowAttr << 8) | load;
I       STI
    }
    Ret("L67.1");
#else
    if (edx != 0)
        Jmp("L60.1", goto L61);
    for (i = 0; i < Count - X; ++i)
    {
        dst[i] = src[i];
    }
    edx = 0;
    Ret("L60.2");
L61:
    for (i = 0; i < Count - X; ++i)
    {
        dst[i] = (shadowAttr << 8) | ((const uchar *)src)[2*i];
    }
    Ret("L61.1");
#endif
}

void TView::writeBuf( short x, short y, short w, short h, const void _FAR* b )
{
    while (h-- > 0)
    {
        writeView(x, y++, w, b);
        b = ((const ushort *) b) + w;
    }
}

void TView::writeChar( short x, short y, char c, uchar color, short count )
{
    if (count > 0)
    {
        count = min(count, 256);
        ushort* attributed = new ushort[count];
        ushort attrcolor = (mapColor(color) << 8) | (uchar) c;
        for (int i = 0; i < count; ++i)
        {
            attributed[i] = attrcolor;
        }
        writeView(x, y, count, attributed);
        delete[] attributed;
    }
}

void TView::writeLine( short x, short y, short w, short h, const void _FAR *b )
{
    while (h-- > 0)
    {
        writeView(x, y++, w, b);
    }
}

void TView::writeStr( short x, short y, const char *str, uchar color )
{
    if (str != 0)
    {
        size_t length = strlen(str);
        if (length > 0)
        {
            ushort *attributed = new ushort[length];
            uchar attr = mapColor(color);
            for (int i = 0; i < length; ++i)
            {
        // The lower byte must be unsigned for concatenation to always work.
                attributed[i] = (attr << 8) | (uchar) str[i];
            }
            writeView(x, y, length, attributed);
            delete[] attributed;
        }
    }
}

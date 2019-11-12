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

#ifdef __FLAT__

#include <string.h>

extern TPoint shadowSize;
extern uchar shadowAttr;

static short X, Y, Count, wOffset;
static const void _FAR *Buffer;
static TView *Target;

static int edx, esi;

static void L10( TView * );
static void L20( TView * );
static void L30( TView * );
static void L40( TView * );
static void L50( TGroup * );

void TView::writeView( short x, short y, short count, const void _FAR* b )
{
    X = x; Y = y; Count = count; Buffer = b;
    wOffset = X;
    Count += X;
    edx = 0;
    if (0 <= Y && Y < size.y)
    {
        if (X < 0)
            X = 0;
        if (Count > size.x)
            Count = size.x;
        if (X < Count)
            L10(this);
    }
}

void L10( TView *dest )
{
    TGroup *owner = dest->owner;
    if ((dest->state & sfVisible) && owner)
    {
        Target = dest;
        Y += dest->origin.y;
        X += dest->origin.x;
        Count += dest->origin.x;
        wOffset += dest->origin.x;
        if (owner->clip.a.y <= Y && Y < owner->clip.b.y)
        {
            if (X < owner->clip.a.x)
                X = owner->clip.a.x;
            if (Count > owner->clip.b.x)
                Count = owner->clip.b.x;
            if (X < Count)
                L20(owner->last);
        }
    }
}

void L20( TView *dest )
{
    TView *next = dest->next;
    if (next == Target)
        L40(next);
    else
    {
        if ((next->state & sfVisible) && next->origin.y <= Y)
        {
            esi = next->origin.y + next->size.y;
            if (Y < esi)
            {
                esi = next->origin.x;
                if (X < esi)
                {
                    if (Count > esi)
                        L30(next);
                    else goto L20next;
                }
                esi += next->size.x;
                if (X < esi)
                {
                    if (Count > esi)
                        X = esi;
                    else return;
                }
                if ((next->state & sfShadow) && next->origin.y + shadowSize.y <= Y)
                    esi += shadowSize.x;
                else goto L20next;
            }
            else if ((next->state & sfShadow) && Y < esi + shadowSize.y)
            {
                esi = next->origin.x + shadowSize.x;
                if (X < esi)
                {
                    if (Count > esi)
                        L30(next);
                    else goto L20next;
                }
                esi += next->size.x;
            }
            else goto L20next;
            if (X < esi)
            {
                edx++;
                if (Count > esi)
                {
                    L30(next);
                    edx--;
                }
            }
        }
L20next:
        L20(next);
    }
}

void L30( TView *dest )
{
    TView *_Target = Target;
    int _wOffset = wOffset, _esi = esi, _edx = edx,
        _count = Count, _y = Y;
    Count = esi;

    L20(dest);

    Y = _y; Count = _count; edx = _edx; esi = _esi;
    wOffset = _wOffset; Target = _Target;
    X = esi;
}

void L40( TView *dest )
{
    TGroup *owner = dest->owner;
    if (owner->buffer)
    {
        if (owner->buffer != TScreen::screenBuffer)
            L50(owner);
        else
        {
            THWMouse::hide();
            L50(owner);
            THWMouse::show();
        }
    }
    if (owner->lockFlag == 0)
        L10(owner);
}

#define loByte(w)    (((uchar *)&w)[0])
#define hiByte(w)    (((uchar *)&w)[1])

void L50( TGroup *owner )
{
    int i;
    ushort *dst = &owner->buffer[Y*owner->size.x + X];
    const ushort *src = &((const ushort *) Buffer)[X - wOffset];
    if (owner->buffer != TScreen::screenBuffer)
    {
        if (edx == 0)
            memmove(dst, src, 2*(Count - X));
        else
        {
            ushort ColorChar;
            hiByte(ColorChar) = shadowAttr;
            for (i = 0; i < Count - X; ++i)
            {
                loByte(ColorChar) = src[i];
                dst[i] = ColorChar;
            }
        }
    }
    else
    {
        if (edx == 0)
            // Expand character/attribute pair
            for (i = 0; i < 2*(Count - X); ++i)
            {
                dst[i] = ((const uchar *) src)[i];
            }
        else
            // Mix in shadow attribute
            for (i = 0; i < 2*(Count - X); i += 2)
            {
                dst[i] = ((const uchar *) src)[i];
                dst[i + 1] = shadowAttr;
            }
        THardwareInfo::screenWrite(X, Y, dst, Count - X);
    }
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
        int length = strlen(str);
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

#endif

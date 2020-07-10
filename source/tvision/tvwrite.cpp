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
#include <malloc.h>

extern TPoint shadowSize;
extern uchar shadowAttr;

struct TVWrite {

    short X, Y, Count, wOffset;
    const void _FAR *Buffer;
    TView *Target;
    int edx, esi;

    void L0( TView *, short, short, short, const void _FAR* );
    void L10( TView * );
    void L20( TView * );
    void L30( TView * );
    void L40( TView * );
    void L50( TGroup * );

};

void TView::writeView( short x, short y, short count, const void _FAR* b )
{
    TVWrite().L0(this, x, y, count, b);
}

void TVWrite::L0( TView *dest, short x, short y, short count, const void _FAR* b )
{
    X = x; Y = y; Count = count; Buffer = b;
    wOffset = X;
    Count += X;
    edx = 0;
    if (0 <= Y && Y < dest->size.y)
    {
        if (X < 0)
            X = 0;
        if (Count > dest->size.x)
            Count = dest->size.x;
        if (X < Count)
            L10(dest);
    }
}

void TVWrite::L10( TView *dest )
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

void TVWrite::L20( TView *dest )
{
    TView *next = dest->next;
    if (next == Target)
        L40(next);
    else
    {
        if ((next->state & sfVisible) && next->origin.y <= Y)
        do {
            esi = next->origin.y + next->size.y;
            if (Y < esi)
            {
                esi = next->origin.x;
                if (X < esi)
                {
                    if (Count > esi)
                        L30(next);
                    else break;
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
                else break;
            }
            else if ((next->state & sfShadow) && Y < esi + shadowSize.y)
            {
                esi = next->origin.x + shadowSize.x;
                if (X < esi)
                {
                    if (Count > esi)
                        L30(next);
                    else break;
                }
                esi += next->size.x;
            }
            else break;
            if (X < esi)
            {
                edx++;
                if (Count > esi)
                {
                    L30(next);
                    edx--;
                }
            }
        } while (0);
        L20(next);
    }
}

void TVWrite::L30( TView *dest )
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

void TVWrite::L40( TView *dest )
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

void TVWrite::L50( TGroup *owner )
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
#define loByte(w)    (((uchar *)&w)[0])
#define hiByte(w)    (((uchar *)&w)[1])
            ushort ColorChar;
            hiByte(ColorChar) = shadowAttr;
            for (i = 0; i < Count - X; ++i)
            {
                loByte(ColorChar) = src[i];
                dst[i] = ColorChar;
            }
#undef loByte
#undef hiByte
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
        ushort attrcolor = (mapColor(color) << 8) | (uchar) c;
        ushort* attributed = (ushort*) alloca(sizeof(ushort)*count);
        for (int i = 0; i < count; ++i)
        {
            attributed[i] = attrcolor;
        }
        writeView(x, y, count, attributed);
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
            uchar attr = mapColor(color);
            ushort *attributed = (ushort*) alloca(sizeof(ushort)*length);
            for (int i = 0; i < length; ++i)
            {
        // The lower byte must be unsigned for concatenation to always work.
                attributed[i] = (attr << 8) | (uchar) str[i];
            }
            writeView(x, y, length, attributed);
        }
    }
}

#endif

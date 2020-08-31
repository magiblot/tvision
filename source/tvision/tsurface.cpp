/*------------------------------------------------------------*/
/* filename -       tsurface.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  Member function(s) of following classes   */
/*                      TDrawSurface                          */
/*                      TSurfaceView                          */
/*------------------------------------------------------------*/

#define Uses_TDrawSurface
#define Uses_TSurfaceView
#define Uses_TDrawBuffer
#define Uses_TRect
#include <tvision/tv.h>

#include <stdlib.h>

TDrawSurface::TDrawSurface() :
    dataLength(0),
    data(0),
    fill(0)
{
    size.x = size.y = 0;
}

TDrawSurface::TDrawSurface(TPoint aSize) :
    dataLength(0),
    data(0),
    fill(0)
{
    resize(aSize);
}

TDrawSurface::~TDrawSurface()
{
    ::free(data);
}

void TDrawSurface::resize(TPoint aSize)
{
    if (aSize.x > 0 && aSize.y > 0) {
        size_t newLength = aSize.x*aSize.y;
        void _FAR *newData = ::realloc(data, newLength*sizeof(TScreenCell));
        if (newData) {
            data = (TScreenCell _FAR *) newData;
            for (size_t i = dataLength; i < newLength; ++i)
                data[i] = fill;
            dataLength = newLength;
            size = aSize;
        }
    } else {
        ::free(data);
        data = 0;
        dataLength = 0;
        size.x = size.y = 0;
    }
}

void TDrawSurface::clear()
{
    for (size_t i = 0; i < dataLength; ++i)
        data[i] = fill;
}

TSurfaceView::TSurfaceView( const TRect &bounds,
                            const TDrawSurface _FAR *aSface ) :
    TView(bounds),
    sface(aSface)
{
    delta.x = delta.y = 0;
}

void TSurfaceView::draw()
{
    if (sface && size.x > 0) {
        // Cache these in local variables.
        const TPoint d = delta;
        const TPoint ssize = sface->size;
        const TPoint size = this->size;
        const TScreenCell *data = &sface->at(max(d.y, 0), max(d.x, 0));
        const TRect extent = TRect(0, 0, size.x, size.y);
        // This is the rectangle within the current view's extent where the
        // surface is to be drawn.
        const TRect clip = TRect(0, 0, ssize.x, ssize.y)
                            .move(-d.x, -d.y)
                            .intersect(extent);
        int y;
        if (clip == extent) {
            // Surface fills all of the view's extent. Can perform direct copy.
            for (y = 0; y < size.y; ++y, data += ssize.x)
                writeBuf(0, y, size.x, 1, data);
        } else {
            // Prepare a buffer filled with whitespaces.
            TScreenCell *b = new TScreenCell[size.x];
            {
                TScreenCell cell = 0;
                ::setChar(cell, ' ');
                ::setAttr(cell, sface->getFillColor());
                for (int i = 0; i < size.x; ++i)
                    b[i] = cell;
            }
            // Write the empty area at the top and the bottom.
            for (y = 0; y < clip.a.y; ++y)
                writeBuf(0, y, size.x, 1, b);
            for (y = clip.b.y; y < size.y; ++y)
                writeBuf(0, y, size.x, 1, b);
            // Write the surface's contents.
            if (clip.a.x == 0 && clip.b.x == size.x)
                // Direct copy also possible.
                for (y = clip.a.y; y < clip.b.y; ++y, data += ssize.x)
                    writeBuf(0, y, size.x, 1, data);
            else
                for (y = clip.a.y; y < clip.b.y; ++y, data += ssize.x) {
                    memcpy(&b[clip.a.x], data, (clip.b.x - clip.a.x)*sizeof(TScreenCell));
                    writeBuf(0, y, size.x, 1, b);
                }
            delete[] b;
        }
    }
}

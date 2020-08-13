/*------------------------------------------------------------*/
/* filename -       tsurface.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  Member function(s) of following classes   */
/*                      TSurface                              */
/*                      TSurfaceView                          */
/*------------------------------------------------------------*/

#define Uses_TSurface
#define Uses_TSurfaceView
#include <tvision/tv.h>

#include <stdlib.h>

TSurface::TSurface(const TRect &bounds) :
    TView(bounds),
    areaSize(0),
    drawArea(0),
    fill(0)
{
    updateSize();
}

TSurface::~TSurface()
{
    free(drawArea);
}

void TSurface::draw()
{
    writeBuf(0, 0, size.x, size.y, drawArea);
}

void TSurface::changeBounds(const TRect &bounds)
{
    setBounds(bounds);
    updateSize();
    drawView();
}

void TSurface::updateSize()
{
    size_t newSize = max(size.x*size.y, 0);
    if ((drawArea = (TScreenCell _FAR *) realloc(drawArea, newSize*sizeof(TScreenCell))))
        for (size_t i = areaSize; i < newSize; ++i)
            drawArea[i] = fill;
    areaSize = newSize;
}

void TSurface::clear()
{
    for (size_t i = 0; i < areaSize; ++i)
        drawArea[i] = fill;
}

void TSurfaceView::draw()
{
    if (sface) {
        int countX = min(size.x, sface->size.x - delta.x);
        int countY = min(size.y, sface->size.y - delta.y);
        if (countX == size.x)
            for (int y = 0; y < countY; ++y)
                writeBuf(0, y, countX, 1, &sface->at(y + delta.y, delta.x));
        else { // countX < size.x
            TCellAttribs color = sface->getFillColor();
            for (int y = 0; y < countY; ++y) {
                writeBuf(0, y, countX, 1, &sface->at(y + delta.y, delta.x));
                writeChar(countX, y, '\0', color, size.x - countX);
            }
        }
    }
}

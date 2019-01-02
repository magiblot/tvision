/*------------------------------------------------------------*/
/* filename -       tframe.cpp                                */
/*                                                            */
/* function(s)                                                */
/*                  TFrame member functions                   */
/*------------------------------------------------------------*/

/*------------------------------------------------------------*/
/*                                                            */
/*    Turbo Vision -  Version 1.0                             */
/*                                                            */
/*                                                            */
/*    Copyright (c) 1991 by Borland International             */
/*    All Rights Reserved.                                    */
/*                                                            */
/*------------------------------------------------------------*/

#define Uses_TFrame
#define Uses_TDrawBuffer
#define Uses_TWindow
#define Uses_TRect
#define Uses_TPoint
#define Uses_TEvent
#include <tv.h>

#if !defined( __STRING_H )
#include <String.h>
#endif  // __STRING_H

#define cpFrame "\x01\x01\x02\x02\x03"

TFrame::TFrame( const TRect& bounds ) : TView( bounds )
{
    growMode = gfGrowHiX + gfGrowHiY;
    eventMask |= evBroadcast;
}

void TFrame::draw()
{
    ushort cFrame, cTitle;
    short  f, i, l, width;
    TDrawBuffer b;

    if( (state & sfActive) == 0 )
        {
        cFrame = 0x0101;
        cTitle = 0x0002;
        f = 0;
        }
    else
        if( (state & sfDragging) != 0 )
            {
            cFrame = 0x0505;
            cTitle = 0x0005;
            f = 0;
            }
        else
            {
            cFrame = 0x0503;
            cTitle = 0x0004;
            f = 9;
            }

    cFrame = getColor(cFrame);
    cTitle = getColor(cTitle);

    width = size.x;
    l = width - 10;

    if( ( ((TWindow *)owner)->flags & (wfClose | wfZoom) ) != 0 )
        l -= 6;
    frameLine( b, 0, f, uchar(cFrame) );
    if( ((TWindow *)owner)->number != wnNoNumber &&
        ((TWindow *)owner)->number < 10
      )
        {
        l -= 4;
        if( ( ((TWindow *)owner)->flags & wfZoom ) != 0 )
            i = 7;
        else
            i = 3;
        b.putChar( width-i, ((TWindow *)owner)->number + '0' );
        }

    if( owner != 0 )
        {
        const char *title = ((TWindow *)owner)->getTitle(l);
        if( title != 0 )
            {
            l = min( cstrlen(title), width - 10 );
            l = max( l, 0 );
            i = (width - l) >> 1;
            b.putChar( i-1, ' ' );
            b.moveBuf( i, title, cTitle, l );
            b.putChar( i+l, ' ' );
            }
        }

    if( (state & sfActive) != 0 )
        {
        if( ( ((TWindow *)owner)->flags & wfClose ) != 0 )
            b.moveCStr( 2, closeIcon, cFrame );
        if( ( ((TWindow *)owner)->flags & wfZoom ) != 0 )
            {
            TPoint minSize, maxSize;
            owner->sizeLimits( minSize, maxSize );
            if( owner->size == maxSize )
                b.moveCStr( width-5, unZoomIcon, cFrame );
            else
                b.moveCStr( width-5, zoomIcon, cFrame );
            }
        }

    writeLine( 0, 0, size.x, 1, b );
    for( i = 1; i <=  size.y - 2; i++ )
        {
        frameLine( b, i, f +  3, cFrame );
        writeLine( 0, i, size.x, 1, b );
        }
    frameLine( b, size.y - 1, f +  6, cFrame );
    if( (state & sfActive) != 0 )
        if( ( ((TWindow *)owner)->flags & wfGrow ) != 0 )
            b.moveCStr( width-2, dragIcon, cFrame );
    writeLine( 0, size.y - 1, size.x, 1, b );
}

TPalette& TFrame::getPalette() const
{
    static TPalette palette( cpFrame, sizeof( cpFrame )-1 );
    return palette;
}

void TFrame::dragWindow( TEvent& event, uchar mode )
{
    TRect  limits;
    TPoint min, max;

    limits = owner->owner->getExtent();
    owner->sizeLimits( min, max );
    owner->dragView( event, owner->dragMode | mode, limits, min, max );
    clearEvent( event );
}

void TFrame::handleEvent( TEvent& event )
{
    TView::handleEvent(event);
    if( event.what== evMouseDown && (state & sfActive) != 0 )
        {
        TPoint mouse = makeLocal( event.mouse.where );
        if( mouse.y == 0 )
            {
            if( ( ((TWindow *)owner)->flags & wfClose ) != 0 &&
                    mouse.x >= 2 &&
                    mouse.x <= 4 )
                {
                event.what= evCommand;
                event.message.command = cmClose;
                event.message.infoPtr = owner;
                putEvent( event );
                clearEvent( event );
                }
            else
                if( (((TWindow *)owner)->flags & wfZoom) != 0 && 
                    ( (mouse.x >= size.x - 5 && mouse.x <= size.x - 3) ||
                       event.mouse.doubleClick
                    )
                  )
                    {
                    event.what= evCommand;
                    event.message.command = cmZoom;
                    event.message.infoPtr = owner;
                    putEvent( event );
                    clearEvent( event );
                    }
                else
                    if( ( ((TWindow *)owner)->flags & wfMove ) != 0 )
                        dragWindow( event, dmDragMove );
            }
        else
            if( mouse.x >= size.x - 2 && mouse.y >= size.y - 1 )
                if( ( ((TWindow *)owner)->flags & wfGrow ) != 0 )
                    dragWindow( event, dmDragGrow );
        }
}

void TFrame::setState( ushort aState, Boolean enable )
{
    TView::setState( aState, enable );
    if( (aState & (sfActive | sfDragging)) != 0 )
        drawView();
}

TStreamable *TFrame::build()
{
    return new TFrame( streamableInit );
}

TFrame::TFrame( StreamableInit ) : TView( streamableInit )
{
}



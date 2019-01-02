/*------------------------------------------------------------*/
/* filename -       tscrolle.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TScroller member functions                */
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

#define Uses_TScroller
#define Uses_TScrollBar
#define Uses_TEvent
#define Uses_TRect
#define Uses_opstream
#define Uses_ipstream
#include <tv.h>

#if !defined( __CTYPE_H )
#include <ctype.h>
#endif  // __CTYPE_H

#if !defined( __STRING_H )
#include <String.h>
#endif  // __STRING_H

#if !defined( __DOS_H )
#include <Dos.h>
#endif  // __DOS_H

#define cpScroller "\x06\x07"

TScroller::TScroller( const TRect& bounds,
                      TScrollBar *aHScrollBar,
                      TScrollBar *aVScrollBar) :
    TView( bounds ),
    drawLock( 0 ),
    drawFlag( False ),
    hScrollBar( aHScrollBar ),
    vScrollBar( aVScrollBar )
{
    delta.x = delta.y = limit.x = limit.y = 0;
    options |= ofSelectable;
    eventMask |= evBroadcast;
}

void TScroller::shutDown()
{
    hScrollBar = 0;
    vScrollBar = 0;
    TView::shutDown();
}

void TScroller::changeBounds( const TRect& bounds )
{
    setBounds(bounds);
    drawLock++;
    setLimit(limit.x, limit.y);
    drawLock--;
    drawFlag = False;
    drawView();
}

void TScroller::checkDraw()
{
    if( drawLock == 0 && drawFlag != False )
        {
        drawFlag = False;
        drawView();
        }
}

TPalette& TScroller::getPalette() const
{
    static TPalette palette( cpScroller, sizeof( cpScroller )-1 );
    return palette;
}

void TScroller::handleEvent(TEvent& event)
{
    TView::handleEvent(event);

    if( event.what == evBroadcast &&
        event.message.command == cmScrollBarChanged &&
          ( event.message.infoPtr == hScrollBar ||
            event.message.infoPtr == vScrollBar )
      )
        scrollDraw();
}

void TScroller::scrollDraw()
{
    TPoint  d;

    if( hScrollBar != 0 )
        d.x = hScrollBar->value;
    else
        d.x = 0;

    if( vScrollBar != 0 )
        d.y = vScrollBar->value;
    else
        d.y = 0;

    if( d.x != delta.x || d.y != delta.y )
        {
        setCursor( cursor.x + delta.x - d.x, cursor.y + delta.y - d.y );
        delta = d;
        if( drawLock != 0 )
            drawFlag = True;
        else
            drawView();
        }
}

void TScroller::scrollTo( short x, short y )
{
    drawLock++;
    if( hScrollBar != 0 )
        hScrollBar->setValue(x);
    if( vScrollBar != 0 )
        vScrollBar->setValue(y);
    drawLock--;
    checkDraw();
}

void TScroller::setLimit( short x, short y )
{
    limit.x = x;
    limit.y = y;
    drawLock++;
    if( hScrollBar != 0 )
        hScrollBar->setParams( hScrollBar->value,
                               0,
                               x - size.x,
                               size.x-1,
                               1
                             );
    if( vScrollBar != 0 )
        vScrollBar->setParams( vScrollBar->value,
                               0,
                               y - size.y,
                               size.y-1,
                               1
                             );
    drawLock--;
    checkDraw();
}

void TScroller::showSBar( TScrollBar *sBar )
{
    if( sBar != 0 )
        if( getState(sfActive | sfSelected) != 0 )
            sBar->show();
        else
            sBar->hide();
}

void TScroller::setState( ushort aState, Boolean enable )
{
    TView::setState(aState, enable);
    if( (aState & (sfActive | sfSelected)) != 0 )
        {
        showSBar(hScrollBar);
        showSBar(vScrollBar);
        }
}

void TScroller::write( opstream& os )
{
    TView::write( os );
    os << hScrollBar << vScrollBar << delta << limit;
}

void *TScroller::read( ipstream& is )
{
    TView::read( is );
    is >> hScrollBar >> vScrollBar >> delta >> limit;
    drawLock = 0;
    drawFlag = False;
    return this;
}

TStreamable *TScroller::build()
{
    return new TScroller( streamableInit );
}

TScroller::TScroller( StreamableInit ) : TView( streamableInit )
{
}



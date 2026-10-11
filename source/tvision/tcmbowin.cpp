/*------------------------------------------------------------*/
/* filename -       tcmbowin.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TComboWindow member functions              */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TComboWindow
#define Uses_TComboViewer
#define Uses_TComboItem
#define Uses_TScrollBar
#define Uses_TEvent
#include <tvision/tv.h>

#define cpComboWindow "\x13\x13\x15\x18\x17\x13\x14"

TComboWindow::TComboWindow( const TRect& bounds,
                            TComboItem *aItems,
                            short aFocused ) noexcept :
    TWindowInit( &TComboWindow::initFrame ),
    TWindow( bounds, 0, wnNoNumber )
{
    flags = wfClose;
    TRect r = getExtent();
    r.grow( -1, -1 );
    viewer = new TComboViewer( r,
        standardScrollBar( sbVertical | sbHandleKeyboard ),
        aItems, aFocused );
    insert( viewer );
}

TPalette& TComboWindow::getPalette() const
{
    static TPalette palette( cpComboWindow, sizeof( cpComboWindow )-1 );
    return palette;
}

short TComboWindow::getSelection()
{
    return viewer->focused;
}

void TComboWindow::handleEvent( TEvent& event )
{
    TWindow::handleEvent( event );
    if( event.what == evMouseDown && !mouseInView( event.mouse.where ) )
        {
        endModal( cmCancel );
        clearEvent( event );
        }
}

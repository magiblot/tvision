/*------------------------------------------------------------*/
/* filename -       tprogram.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TProgram member functions                 */
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

#define Uses_TKeys
#define Uses_TProgram
#define Uses_TEvent
#define Uses_TScreen
#define Uses_TStatusLine
#define Uses_TMenu
#define Uses_TGroup
#define Uses_TDeskTop
#define Uses_TEventQueue
#define Uses_TMenuBar
#define Uses_TStatusDef
#define Uses_TStatusItem
#include <tv.h>

// Public variables

TStatusLine * near TProgram::statusLine = 0;
TMenuBar * near TProgram::menuBar = 0;
TDeskTop * near TProgram::deskTop = 0;
TProgram * near TProgram::application = 0;
int near TProgram::appPalette = apColor;
TEvent near TProgram::pending;

extern TPoint shadowSize;

TProgInit::TProgInit( TStatusLine *(*cStatusLine)( TRect ),
                            TMenuBar *(*cMenuBar)( TRect ),
                            TDeskTop *(*cDeskTop )( TRect )
                          ) :
    createStatusLine( cStatusLine ),
    createMenuBar( cMenuBar ),
    createDeskTop( cDeskTop )
{
}


TProgram::TProgram() :
    TProgInit( &TProgram::initStatusLine,
                  &TProgram::initMenuBar,
                  &TProgram::initDeskTop
                ),
    TGroup( TRect( 0,0,TScreen::screenWidth,TScreen::screenHeight ) )
{
    application = this;
    initScreen();
    state = sfVisible | sfSelected | sfFocused | sfModal | sfExposed;
    options = 0;
    buffer = (ushort far *)TScreen::screenBuffer;

    if( createDeskTop != 0 &&
        (deskTop = createDeskTop( getExtent() )) != 0
      )
        insert(deskTop);

    if( createStatusLine != 0 &&
        (statusLine = createStatusLine( getExtent() )) != 0
      )
        insert(statusLine);

    if( createMenuBar != 0 &&
        (menuBar = createMenuBar( getExtent() )) != 0
      )
        insert(menuBar);

}

TProgram::~TProgram()
{
    application = 0;
}

void TProgram::shutDown()
{
    statusLine = 0;
    menuBar = 0;
    deskTop = 0;
    TGroup::shutDown();
}

inline Boolean hasMouse( TView *p, void *s )
{
    return Boolean( (p->state & sfVisible) != 0 &&
                     p->mouseInView( ((TEvent *)s)->mouse.where ));
}

void TProgram::getEvent(TEvent& event)
{
    if( pending.what != evNothing )
        {
        event = pending;
        pending.what = evNothing;
        }
    else
        {
        event.getMouseEvent();
        if( event.what == evNothing )
            {
            event.getKeyEvent();
            if( event.what == evNothing )
                idle();
            }
        }

    if( statusLine != 0 )
        {
        if( (event.what & evKeyDown) != 0 ||
            ( (event.what & evMouseDown) != 0 &&
              firstThat( hasMouse, &event ) == statusLine
            )
          )
            statusLine->handleEvent( event );
        }
}

TPalette& TProgram::getPalette() const
{
    static TPalette color ( cpColor, sizeof( cpColor )-1 );
    static TPalette blackwhite( cpBlackWhite, sizeof( cpBlackWhite )-1 );
    static TPalette monochrome( cpMonochrome, sizeof( cpMonochrome )-1 );
    static TPalette *palettes[] =
        {
        &color,
        &blackwhite,
        &monochrome
        };
    return *(palettes[appPalette]);
}

void TProgram::handleEvent( TEvent& event )
{
    if( event.what == evKeyDown )
        {
        char c = getAltChar( event.keyDown.keyCode );
        if( c >= '1' && c <= '9' )
            {
            if( message( deskTop,
                         evBroadcast,
                         cmSelectWindowNum,
                         (void *)(c - '0')
                       ) != 0 )
                clearEvent( event );
            }
        }

    TGroup::handleEvent( event );
    if( event.what == evCommand && event.message.command == cmQuit )
        {
        endModal( cmQuit );
        clearEvent( event );
        }
}

void TProgram::idle()
{
    if( statusLine != 0 )
        statusLine->update();

    if( commandSetChanged == True )
        {
        message( this, evBroadcast, cmCommandSetChanged, 0 );
        commandSetChanged = False;
        }
}

TDeskTop *TProgram::initDeskTop( TRect r )
{
    r.a.y++;
    r.b.y--;
    return new TDeskTop( r );
}

TMenuBar *TProgram::initMenuBar( TRect r )
{
    r.b.y = r.a.y + 1;
    return new TMenuBar( r, (TMenu *)0 );
}

void TProgram::initScreen()
{
    if( (TScreen::screenMode & 0x00FF) != TDisplay::smMono )
        {
        if( (TScreen::screenMode & TDisplay::smFont8x8) != 0 )
            shadowSize.x = 1;
        else
            shadowSize.x = 2;
        shadowSize.y = 1;
        showMarkers = False;
        if( (TScreen::screenMode & 0x00FF) == TDisplay::smBW80 )
            appPalette = apBlackWhite;
        else
            appPalette = apColor;
        }
    else
        {

        shadowSize.x = 0;
        shadowSize.y = 0;
        showMarkers = True;
        appPalette = apMonochrome;
        }
}

TStatusLine *TProgram::initStatusLine( TRect r )
{
    r.a.y = r.b.y - 1;
    return new TStatusLine( r,
        *new TStatusDef( 0, 0xFFFF ) +
            *new TStatusItem( exitText, kbAltX, cmQuit ) +
            *new TStatusItem( 0, kbF10, cmMenu ) +
            *new TStatusItem( 0, kbAltF3, cmClose ) +
            *new TStatusItem( 0, kbF5, cmZoom ) +
            *new TStatusItem( 0, kbCtrlF5, cmResize )
            );
}

void TProgram::outOfMemory()
{
}

void TProgram::putEvent( TEvent & event )
{
    pending = event;
}

void TProgram::run()
{
    execute();
}

void TProgram::setScreenMode( ushort mode )
{
    TRect  r;

    TEventQueue::mouse.hide(); //HideMouse();
    TScreen::setVideoMode( mode );
    initScreen();
    buffer = (ushort far *)TScreen::screenBuffer;
    r = TRect( 0, 0, TScreen::screenWidth, TScreen::screenHeight );
    changeBounds( r );
    setState(sfExposed, False);
    redraw();
    setState(sfExposed, True);
    TEventQueue::mouse.show(); //ShowMouse();
}

TView* TProgram::validView(TView* p)
{
    if( p == 0 )
        return 0;
    if( lowMemory() )
        {
        destroy( p );
        outOfMemory();
        return 0;
        }
    if( !p->valid( cmValid ) )
        {
        destroy( p );
        return 0;
        }
    return p;
}




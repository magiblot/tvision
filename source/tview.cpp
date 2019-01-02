/*------------------------------------------------------------*/
/* filename -       tview.cpp                                 */
/*                                                            */
/* function(s)                                                */
/*                  TView member functions                    */
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
#define Uses_TView
#define Uses_TCommandSet
#define Uses_TPoint
#define Uses_TGroup
#define Uses_TRect
#define Uses_TEvent
#define Uses_opstream
#define Uses_ipstream
#include <tv.h>

#if !defined( __DOS_H )
#include <Dos.h>
#endif  // __DOS_H

#if !defined( __LIMITS_H )
#include <Limits.h>
#endif  // __LIMITS_H

TPoint shadowSize = {2,1};
uchar shadowAttr = 0x08;
Boolean near TView::showMarkers = False;
uchar near TView::errorAttr = 0xCF;
Boolean near TView::commandSetChanged = False;

extern TView *TheTopView;

static TCommandSet initCommands()
{
    TCommandSet temp;
    for( int i = 0; i < 256; i++ )
        temp.enableCmd( i );
    temp.disableCmd( cmZoom );
    temp.disableCmd( cmClose );
    temp.disableCmd( cmResize );
    temp.disableCmd( cmNext );
    temp.disableCmd( cmPrev );
    return temp;
}

TCommandSet near TView::curCommandSet = initCommands();

TView::TView( const TRect& bounds) :
    owner( 0 ), next( 0 ), options( 0 ), state( sfVisible ),
    growMode( 0 ), dragMode( dmLimitLoY ), helpCtx( hcNoContext ),
    eventMask( evMouseDown | evKeyDown | evCommand )
{
    setBounds( bounds);
    cursor.x = cursor.y = 0;
}

TView::~TView()
{
}

void TView::blockCursor()
{
     setState(sfCursorIns, True);
}

#define grow(i) (( (growMode & gfGrowRel)) ? \
                (i = (i * s + ((s - d) >> 1)) / (s - d)) : (i += d))

inline int range( int val, int min, int max )
{
    if( val < min )
        return min;
    else if( val > max )
        return max;
    else
        return val;
}
                
void TView::calcBounds( TRect& bounds, TPoint delta )
{
    bounds = getBounds();

    short s = owner->size.x;
    short d = delta.x;

    if( (growMode & gfGrowLoX) != 0 )
        grow(bounds.a.x);

    if( (growMode & gfGrowHiX) != 0 )
        grow(bounds.b.x);

    s = owner->size.y;
    d = delta.y;

    if( (growMode & gfGrowLoY) != 0 )
        grow(bounds.a.y);

    if( (growMode & gfGrowHiY) != 0 )
        grow(bounds.b.y);

    TPoint minLim, maxLim;
    sizeLimits( minLim, maxLim );
    bounds.b.x = bounds.a.x + range( bounds.b.x-bounds.a.x, minLim.x, maxLim.x );
    bounds.b.y = bounds.a.y + range( bounds.b.y-bounds.a.y, minLim.y, maxLim.y );
}

void TView::changeBounds( const TRect& bounds )
{
    setBounds(bounds);
    drawView();
}

void TView::clearEvent( TEvent& event )
{
    event.what = evNothing;
    event.message.infoPtr = this;
}

Boolean TView::commandEnabled( ushort command )
{
    return Boolean((command > 255) || curCommandSet.has(command));
}

ushort TView::dataSize()
{
    return 0;
}

void TView::disableCommands( TCommandSet& commands )
{
    commandSetChanged = Boolean( commandSetChanged ||
                                !(curCommandSet & commands).isEmpty());
    curCommandSet.disableCmd(commands);
}

void TView::disableCommand( ushort command )
{
    commandSetChanged = Boolean( commandSetChanged ||
                                 curCommandSet.has(command) );
    curCommandSet.disableCmd(command);
}

void TView::moveGrow( TPoint p,
                      TPoint s,
                      TRect& limits,
                      TPoint minSize,
                      TPoint maxSize,
                      uchar mode
                    )
{
    TRect   r;
    s.x = min(max(s.x, minSize.x), maxSize.x);
    s.y = min(max(s.y, minSize.y), maxSize.y);
    p.x = min(max(p.x, limits.a.x - s.x+1), limits.b.x-1);
    p.y = min(max(p.y, limits.a.y - s.y+1), limits.b.y-1);

    if( (mode & dmLimitLoX) != 0 )
        p.x = max(p.x, limits.a.x);
    if( (mode & dmLimitLoY) != 0 )
        p.y = max(p.y, limits.a.y);
    if( (mode & dmLimitHiX) != 0 )
        p.x = min(p.x, limits.b.x-s.x);
    if( (mode & dmLimitHiY) != 0 )
        p.y = min(p.y, limits.b.y-s.y);
    r = TRect(p.x, p.y, p.x +  s.x, p.y +  s.y);
    locate(r);
}

void TView::change( uchar mode, TPoint delta, TPoint& p, TPoint& s )
{
# ifdef PROTECT
    unsigned char *shiftState = (unsigned char *) MK_FP(biosSeg,0x17);
# else
    unsigned char *shiftState = (unsigned char *) MK_FP(0x40,0x17);
# endif

    if( (mode & dmDragMove) != 0 && (*shiftState & 3) == 0 )
        p += delta;
    else if( (mode & dmDragGrow) != 0 && (*shiftState & 3) != 0 )
        s += delta;
}

void TView::dragView( TEvent& event,
                      uchar mode,
                      TRect& limits,
                      TPoint minSize,
                      TPoint maxSize
                    )
{
    TRect saveBounds;

    TPoint p, s;
    setState( sfDragging, True );

    if( event.what == evMouseDown )
        {
        if( (mode & dmDragMove) != 0 )
            {
            p = origin - event.mouse.where;
            do  {
                event.mouse.where += p;
                moveGrow( event.mouse.where,
                          size,
                          limits,
                          minSize,
                          maxSize,
                          mode
                        );
                } while( mouseEvent(event,evMouseMove) );
            }
        else
            {
            p = size - event.mouse.where;
            do  {
                event.mouse.where += p;
                moveGrow( origin,
                          event.mouse.where,
                          limits,
                          minSize,
                          maxSize,
                          mode
                        );
                } while( mouseEvent(event,evMouseMove) );
            }
        }
    else
        {
        static TPoint 
            goLeft      =   {-1, 0}, 
            goRight     =   { 1, 0}, 
            goUp        =   { 0,-1}, 
            goDown      =   { 0, 1}, 
            goCtrlLeft  =   {-8, 0}, 
            goCtrlRight =   { 8, 0};
            
        saveBounds = getBounds();
        do  {
            p = origin;
            s = size;
            keyEvent(event);
            switch (event.keyDown.keyCode & 0xFF00)
                {
                case kbLeft:
                    change(mode,goLeft,p,s);
                    break;
                case kbRight:
                    change(mode,goRight,p,s);
                    break;
                case kbUp:
                    change(mode,goUp,p,s);
                    break;
                case kbDown:
                    change(mode,goDown,p,s);
                    break;
                case kbCtrlLeft:
                    change(mode,goCtrlLeft,p,s);
                    break;
                case kbCtrlRight:
                    change(mode,goCtrlRight,p,s);
                    break;
                case kbHome:
                    p.x = limits.a.x;
                    break;
                case kbEnd:
                    p.x = limits.b.x - s.x;
                    break;
                case kbPgUp:
                    p.y = limits.a.y;
                    break;
                case kbPgDn:
                    p.y = limits.b.y - s.y;
                    break;
                }
            moveGrow( p, s, limits, minSize, maxSize, mode );
            } while( event.keyDown.keyCode != kbEsc &&
                     event.keyDown.keyCode != kbEnter
                   );
        if( event.keyDown.keyCode == kbEsc )
            locate(saveBounds);
        }
    setState(sfDragging, False);
}

void TView::draw()
{
    TDrawBuffer  b;

    b.moveChar( 0, ' ', getColor(1), size.x );
    writeLine( 0, 0, size.x, size.y, b );
}

void TView::drawCursor()
{
    if( (state & sfFocused) != 0 )
        resetCursor();
}

void TView::drawHide( TView* lastView )
{
    drawCursor();
    drawUnderView(Boolean(state & sfShadow), lastView);
}

void TView::drawShow( TView* lastView )
{
    drawView();
    if( (state & sfShadow) != 0 )
        drawUnderView( True, lastView );
}

void TView::drawUnderRect( TRect& r, TView* lastView )
{
    owner->clip.intersect(r);
    owner->drawSubViews(nextView(), lastView);
    owner->clip = owner->getExtent();
}

void TView::drawUnderView( Boolean doShadow, TView* lastView )
{
    TRect r = getBounds();
    if( doShadow != False )
        r.b += shadowSize;
    drawUnderRect( r, lastView );
}

void TView::drawView()
{
    if (exposed())
        {
        draw();
        drawCursor();
        }
}

void TView::enableCommands( TCommandSet& commands )
{
    commandSetChanged = Boolean( commandSetChanged ||
                                ((curCommandSet&commands) != commands) );
    curCommandSet += commands;
}

void TView::enableCommand( ushort command )
{
    commandSetChanged = Boolean( commandSetChanged ||
                                 !curCommandSet.has( command ) );
    curCommandSet += command;
}

void TView::endModal( ushort command )
{
    if( TopView() != 0 )
        TopView()->endModal(command);
}

Boolean  TView::eventAvail()
{
    TEvent event;
    getEvent(event);
    if( event.what != evNothing )
        putEvent(event);
    return Boolean( event.what != evNothing );
}

TRect TView::getBounds()
{
    return TRect( origin, origin+size );
}

ushort  TView::execute()
{
    return cmCancel;
}

TRect TView::getClipRect()
{
    TRect clip = getBounds();
    if( owner != 0 )
        clip.intersect(owner->clip);
    clip.move(-origin.x, -origin.y);
    return clip;
}

ushort TView::getColor( ushort color )
{
    ushort colorPair = color >> 8;

    if( colorPair != 0 )
        colorPair = mapColor(colorPair) << 8;

    colorPair |= mapColor( uchar(color) );

    return colorPair;
}

void TView::getCommands( TCommandSet& commands )
{
    commands = curCommandSet;
}

void TView::getData( void * )
{
}

void TView::getEvent( TEvent& event )
{
    if( owner != 0 )
        owner->getEvent(event);
}

TRect TView::getExtent()
{
    return TRect( 0, 0, size.x, size.y );
}

ushort TView::getHelpCtx()
{
    if( (state & sfDragging) != 0 )
        return hcDragging;
    return helpCtx;
}

TPalette& TView::getPalette() const
{
    static char ch = 0;
    static TPalette palette( &ch, 0 );
    return palette;
}

Boolean TView::getState( ushort aState )
{
    return Boolean( (state & aState) == aState );
}

void TView::growTo( short x, short y )
{
    TRect r = TRect(origin.x, origin.y, origin.x + x, origin.y + y);
    locate(r);
}

void TView::handleEvent(TEvent& event)
{
    if( event.what == evMouseDown )
        {
        if(!(state & (sfSelected | sfDisabled)) && (options & ofSelectable) )
            {
            select();
            if( !(options & ofFirstClick) )
                clearEvent(event);
            }
        }
}

void TView::hide()
{
    if( (state & sfVisible) != 0 )
        setState( sfVisible, False );
}

void TView::hideCursor()
{
    setState( sfCursorVis, False );
}

void TView::keyEvent( TEvent& event )
{
    do {
       getEvent(event);
        } while( event.what != evKeyDown );
}

#define range(Val, Min, Max)    (((Val < Min) ? Min : ((Val > Max) ? Max : Val)))

void TView::locate( TRect& bounds )
{
    TPoint   min, max;
    sizeLimits(min, max);
    bounds.b.x = bounds.a.x + range(bounds.b.x - bounds.a.x, min.x, max.x);
    bounds.b.y = bounds.a.y + range(bounds.b.y - bounds.a.y, min.y, max.y);
    TRect r = getBounds();
    if( bounds != r )
        {
        changeBounds( bounds );
        if( owner != 0 && (state & sfVisible) != 0 )
            {
            if( (state & sfShadow) != 0 )
                {
                r.Union(bounds);
                r.b += shadowSize;
                }
            drawUnderRect( r, 0 );
            }
        }
}

void TView::makeFirst()
{
    putInFrontOf(owner->first());
}

TPoint TView::makeGlobal( TPoint source )
{
    TPoint temp = source + origin;
    TView *cur = this;
    while( cur->owner != 0 )
        {
        cur = cur->owner;
        temp += cur->origin;
        }
    return temp;
}

TPoint TView::makeLocal( TPoint source )
{
    TPoint temp = source - origin;
    TView* cur = this;
    while( cur->owner != 0 )
        {
        cur = cur->owner;
        temp -= cur->origin;
        }
    return temp;
}

Boolean TView::mouseEvent(TEvent& event, ushort mask)
{
    do {
       getEvent(event);
        } while( !(event.what & (mask | evMouseUp)) );

    return Boolean(event.what != evMouseUp);
}

Boolean TView::mouseInView(TPoint mouse)
{
     mouse = makeLocal( mouse );
     TRect r = getExtent();
     return r.contains(mouse);
}

void TView::moveTo( short x, short y )
{
     TRect r( x, y, x+size.x, y+size.y );
     locate(r);
}

TView *TView::nextView()
{
    if( this == owner->last )
        return 0;
    else
        return next;
}

void TView::normalCursor()
{
    setState(sfCursorIns, False);
}

TView *TView::prev()
{
    TView* res = this;
    while( res->next != this )
        res = res->next;
    return res;
}

TView *TView::prevView()
{
    if( this == owner->first() )
        return 0;
    else
        return prev();
}

void TView::putEvent( TEvent& event )
{
    if( owner != 0 )
        owner->putEvent(event);
}

void TView::putInFrontOf( TView *Target )
{
    TView *p, *lastView;

    if( owner != 0 && Target != this && Target != nextView() &&
         ( Target == 0 || Target->owner == owner)
      )
        if( (state & sfVisible) == 0 )
            {
            owner->removeView(this);
            owner->insertView(this, Target);
            }
        else
            {
            lastView = nextView();
            p = Target;
            while( p != 0 && p != this )
                p = p->nextView();
            if( p == 0 )
                lastView = Target;
            state &= ~sfVisible;
            if( lastView == Target )
                drawHide(lastView);
            owner->removeView(this);
            owner->insertView(this, Target);
            state |= sfVisible;
            if( lastView != Target )
                drawShow(lastView);
            if( (options & ofSelectable) != 0 )
                owner->resetCurrent();
            }
}

void TView::select()
{
    if( (options & ofTopSelect) != 0 )
        makeFirst();
    else if( owner != 0 )
        owner->setCurrent( this, normalSelect );
}

void TView::setBounds( const TRect& bounds )
{            
    origin = bounds.a;
    size = bounds.b - bounds.a;
}

void TView::setCommands( TCommandSet& commands )
{
    commandSetChanged = Boolean( commandSetChanged ||
                                (curCommandSet != commands ));
    curCommandSet = commands;
}

void TView::setCursor( short x, short y )
{
    cursor.x = x;
    cursor.y = y;
    drawCursor();
}

void TView::setData( void * )
{
}

void TView::setState( ushort aState, Boolean enable )
{
    if( enable == True )
        state |= aState;
    else
        state &= ~aState;

    if( owner == 0 )
        return;

    switch( aState )
        {
        case  sfVisible:
            if( (owner->state & sfExposed) != 0 )
                setState( sfExposed, enable );
            if( enable == True )
                drawShow( 0 );
            else
                drawHide( 0 );
            if( (options & ofSelectable) != 0 )
                owner->resetCurrent();
            break;
        case  sfCursorVis:
        case  sfCursorIns:
            drawCursor();
            break;
        case  sfShadow:
            drawUnderView( True, 0 );
            break;
        case  sfFocused:
            resetCursor();
            message( owner,
                     evBroadcast,
                     (enable == True) ? cmReceivedFocus : cmReleasedFocus,
                     this
                   );
            break;
        }
}

void TView::show()
{
    if( (state & sfVisible) == 0 )
        setState(sfVisible, True);
}

void TView::showCursor()
{
    setState( sfCursorVis, True );
}

void TView::sizeLimits( TPoint& min, TPoint& max )
{
    min.x = min.y = 0;
    if( owner != 0 )
        max = owner->size;
    else
        max.x = max.y = INT_MAX;
}

TView* TView::TopView()
{
    if( TheTopView != 0 )
        return TheTopView;
    else
        {
        TView* p = this;
        while( p != 0 && !(p->state & sfModal) )
            p = p->owner;
        return p;
        }
}

Boolean TView::valid( ushort )
{
    return True;
}

Boolean TView::containsMouse( TEvent& event )
{
    return Boolean( (state & sfVisible) != 0 &&
                    mouseInView( event.mouse.where )
                  );
}

void TView::shutDown()
{
    hide();
    if( owner != 0 )
        owner->remove( this );
    TObject::shutDown();
}

void TView::write( opstream& os )
{
    ushort saveState =
        state & ~( sfActive | sfSelected | sfFocused | sfExposed );

    os << origin << size << cursor
       << growMode << dragMode << helpCtx
       << saveState << options << eventMask;
}

void *TView::read( ipstream& is )
{
    is >> origin >> size >> cursor
       >> growMode >> dragMode >> helpCtx
       >> state >> options >> eventMask;
    owner = 0;
    next = 0;
    return this;
}

TStreamable *TView::build()
{
    return new TView( streamableInit );
}

TView::TView( StreamableInit )
{
}

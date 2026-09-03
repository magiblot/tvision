/*------------------------------------------------------------*/
/* filename -       tcombobo.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TComboBox member functions                 */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TKeys
#define Uses_TComboBox
#define Uses_TComboWindow
#define Uses_TComboItem
#define Uses_TEvent
#define Uses_TDrawBuffer
#define Uses_TGroup
#include <tvision/tv.h>

#define cpComboBox "\x13\x14\x16"

TComboBox::TComboBox( const TRect& bounds,
                      TComboItem *aItems,
                      short aFocused ) noexcept :
    TView( bounds ),
    focused( 0 ),
    text( 0 ),
    value( 0 ),
    items( aItems ),
    numItems( 0 )
{
    options |= ofSelectable | ofFirstClick;
    eventMask |= evBroadcast;

    TComboItem *p;
    for( p = items; p != 0; p = p->next )
        numItems++;

    if( numItems > 0 )
        {
        if( aFocused < 0 )
            aFocused = 0;
        else if( aFocused >= numItems )
            aFocused = numItems - 1;
        focused = aFocused;
        p = items;
        short i;
        for( i = 0; i < focused && p != 0; i++ )
            p = p->next;
        if( p != 0 )
            {
            text = p->text;
            value = p->value;
            }
        }
}

TComboBox::~TComboBox()
{
    TComboItem *p = items;
    while( p != 0 )
        {
        TComboItem *n = p->next;
        delete p;
        p = n;
        }
}

void TComboBox::draw()
{
    TDrawBuffer b;
    TColorAttr color = mapColor( (state & sfFocused) != 0 ? 2 : 1 );

    b.moveChar( 0, ' ', color, size.x );
    if( text != 0 && size.x > 3 )
        b.moveStr( 1, text, color, size.x - 3 );
    if( size.x > 1 )
        b.moveChar( size.x - 2, '\x1F', mapColor(3), 1 );
    writeLine( 0, 0, size.x, 1, b );
}

TPalette& TComboBox::getPalette() const
{
    static TPalette palette( cpComboBox, sizeof( cpComboBox )-1 );
    return palette;
}

void TComboBox::focusItem( short item ) noexcept
{
    if( numItems == 0 )
        return;
    if( item < 0 )
        item = 0;
    else if( item >= numItems )
        item = numItems - 1;
    focused = item;

    TComboItem *p = items;
    short i;
    for( i = 0; i < item && p != 0; i++ )
        p = p->next;
    if( p != 0 )
        {
        text = p->text;
        value = p->value;
        }
    drawView();
}

void TComboBox::newList( TComboItem *aItems, short aFocused ) noexcept
{
    TComboItem *p = items;
    while( p != 0 )
        {
        TComboItem *n = p->next;
        delete p;
        p = n;
        }

    items = aItems;
    numItems = 0;
    for( p = items; p != 0; p = p->next )
        numItems++;

    text = 0;
    value = 0;
    focused = 0;
    if( numItems > 0 )
        focusItem( aFocused );
    else
        drawView();
}

TComboWindow *TComboBox::initComboWindow( const TRect& bounds )
{
    return new TComboWindow( bounds, items, focused );
}

void TComboBox::handleEvent( TEvent& event )
{
    TView::handleEvent( event );
    if( numItems > 0 &&
        ( event.what == evMouseDown ||
          ( event.what == evKeyDown &&
            ( event.keyDown.charScan.charCode == ' ' ||
              event.keyDown.keyCode == kbEnter ||
              ctrlToArrow(event.keyDown.keyCode) == kbDown )
          )
        )
      )
        {
        TRect r = getBounds();
        r.a.y++;
        short rows = numItems < 8 ? numItems : 8;
        r.b.y = r.a.y + rows + 2;
        TRect limits = owner->getExtent();
        r.intersect( limits );

        TComboWindow *win = initComboWindow( r );
        if( win != 0 )
            {
            ushort c = owner->execView( win );
            if( c == cmOK )
                {
                short sel = win->getSelection();
                if( sel != focused )
                    {
                    focusItem( sel );
                    message( owner, evBroadcast, cmComboBoxSelectionChanged, this );
                    }
                }
            destroy( win );
            }
        drawView();
        clearEvent( event );
        }
}

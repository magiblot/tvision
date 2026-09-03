/*------------------------------------------------------------*/
/* filename -       tcmbovie.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TComboViewer member functions              */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TKeys
#define Uses_TComboViewer
#define Uses_TComboItem
#define Uses_TScrollBar
#define Uses_TEvent
#include <tvision/tv.h>

#if !defined( __STRING_H )
#include <string.h>
#endif  // __STRING_H

#if !defined( __CTYPE_H )
#include <ctype.h>
#endif  // __CTYPE_H

#define cpComboViewer "\x06\x06\x07\x06\x06"

static TComboItem *nthItem( TComboItem *items, short n ) noexcept
{
    while( items != 0 && n > 0 )
        {
        items = items->next;
        n--;
        }
    return items;
}

TComboViewer::TComboViewer( const TRect& bounds,
                            TScrollBar *aVScrollBar,
                            TComboItem *aItems,
                            short aFocused ) noexcept :
    TListViewer( bounds, 1, 0, aVScrollBar ),
    items( aItems )
{
    short count = 0;
    TComboItem *p;
    for( p = items; p != 0; p = p->next )
        count++;
    setRange( count );
    if( range > 0 )
        focusItem( aFocused < range ? aFocused : short(range - 1) );
}

TPalette& TComboViewer::getPalette() const
{
    static TPalette palette( cpComboViewer, sizeof( cpComboViewer )-1 );
    return palette;
}

void TComboViewer::getText( char *dest, short item, short maxChars )
{
    TComboItem *p = nthItem( items, item );
    if( p != 0 )
        {
        strncpy( dest, p->text, maxChars );
        dest[maxChars] = EOS;
        }
    else
        *dest = EOS;
}

ulong TComboViewer::getValue( short item ) noexcept
{
    TComboItem *p = nthItem( items, item );
    return p != 0 ? p->value : 0;
}

void TComboViewer::handleEvent( TEvent& event )
{
    if( ( event.what == evMouseDown && (event.mouse.eventFlags & meDoubleClick) ) ||
        ( event.what == evKeyDown && event.keyDown.keyCode == kbEnter )
      )
        {
        endModal( cmOK );
        clearEvent( event );
        }
    else
        if( ( event.what == evKeyDown && event.keyDown.keyCode == kbEsc ) ||
            ( event.what == evCommand && event.message.command == cmCancel )
          )
            {
            endModal( cmCancel );
            clearEvent( event );
            }
        else if( event.what == evKeyDown && event.keyDown.charScan.charCode != ' ' &&
                 event.keyDown.charScan.charCode != '\0' )
            {
            // Incremental search: jump to the next entry whose text starts
            // with the typed character (case-insensitive), wrapping around.
            char typed = event.keyDown.charScan.charCode;
            if( range > 0 )
                {
                char buf[256];
                short i;
                for( i = 1; i <= range; i++ )
                    {
                    short candidate = (focused + i) % range;
                    getText( buf, candidate, 255 );
                    if( buf[0] != EOS && toupper((uchar)buf[0]) == toupper((uchar)typed) )
                        {
                        focusItemNum( candidate );
                        drawView();
                        break;
                        }
                    }
                }
            clearEvent( event );
            }
        else
            TListViewer::handleEvent( event );
}

int TComboViewer::itemWidth() noexcept
{
    int width = 0;
    char buf[256];
    short i;
    for( i = 0; i < range; i++ )
        {
        getText( buf, i, 255 );
        int w = strwidth( buf );
        if( w > width )
            width = w;
        }
    return width;
}

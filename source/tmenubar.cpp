/*------------------------------------------------------------*/
/* filename -       tmenubar.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TMenuBar member functions                 */
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

#define Uses_TMenuBar
#define Uses_TDrawBuffer
#define Uses_TMenu
#define Uses_TMenuItem
#define Uses_TRect
#define Uses_TSubMenu
#include <tv.h>

#if !defined( __STRING_H )
#include <String.h>
#endif  // __STRING_H

TMenuBar::TMenuBar( const TRect& bounds, TMenu *aMenu ) :
    TMenuView( bounds )
{
    menu = aMenu;
    growMode = gfGrowHiX;
    options |= ofPreProcess;
}

TMenuBar::TMenuBar( const TRect& bounds, TSubMenu& aMenu ) :
    TMenuView( bounds )
{
    menu = new TMenu( aMenu );
    growMode = gfGrowHiX;
    options |= ofPreProcess;
}

TMenuBar::~TMenuBar()
{
    delete menu;
}

void TMenuBar::draw()
{
    ushort color;
    short x, l;
    TMenuItem *p;
    TDrawBuffer b;

    ushort cNormal = getColor(0x0301);
    ushort cSelect = getColor(0x0604);
    ushort cNormDisabled =  getColor(0x0202);
    ushort cSelDisabled =  getColor(0x0505);
    b.moveChar( 0, ' ', cNormal, size.x );
    if( menu != 0 )
        {
        x = 1;
        p = menu->items;
        while( p != 0 )
            {
            if( p->name != 0 )
                {
                l = cstrlen(p->name);
                if( x + l < size.x )
                    {
                    if( p->disabled )
                        if( p == current )
                            color = cSelDisabled;
                        else
                            color = cNormDisabled;
                    else
                        if( p == current )
                            color = cSelect;
                        else
                            color = cNormal;

                    b.moveChar( x, ' ', color, 1 );
                    b.moveCStr( x+1, p->name, color );
                    b.moveChar( x+l+1, ' ', color, 1 );
                    }
                x += l + 2;
                }
            p = p->next;
            }
        }
    writeBuf( 0, 0, size.x, 1, b );
}

TRect TMenuBar::getItemRect( TMenuItem *item )
{
    TRect r( 1, 0, 1, 1 );
    TMenuItem *p = menu->items;
    while( True )
        {
        r.a.x = r.b.x;
        if( p->name != 0 )
            r.b.x += cstrlen(p->name) + 2;
        if( p == item )
            return r;
        p = p->next;
        }
}

TStreamable *TMenuBar::build()
{
    return new TMenuBar( streamableInit );
}

TMenuBar::TMenuBar( StreamableInit ) : TMenuView( streamableInit )
{
}



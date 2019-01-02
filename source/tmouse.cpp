/*------------------------------------------------------------*/
/* filename -       tmouse.cpp                                */
/*                                                            */
/* function(s)                                                */
/*                  TMouse and THWMouse member functions      */
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

#define Uses_TEvent
#include <tv.h>

#if !defined( __DOS_H )
#include <Dos.h>
#endif  // __DOS_H

uchar near THWMouse::buttonCount = 0;
Boolean near THWMouse::handlerInstalled = False;

THWMouse::THWMouse()
{
    resume();
}

void THWMouse::resume()
{
    if( getvect( 0x33 ) == 0 )
        return;

    _AX = 0;
    geninterrupt( 0x33 );

    if( _AX == 0 )
        return;
    buttonCount = _BL;

    _AX = 4;
    _CX = 0;
    _DX = 0;
    geninterrupt( 0x33 );
}

THWMouse::~THWMouse()
{
    suspend();
}

void THWMouse::suspend()
{
    if( present() == False )
        return;
    hide();
    if( handlerInstalled == True )
        {
        registerHandler( 0, 0 );
        handlerInstalled = False;
        }
    buttonCount = 0;
}

#pragma warn -asc

void THWMouse::show()
{
    asm push ax;
    asm push es;
       
    if( present() )
        {
        _AX = 1;
        geninterrupt( 0x33 );
        }

    asm pop es;
    asm pop ax;
}

void THWMouse::hide()
{
    asm push ax;
    asm push es;
       
    if( buttonCount != 0 )
        {
        _AX = 2;
        geninterrupt( 0x33 );
        }

    asm pop es;
    asm pop ax;
}

#pragma warn .asc

void THWMouse::setRange( ushort rx, ushort ry )
{
    if( buttonCount != 0 )
        {
        _DX = rx;
        _DX <<= 3;
        _CX = 0;
        _AX = 7;
        geninterrupt( 0x33 );

        _DX = ry;
        _DX <<= 3;
        _CX = 0;
        _AX = 8;
        geninterrupt( 0x33 );
        }
}

void THWMouse::getEvent( MouseEventType& me )
{
    _AX = 3;
    geninterrupt( 0x33 );
    _AL = _BL;
    me.buttons = _AL;
    me.where.x = _CX >> 3;
    me.where.y = _DX >> 3;
    me.doubleClick = False;
}

void THWMouse::registerHandler( unsigned mask, void (far *func)() )
{
    if( !present() )
        return;

#if defined( ProtectVersion )
    _AX = 20;
#else
    _AX = 12;
#endif
    _CX = mask;
    _DX = FP_OFF( func );
    _ES = FP_SEG( func );
    geninterrupt( 0x33 );
    handlerInstalled = True;
}

TMouse::TMouse()
{
    show();
}

TMouse::~TMouse()
{
    hide();
}


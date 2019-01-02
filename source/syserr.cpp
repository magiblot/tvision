/*------------------------------------------------------------*/
/* filename -       syserr.cpp                                */
/*                                                            */
/* function(s)                                                */
/*          TSystemError member functions                     */
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

#pragma inline

#define Uses_TDrawBuffer
#define Uses_TSystemError
#define Uses_TScreen
#include <tv.h>

#if !defined( __DOS_H )
#include <Dos.h>
#endif  // __DOS_H

#if !defined( __CONIO_H )
#include <conio.h>
#endif  // __CONIO_H

#if !defined( __STDIO_H )
#include <stdio.h>
#endif  // __STDIO_H

#if !defined( __STRING_H )
#include <String.h>
#endif  // __STRING_H

#if !defined( __MATH_H )
#include <Math.h>
#endif  // __MATH_H

Boolean near TSystemError::ctrlBreakHit = False;
short ( far * near TSystemError::sysErrorFunc )( short, uchar ) = &TSystemError::sysErr;
ushort near TSystemError::sysColorAttr = 0x4E4F;
ushort near TSystemError::sysMonoAttr = 0x7070;
Boolean near TSystemError::saveCtrlBreak = False;
Boolean near TSystemError::sysErrActive = False;
Boolean near TSystemError::inIDE = False;

const SecretWord = 1495;
const productID  =  136;

static void checkIDE()
{
    Int11trap trap;
    _BX = SecretWord;
    _AX = SecretWord;
    geninterrupt( 0x12 );
}

TSystemError::TSystemError()
{
    inIDE = False;
    checkIDE();
    resume();
}

TSystemError::~TSystemError()
{
    suspend();
}

#pragma warn -wasc

static int getakey()
{
    asm {
        MOV AH,1;
        INT 16h;
        JNZ keyWaiting;
        };
    return 0;

keyWaiting:

    asm {
        MOV AH,0;
        INT 16h;
        };
    return _AX;
}

#pragma warn .wasc

ushort TSystemError::selectKey()
{
    ushort crsrType = TScreen::getCursorType();

    TScreen::setCursorType( 0x2000 );

    while( getakey() )
        ;
    
    int ch = getakey() & 0xFF;
    while( ch != 13 && ch != 27 )
        ch = getakey() & 0xFF;

    TScreen::setCursorType( crsrType );
    return ch == 27;
}

short TSystemError::sysErr( short errorCode, uchar drive )
{
    ushort c = ( (TScreen::screenMode & 0x00fF) != TDisplay::smMono  ) ?
                                        sysColorAttr : sysMonoAttr;
    char s[ 63 ];
    TDrawBuffer b;

    sprintf( s, errorString[ errorCode ], drive + 'a' );

    b.moveChar( 0, ' ', c, 80);
    b.moveCStr( 1, s, c);
    b.moveCStr( 79-cstrlen(sRetryOrCancel), sRetryOrCancel, c);
    swapStatusLine(b);
    int res = selectKey();
    swapStatusLine(b);
    return res;
}

Int11trap::Int11trap()
{
    oldHandler = getvect( 0x11 );
    setvect( 0x11, &Int11trap::handler );
}

Int11trap::~Int11trap()
{
    setvect( 0x11, oldHandler );
}

void interrupt (far * near Int11trap::oldHandler)(...) = 0;

#pragma warn -eas

void interrupt Int11trap::handler(...)
{
    if( _AX == SecretWord && _BX == productID )
        TSystemError::inIDE++;
    oldHandler();
}

#pragma warn .eas

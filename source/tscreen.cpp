/*------------------------------------------------------------*/
/* filename -       tscreen.cpp                               */
/*                                                            */
/* function(s)                                                */
/*                  TScreen member functions                  */
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

#define Uses_TScreen
#define Uses_TEvent
#include <tv.h>

#if !defined( __DOS_H )
#include <dos.h>
#endif  // __DOS_H

#ifdef PROTECT
ushort far * near TDisplay::equipment = (ushort far *)MK_FP( biosSeg, 0x10 );
uchar far * near TDisplay::crtInfo = (uchar far *)MK_FP( biosSeg, 0x87 );
uchar far * near TDisplay::crtRows = (uchar far *)MK_FP( biosSeg, 0x84 );
#else
ushort far * near TDisplay::equipment = (ushort far *)MK_FP( 0x40, 0x10 );
uchar far * near TDisplay::crtInfo = (uchar far *)MK_FP( 0x40, 0x87 );
uchar far * near TDisplay::crtRows = (uchar far *)MK_FP( 0x40, 0x84 );
#endif

ushort near TScreen::startupMode = 0xFFFF;
ushort near TScreen::startupCursor = 0;
ushort near TScreen::screenMode = 0;
uchar near TScreen::screenWidth = 0;
uchar near TScreen::screenHeight = 0;
Boolean near TScreen::hiResScreen = False;
Boolean near TScreen::checkSnow = True;
uchar far * near TScreen::screenBuffer = 0;
ushort near TScreen::cursorLines = 0;

#ifdef PROTECT

extern "C"
    extern char _protected;

#pragma warn -asc

static ushort protGetRows(void)
{
    _AX = 0x1130;
    _BH = 0;
    _DL = 0;

    if (_protected)
    {
    asm {
        PUSH ES
        PUSH DS
        PUSH DI
        PUSH SI
        PUSHF
        PUSH DX
        PUSH CX
        PUSH BX
        PUSH AX

        MOV  DX,SP
        PUSH SS
        POP  DS     /* DS:DX points to the MSB */
        MOV  BX, 8      /* only AX - DX can be changed */
        MOV  AX, 0E310h /* call int 10H in real mode */
        INT  21h

        POP  AX
        POP  BX
        POP  CX
        POP  CX
        POPF        /* flags and below were not modified by E3 */
        POP  SI
        POP  DI
        POP  DS
        POP  ES
    }
    }
    else
    {
    asm INT 10h
    }
    if (_DL == 0)
        _DL = 24;
    return _DL + 1;
}

#pragma warn .asc

#endif

ushort TDisplay::getCursorType()
{
    _AH = 3;
    _BH = 0;
    videoInt();
    return _CX;
}

void TDisplay::setCursorType( ushort ct )
{
    _AH = 1;
    _CX = ct;
    videoInt();
}

void TDisplay::clearScreen( uchar w, uchar h )
{
    _BH = 0x07;
    _CX = 0;
    _DL = w;
    _DH = h - 1;
    _AX = 0x0600;
    videoInt();
}

#pragma warn -asc

void TDisplay::videoInt()
{
    asm {
        PUSH    BP
        PUSH    ES
        INT     10h
        POP     ES
        POP     BP
    }
}

#pragma warn .asc

ushort TDisplay::getRows()
{
#ifdef PROTECT
    return protGetRows();
#else
    _AX = 0x1130;
    _BH = 0;
    _DL = 0;
    videoInt();
    if( _DL == 0 )
        _DL = 24;
    return _DL + 1;
#endif
}

ushort TDisplay::getCols()
{
    _AH = 0x0F;
    videoInt();
    return _AH;
}

ushort TDisplay::getCrtMode()
{
    _AH = 0x0F;
    videoInt();
    ushort mode = _AL;
    if( getRows() > 25 )
        mode |= smFont8x8;
    return mode;
}

void TDisplay::setCrtMode( ushort mode )
{
    *equipment &= 0xFFCF;
    *equipment |= (mode == smMono) ? 0x30 : 0x20;
    *crtInfo &= 0x00FE;

   
    _AH = 0;
    _AL = mode;
    videoInt();


    if( (mode & smFont8x8) != 0 )
        {
        _AX = 0x1112;
        _BL = 0;
        videoInt();

        if( getRows() > 25 )
            {
            *crtInfo |= 1;

            _AH = 1;
            _CX = 0x0607;
            videoInt();

            _AH = 0x12;
            _BL = 0x20;
            videoInt();
            }
        }
}

TScreen::TScreen()
{
    startupMode = getCrtMode();
    startupCursor = getCursorType();
    setCrtData();
}

void TScreen::resume()
{
    startupMode = getCrtMode();
    startupCursor = getCursorType();
    if (screenMode != startupMode)
       setCrtMode( screenMode );
    setCrtData();
}

TScreen::~TScreen()
{
    suspend();
}

void TScreen::suspend()
{
    if( startupMode != screenMode )
        setCrtMode( startupMode );
    clearScreen();
    setCursorType( startupCursor );
}

ushort TScreen::fixCrtMode( ushort mode )
{
    _AX = mode;
    if( _AL != smMono && _AL != smCO80 && _AL != smBW80 )
        _AL = smCO80;
    return _AX;
}

void TScreen::setCrtData()
{
    screenMode = getCrtMode();
    screenWidth = getCols();
    screenHeight = getRows();
    hiResScreen = Boolean(screenHeight > 25);

    if( screenMode == smMono )
        {
#ifdef PROTECT
    screenBuffer = (uchar far *)MK_FP( monoSeg, 0 );
#else
    screenBuffer = (uchar far *)MK_FP( 0xB000, 0 );
#endif
        checkSnow = False;
        }
    else
        {
#ifdef PROTECT
    screenBuffer = (uchar far *)MK_FP( colrSeg, 0 );
#else
    screenBuffer = (uchar far *)MK_FP( 0xB800, 0 );
#endif
        if( hiResScreen )
            checkSnow = False;
        }

    cursorLines = getCursorType();
    setCursorType( 0x2000 );

}

void TScreen::clearScreen()
{
    TDisplay::clearScreen( screenWidth, screenHeight );
}

void TScreen::setVideoMode( ushort mode )
{
    setCrtMode( fixCrtMode( mode ) );
    setCrtData();
}

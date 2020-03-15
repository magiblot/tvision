/*-------------------------------------------------------------*/
/* filename -       textview.cpp                               */
/*                                                             */
/* function(s)                                                 */
/*                  TTerminal and TTextDevice member functions */
/*-------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TTextDevice
#define Uses_TTerminal
#define Uses_otstream
#include <tvision/tv.h>

#if !defined( __STRING_H )
#include <string.h>
#endif  // __STRING_H

TTextDevice::TTextDevice( const TRect& bounds,
                          TScrollBar *aHScrollBar,
                          TScrollBar *aVScrollBar,
                          ushort aBufSize ) :
    TScroller(bounds,aHScrollBar,aVScrollBar)
{
    if( aBufSize )
        {
        char *buffer = new char[aBufSize];
        setp( buffer, buffer + aBufSize );
        }
    else
        setp( 0, 0 );
}

TTextDevice::~TTextDevice()
{
    delete[] pbase();
}

int TTextDevice::overflow( int c )
{
    if( c != EOF )
        {
        if( pptr() > pbase() )
            {
            sync();
            sputc(c);
            }
        else
            {
            char b = c;
            do_sputn( &b, 1 );
            }
        }
    return 1;
}

int TTextDevice::sync()
{
    if( pptr() > pbase() )
        {
        do_sputn( pbase(), pptr() - pbase() );
        setp( pbase(), epptr() );
        }
    return 0;
}

TTerminal::TTerminal( const TRect& bounds,
                      TScrollBar *aHScrollBar,
                      TScrollBar *aVScrollBar,
                      ushort aBufSize ) :
    TTextDevice(bounds, aHScrollBar, aVScrollBar),
    queFront( 0 ),
    queBack( 0 )
{
    growMode = gfGrowHiX + gfGrowHiY;
    bufSize = min( 32000U, aBufSize );
    buffer = new char[ bufSize ];
    setLimit( 0, 1 );
    setCursor( 0, 0 );
    showCursor();
}


TTerminal::~TTerminal()
{
    delete[] buffer;
}

void TTerminal::bufDec( ushort& val )
{
    if (val == 0)
        val = bufSize - 1;
    else
        val--;
}

void TTerminal::bufInc( ushort& val )
{
    if( ++val >= bufSize )
        val = 0;
}

Boolean TTerminal::canInsert( ushort amount )
{
    long T = (queFront < queBack) ?
        ( queFront +  amount ) :
        ( long(queFront) - bufSize + amount);   // cast needed so we get
                                                // signed comparison
    return Boolean( queBack > T );
}

void TTerminal::draw()
{
    short  i;
    ushort begLine, endLine;
    char s[256];
    ushort bottomLine;

    bottomLine = size.y + delta.y;
    if( limit.y > bottomLine )
        {
        endLine = prevLines( queFront, limit.y - bottomLine );
        bufDec( endLine );
        }
    else
        endLine = queFront;

    if( limit.y > size.y )
        i = size.y - 1;
    else
        {
        for( i = limit.y; i <= size.y - 1; i++ )
            writeChar(0, i, ' ', 1, size.x);
        i =  limit.y -  1;
        }

    for( ; i >= 0; i-- )
        {
        begLine = prevLines(endLine, 1);
        if (endLine >= begLine)
            {
            int T = int( endLine - begLine );
            memcpy( s, &buffer[begLine], T );
            s[T] = EOS;
            }
        else
            {
            int T = int( bufSize - begLine);
            memcpy( s, &buffer[begLine], T );
            memcpy( s+T, buffer, endLine );
            s[T+endLine] = EOS;
            }
        int l = strlen(s);
        if( delta.x >= l )
            *s = EOS;
        else
            memmove( s, &s[delta.x], l );

        writeStr( 0, i, s, 1 );
        writeChar( strlen(s), i, ' ', 1, size.x );
        endLine = begLine;
        bufDec( endLine );
        }
}

ushort TTerminal::nextLine( ushort pos )
{
    if( pos != queFront )
        {
        while( buffer[pos] != '\n' && pos != queFront )
            bufInc(pos);
        if( pos != queFront )
            bufInc( pos );
        }
    return pos;
}

int TTerminal::do_sputn( const char *s, int count )
{
    ushort screenLines = limit.y;
    ushort i;
    for( i = 0; i < count; i++ )
        if( s[i] == '\n' )
            screenLines++;

    while( !canInsert( count ) )
        {
        queBack = nextLine( queBack );
        screenLines--;
        }

    if( queFront + count >= bufSize )
        {
        i = bufSize - queFront;
        memcpy( &buffer[queFront], s, i );
        memcpy( buffer, &s[i], count - i );
        queFront = count - i;
        }
    else
        {
        memcpy( &buffer[queFront], s, count );
        queFront += count;
        }

    // drawLock: avoid redundant calls to drawView()
    drawLock++;
    setLimit( limit.x, screenLines );
    scrollTo( 0, screenLines + 1 );
    drawLock--;

    i = prevLines( queFront, 1 );
    if( i <= queFront )
        i = queFront - i;
    else
        i = bufSize - (i - queFront);
    setCursor( i, screenLines - delta.y - 1 );
    drawView();
    return count;
}

Boolean TTerminal::queEmpty()
{
    return Boolean( queBack == queFront );
}

otstream::otstream( TTerminal *tt )
{
    ios::init( tt );
}

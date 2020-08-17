/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   UTIL.H                                                                */
/*                                                                         */
/*   defines various utility functions used throughout Turbo Vision        */
/*                                                                         */
/* ------------------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#if !defined( __UTIL_H )
#define __UTIL_H

#include <stddef.h>

inline int min( int a, int b )
{
    return (a>b) ? b : a;
}

inline int max( int a, int b )
{
    return (a<b) ? b : a;
}

void fexpand( char * );

char hotKey( const char *s );
ushort ctrlToArrow( ushort );
char getAltChar( ushort keyCode );
ushort getAltCode( char ch );
char getCtrlChar(ushort);
ushort getCtrlCode(uchar);

ushort historyCount( uchar id );
const char *historyStr( uchar id, int index );
void historyAdd( uchar id, const char * );

class TStringView;
int cstrlen( const char * );
int cstrlen( TStringView );
int strwidth( const char * );
int strwidth( TStringView );

class _FAR TView;
void *message( TView *receiver, ushort what, ushort command, void *infoPtr );
Boolean lowMemory();

char *newStr( const char * );

Boolean driveValid( char drive );

Boolean isDir( const char *str );

Boolean pathValid( const char *path );

Boolean validFileName( const char *fileName );

void getCurDir( char *dir );

Boolean isWild( const char *f );

char *strnzcpy( char *dest, const char *src, size_t n );

unsigned int fast_utoa( uint value, char *buffer );

#ifndef __BORLANDC__

int strnicmp( const char *s1, const char *s2, size_t maxlen );
char *strupr(char *s);
char *itoa( int value, char *buffer, int radix );
char *ltoa( long value, char *buffer, int radix );
char *ultoa( ulong value, char *buffer, int radix );

#endif

#endif  // __UTIL_H

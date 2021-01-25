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

inline constexpr int min( int a, int b )
{
    return a < b ? a : b;
}

inline constexpr int max( int a, int b )
{
    return a > b ? a : b;
}

#if !defined( __MINMAX_DEFINED ) // Also defined in Borland C++'s stdlib.h.
#define __MINMAX_DEFINED
template <class T>
inline constexpr const T& min( const T& a, const T& b )
{
    return a < b ? a : b;
}

template <class T>
inline constexpr const T& max( const T& a, const T& b )
{
    return a > b ? a : b;
}
#endif  // __MINMAX_DEFINED

void fexpand( char *rpath );
void fexpand( char *rpath, const char *relativeTo );

char hotKey( const char *s );
ushort ctrlToArrow( ushort );
char getAltChar( ushort keyCode );
ushort getAltCode( char ch );
char getCtrlChar(ushort);
ushort getCtrlCode(uchar);

ushort historyCount( uchar id );
const char *historyStr( uchar id, int index );
void historyAdd( uchar id, TStringView );

int cstrlen( TStringView );
int strwidth( TStringView );

class _FAR TView;
void *message( TView *receiver, ushort what, ushort command, void *infoPtr );

class _FAR TPoint;
class _FAR TGroup;
class _FAR TMenu;
class _FAR TMenuItem;
ushort popupMenu(TPoint where, TMenuItem &aMenu, TGroup * = 0);

Boolean lowMemory();

char *newStr( TStringView );

Boolean driveValid( char drive );
Boolean isDir( const char *str );
Boolean pathValid( const char *path );
Boolean validFileName( const char *fileName );
void getCurDir( char *dir, char drive=-1 );
Boolean getHomeDir( char *drive, char *dir );
Boolean isWild( const char *f );

size_t strnzcpy( char *dest, TStringView src, size_t n );
size_t strnzcat( char *dest, TStringView src, size_t n );

#if !defined( __BORLANDC__ ) && !defined( _WIN32 )

int stricmp( const char *s1, const char *s2 );
int strnicmp( const char *s1, const char *s2, size_t maxlen );
char *strupr(char *s);
char *itoa( int value, char *buffer, int radix );
char *ltoa( long value, char *buffer, int radix );
char *ultoa( ulong value, char *buffer, int radix );

#endif

#endif  // __UTIL_H

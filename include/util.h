/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   UTIL.H                                                                */
/*                                                                         */
/*   Copyright (c) Borland International 1991                              */
/*   All Rights Reserved.                                                  */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if !defined( __UTIL_H )
#define __UTIL_H

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

ushort historyCount( uchar id );
const char *historyStr( uchar id, int index );
void historyAdd( uchar id, const char * );

int cstrlen( const char * );

class far TView;
void *message( TView *receiver, ushort what, ushort command, void *infoPtr );
Boolean lowMemory();

char *newStr( const char * );

Boolean driveValid( char drive );

Boolean isDir( const char *str );

Boolean pathValid( const char *path );

Boolean validFileName( const char *fileName );

void getCurDir( char *dir );

Boolean isWild( const char *f );


#endif  // __UTIL_H

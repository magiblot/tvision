/*------------------------------------------------------------*/
/* filename - histlist.cpp                                    */
/*                                                            */
/* function(s)                                                */
/*          startId                                           */
/*          historyCount                                      */
/*          historyAdd                                        */
/*          historyStr                                        */
/*          clearHistory                                      */
/*          initHistory                                       */
/*          doneHistory                                       */
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

#include <tv.h>

#if !defined( __STRING_H )
#include <String.h>
#endif  // __STRING_H

#if !defined( __MEM_H )
#include <Mem.h>
#endif  // __MEM_H

#if !defined( __DOS_H )
#include <Dos.h>
#endif  // __DOS_H

#if !defined( __STDLIB_H )
#include <StdLib.h>
#endif  // __STDLIB_H

class HistRec
{
    
public:

    HistRec( uchar nId, const char *nStr );

    void *operator new( size_t );
    void *operator new( size_t, HistRec * );

    uchar id;
    uchar len;
    char str[1];

};

void *HistRec::operator new( size_t, HistRec *hr )
{
    return hr;
}

void *HistRec::operator new( size_t )
{
    abort();
    return 0;
}

inline HistRec::HistRec( uchar nId, const char *nStr ) :
    id( nId ),
    len( strlen( nStr ) + 3 )
{
    strcpy( str, nStr );
}


inline HistRec *advance( HistRec *ptr, size_t s )
{
    return (HistRec *)((char *)ptr + s);
}

inline HistRec *backup( HistRec *ptr, size_t s )
{
    return (HistRec *)((char *)ptr - s);
}

inline HistRec *next( HistRec *ptr )
{
    return advance( ptr, ptr->len );
}

inline HistRec *prev( HistRec *ptr )
{
    return backup( ptr, ptr->len );
}

ushort historySize = 1024;  // initial size of history block

static uchar curId;
static HistRec *curRec;
static HistRec *historyBlock;
static HistRec *lastRec;

void advanceStringPointer()
{
    curRec = next( curRec );
    while( curRec < lastRec && curRec->id != curId )
        curRec = next( curRec );
    if( curRec >= lastRec )
        curRec = 0;
}

void deleteString()
{
    size_t len = curRec->len;
    movmem( next( curRec ), curRec, size_t( (char *)lastRec - (char *)curRec ) );
    lastRec = backup( lastRec, len );
}

void insertString( uchar id, const char *str )
{
    ushort len = strlen( str ) + 3;
    while( len > historySize - ( (char *)lastRec - (char *)historyBlock ) )
        {
        ushort firstLen = historyBlock->len;
        HistRec *dst = historyBlock;
        HistRec *src = next( historyBlock );
        movmem( src, dst,  size_t( (char *)lastRec - (char *)src ) );
        lastRec = backup( lastRec, firstLen );
        }
    new( lastRec ) HistRec( id, str );
    lastRec = next( lastRec );
}

void startId( uchar id )
{
    curId = id;
    curRec = historyBlock;
}

ushort historyCount( uchar id )
{
    startId( id );
    ushort count =  0;
    advanceStringPointer();
    while( curRec != 0 )
        {
        count++;
        advanceStringPointer();
        }
    return count;
}

void historyAdd( uchar id, const char *str )
{
    if( str[0] == EOS )
        return;
    startId( id );
    advanceStringPointer();
    while( curRec != 0 )
        {
        if( strcmp( str, curRec->str ) == 0 )
            deleteString();
        advanceStringPointer();
        }
    insertString( id, str );
}

const char *historyStr( uchar id, int index )
{
    startId( id );
    for( short i = 0; i <= index; i++ )
        advanceStringPointer();
    if( curRec != 0 )
        return curRec->str;
    else
        return 0;
}

void clearHistory()
{
    new (historyBlock) HistRec( 0, "" );
    lastRec = next( historyBlock );
}

void initHistory()
{
    historyBlock = (HistRec *) new char[historySize];
    clearHistory();
}

void doneHistory()
{
    delete historyBlock;
}

/*------------------------------------------------------------*/
/* filename -       tstrlist.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TStrListMaker member functions            */
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

#define Uses_TStringList
#define Uses_TStrIndexRec
#define Uses_TStrListMaker
#define Uses_opstream
#define Uses_ipstream
#include <tv.h>

#if !defined( __STRING_H )
#include <String.h>
#endif  // __STRING_H

#if !defined( __MEM_H )
#include <Mem.h>
#endif  // __MEM_H

const MAXKEYS = 16;

TStrIndexRec::TStrIndexRec() :
    count(0)
{
}

TStrListMaker::TStrListMaker( ushort aStrSize, ushort aIndexSize ) :
    strSize( aStrSize ),
    indexSize( aIndexSize ),
    strings( new char[aStrSize] ),
    index( new TStrIndexRec[aIndexSize] ),
    strPos( 0 ),
    indexPos( 0 )
{
}

#pragma warn -dsz

TStrListMaker::~TStrListMaker()
{
    delete strings;
    delete [indexSize] index;
}

#pragma warn .dsz

void TStrListMaker::closeCurrent()
{
    if( cur.count != 0 )
        {
        index[indexPos++] = cur;
        cur.count = 0;
        }
}

void TStrListMaker::put( ushort key, char *str )
{
    if( cur.count == MAXKEYS || key != cur.key + cur.count )
        closeCurrent();
    if( cur.count == 0 )
        {
        cur.key = key;
        cur.offset = strPos;
        }
    int len = strlen( str );
    strings[strPos] = len;
    movmem( str, strings+strPos+1, len );
    strPos += len+1;
    cur.count++;
}

TStringList::TStringList( StreamableInit ) :
    indexSize(0),
    index(0),
    basePos(0)
{
}

#pragma warn -dsz

TStringList::~TStringList()
{
    delete [indexSize] index;
}

#pragma warn .dsz

void TStringList::get( char *dest, ushort key )
{
    if( indexSize == 0 )
        {
        *dest = EOS;
        return;
        }

    TStrIndexRec *cur = index;
    while( cur->key + MAXKEYS < key && cur - index < indexSize )
        cur++;
    if( cur->key + MAXKEYS < key )
        {
        *dest = EOS;
        return;
        }
    ip->seekg( basePos + cur->offset );
    int count = key - cur->key;
    do  {
        uchar sz = ip->readByte();
        ip->readBytes( dest, sz );
        dest[sz] = EOS;
        } while( count-- > 0 );
}

void TStrListMaker::write( opstream& os )
{
    closeCurrent();
    os << strPos;
    os.writeBytes( strings, strPos );
    os << indexPos;
    os.writeBytes( index, indexPos * sizeof( TStrIndexRec ) );
}

void *TStringList::read( ipstream& is )
{
    ip = &is;

    ushort strSize;
    is >> strSize;

    basePos = is.tellg();
    is.seekg( basePos + strSize );
    is >> indexSize;
    index = new TStrIndexRec[indexSize];
    is.readBytes( index, indexSize * sizeof( TStrIndexRec ) );
    return this;
}

TStreamable *TStringList::build()
{
    return new TStringList( streamableInit );
}



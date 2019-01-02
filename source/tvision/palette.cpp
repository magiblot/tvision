/*------------------------------------------------------------*/
/* filename -       palette.cpp                               */
/*                                                            */
/* function(s)                                                */
/*                  TPalette member functions                 */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TPalette
#include <tvision\tv.h>

#if !defined( __MEM_H )
#include <Mem.h>
#endif  // __MEM_H


TPalette::TPalette( const char* d, ushort len ) :
    data( new uchar[ len+1 ] )
{
    data[0] = len;
    memcpy( data+1, d, len );
}

TPalette::TPalette( const TPalette& tp ) :
    data( new uchar[ tp.data[0] + 1 ] )
{
    memcpy( data, tp.data, tp.data[0] + 1 );
}

TPalette::~TPalette()
{
    delete data;
}

TPalette& TPalette::operator = ( const TPalette& tp )
{
    if( data != tp.data )
        {
        if( data[0] != tp.data[0] )
            {
            delete data;
            data = new uchar[ tp.data[0] + 1 ];
            data[0] = tp.data[0];
            }
        memcpy( data+1, tp.data+1, data[0] );
        }
    return *this;
}

uchar& TPalette::operator[]( int index ) const
{
    return data[index];
}

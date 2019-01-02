/*------------------------------------------------------------*/
/* filename -       tstrcoll.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TStringCollection member functions        */
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

#define Uses_TStringCollection
#define Uses_opstream
#define Uses_ipstream
#include <tv.h>

#if !defined( __STRING_H )
#include <String.h>
#endif  // __STRING_H

TStringCollection::TStringCollection( short aLimit, short aDelta ) :
    TSortedCollection(aLimit, aDelta)
{
}

int TStringCollection::compare( void *key1, void *key2 )
{
    return strcmp( (char *)key1, (char *)key2 );
}

void TStringCollection::freeItem( void* item )
{
    delete item;
}

TStreamable *TStringCollection::build()
{
    return new TStringCollection( streamableInit );
}

void TStringCollection::writeItem( void *obj, opstream& os )
{
    os.writeString( (const char *)obj );
}

void *TStringCollection::readItem( ipstream& is )
{
    return is.readString();
}

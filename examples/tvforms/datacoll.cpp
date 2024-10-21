/*-------------------------------------------------------*/
/*                                                       */
/*   Turbo Vision Forms Demo                             */
/*                                                       */
/*   Datacoll.cpp: Support source file for TVFORMS demo  */
/*-------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TStreamableClass
#define Uses_TStringCollection
#define Uses_ipstream
#define Uses_opstream
#include <tvision/tv.h>
__link( RStringCollection )

#if !defined( __DATACOLL_H )
#include "datacoll.h"
#endif  // __DATACOLL_H

#if !defined( __STRING_H )
#include <string.h>
#endif  // __STRING_H

#if !defined( __STDLIB_H )
#include <stdlib.h>
#endif  // __STDLIB_H

const char * const TDataCollection::name = "TDataCollection";

void TDataCollection::write( opstream& os )
{
    os << itemSize;
    TStringCollection::write(os);
    int temp = int(keyType);
    os << temp;
}

void *TDataCollection::read( ipstream& is )
{
    is >> itemSize;
    TStringCollection::read( is );
    int temp;
    is >> temp;
    keyType = KeyTypes(temp);
    status = 0;
    return this;
}

TStreamable *TDataCollection::build()
{
    return new TDataCollection( streamableInit );
}

void TDataCollection::writeItem( void *obj, opstream& os )
{
    os.writeBytes( obj, itemSize );
}

void *TDataCollection::readItem( ipstream& is )
{
    void *obj;

    obj = malloc(itemSize);
    is.readBytes(obj, itemSize);
    return obj;
}

TStreamableClass RDataCollection( TDataCollection::name,
                                  TDataCollection::build,
                                  __DELTA(TDataCollection)
                                );

TDataCollection::TDataCollection( short aLimit, short aDelta,
                                  int anItemSize,
                                  KeyTypes aKeyType) :
    TStringCollection( aLimit, aDelta ),
    itemSize( anItemSize ),
    keyType( aKeyType )
{
}

int TDataCollection::compare( void *key1, void *key2 )
{

    if (keyType == stringKey)
        return stricmp((char*)key1, (char*) key2);
    else
        {
        if (!key1 || !key2 || *(int32_t *)key1 < *(int32_t *)key2)
            return -1;
        else if (*(int32_t *)key1 == *(int32_t *)key2)
            return 0;
        else
            return 1;
        }
}

void TDataCollection::error( int code )
// Save error status instead of giving a runtime error
{
    status = code;
}

void TDataCollection::freeItem( void *item )
{
    ::free(item);
}

/*------------------------------------------------------------*/
/* filename -       tcollect.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TNSCollection member functions            */
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

#define Uses_TNSCollection
#define Uses_opstream
#define Uses_ipstream
#define Uses_TCollection
#include <tv.h>

#if !defined( __STDLIB_H )
#include <StdLib.h>
#endif  // __STDLIB_H

#if !defined( __MEM_H )
#include <Mem.h>
#endif  // __MEM_H


TNSCollection::TNSCollection( ccIndex aLimit, ccIndex aDelta ) :
    count( 0 ),
    items( 0 ),
    limit( 0 ),
    delta( aDelta ),
    shouldDelete( True )
{
    setLimit( aLimit );
}

TNSCollection::TNSCollection() :
    count( 0 ),
    items( 0 ),
    limit( 0 ),
    delta( 0 ),
    shouldDelete( True )
{
    items = 0;
}

TNSCollection::~TNSCollection()
{
    delete items;
}

void TNSCollection::shutDown()
{
    if( shouldDelete )
        freeAll();
    setLimit(0);
    TObject::shutDown();
}

void *TNSCollection::at( ccIndex index )
{
    if( index < 0 || index >= count )
        error(1,0);
    return items[index];
}

void TNSCollection::atRemove( ccIndex index )
{
    if( index < 0 || index >= count )
        error(1,0);

    count--;
    memmove( &items[index], &items[index+1], (count-index)*sizeof(void *) );
}

void TNSCollection::atFree( ccIndex index )
{
    void *item = at( index );
    atRemove( index );
    freeItem( item );
}

void TNSCollection::atInsert(ccIndex index, void *item)
{
    if( index < 0 )
        error(1,0);
    if( count == limit )
        setLimit(count + delta);

    memmove( &items[index+1], &items[index], (count-index)*sizeof(void *) );
    count++;

    items[index] = item;
}

void TNSCollection::atPut( ccIndex index, void *item )
{
    if( index >= count )
        error(1,0);

    items[index] = item;
}

void TNSCollection::remove( void *item )
{
    atRemove( indexOf(item) );
}

void TNSCollection::removeAll()
{
    count = 0;
}

void TNSCollection::error( ccIndex code, ccIndex )
{
    exit(212 - code);
}

void *TNSCollection::firstThat( ccTestFunc Test, void *arg )
{
    for( ccIndex i = 0; i < count; i++ )
        {
        if( Test( items[i], arg ) == True )
            return items[i];
        }
    return 0;
}

void *TNSCollection::lastThat( ccTestFunc Test, void *arg )
{
    for( ccIndex i = count; i > 0; i-- )
        {
        if( Test( items[i-1], arg ) == True )
            return items[i-1];
        }
    return 0;
}

void TNSCollection::forEach( ccAppFunc action, void *arg )
{
    for( ccIndex i = 0; i < count; i++ )
        action( items[i], arg );
}

void TNSCollection::free( void *item )
{
    remove( item );
    freeItem( item );
}

void TNSCollection::freeAll()
{
    for( ccIndex i =  0; i < count; i++ )
        freeItem( at(i) );
    count = 0;
}

void TNSCollection::freeItem( void *item )
{
    delete item;
}

#pragma warn -rvl
ccIndex TNSCollection::indexOf(void *item)
{
    for( ccIndex i = 0; i < count; i++ )
        if( item == items[i] )
            return i;

    error(1,0);
}
#pragma warn .rvl

ccIndex TNSCollection::insert( void *item )
{
    ccIndex loc = count;
    atInsert( count, item );
    return loc;
}

void TNSCollection::pack()
{
    void **curDst = items;
    void **curSrc = items;
    void **last = items + count;
    while( curSrc < last )
        {
        if( *curSrc != 0 )
            *curDst++ = *curSrc;
        *curSrc++;
        }
}

void TNSCollection::setLimit(ccIndex aLimit)
{
    if( aLimit < count )
        aLimit =  count;
    if( aLimit > maxCollectionSize)
        aLimit = maxCollectionSize;
    if( aLimit != limit )
        {
        void **aItems;
        if (aLimit == 0 )
            aItems = 0;
        else
            {
            aItems = new void *[aLimit];
            if( count !=  0 )
                memcpy( aItems, items, count*sizeof(void *) );
            }
        delete items;
        items =  aItems;
        limit =  aLimit;
        }
}

void TCollection::write( opstream& os )
{
    os << count << limit << delta;
    for( ccIndex idx = 0; idx < count; idx++ )
        writeItem( items[idx], os );
}

void *TCollection::read( ipstream& is )
{
    int limit;
    is >> count >> limit >> delta;
    limit = 0;
    setLimit(limit);
    for( ccIndex idx = 0; idx < count; idx++ )
        items[idx] = readItem( is );
    return this;
}


TCollection::TCollection( StreamableInit )
{
}

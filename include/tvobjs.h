/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   TVOBJS.H                                                              */
/*                                                                         */
/*   Copyright (c) Borland International 1991                              */
/*   All Rights Reserved.                                                  */
/*                                                                         */
/*   defines the classes TObject, TNSCollection, and TNSSortedCollection.  */
/*                                                                         */
/*   The NS variants of collections are Not Streamable.  These are         */
/*   needed for internal use in the stream manager.  There are             */
/*   streamable variants of each of these classes for use by the           */
/*   rest of the library.                                                  */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#pragma option -Vo-
#if defined( __BCOPT__ )
#pragma option -po-
#endif

#if defined( Uses_TObject ) && !defined( __TObject )
#define __TObject

#if !defined( __STDDEF_H )
#include <StdDef.h>
#endif  // __STDDEF_H

class TObject
{

public:

    virtual ~TObject();

    static void destroy( TObject * );
    virtual void shutDown();

private:

};

inline void TObject::destroy( TObject *o )
{
    if( o != 0 )
        o->shutDown();
    delete o;
}

#endif  // Uses_TObject

#if defined( Uses_TNSCollection ) && !defined( __TNSCollection )
#define __TNSCollection

class TNSCollection : public TObject
{

public:

    TNSCollection( ccIndex aLimit, ccIndex aDelta );
    ~TNSCollection();

    virtual void shutDown();

    void *at( ccIndex index );
    virtual ccIndex indexOf( void *item );

    void atFree( ccIndex index );
    void atRemove( ccIndex index );
    void remove( void *item );
    void removeAll();
    void free( void *item );
    void freeAll();

    void atInsert( ccIndex index, void *item );
    void atPut( ccIndex index, void *item );
    virtual ccIndex insert( void *item );

    static void error( ccIndex code, ccIndex info );

    void *firstThat( ccTestFunc Test, void *arg );
    void *lastThat( ccTestFunc Test, void *arg );
    void forEach( ccAppFunc action, void *arg );

    void pack();
    virtual void setLimit( ccIndex aLimit );

    ccIndex getCount()
        { return count; }

protected:

    TNSCollection();

    void **items;
    ccIndex count;
    ccIndex limit;
    ccIndex delta;
    Boolean shouldDelete;

private:

    virtual void freeItem( void *item );

};

#endif  // Uses_TNSCollection

#if defined( Uses_TNSSortedCollection ) && !defined( __TNSSortedCollection )
#define __TNSSortedCollection

class TNSSortedCollection: public virtual TNSCollection
{

public:

    TNSSortedCollection( ccIndex aLimit, ccIndex aDelta) :
        TNSCollection( aLimit, aDelta ), duplicates(False)
            { delta = aDelta; setLimit( aLimit ); }

    virtual Boolean search( void *key, ccIndex& index );

    virtual ccIndex indexOf( void *item );
    virtual ccIndex insert( void *item );

    Boolean duplicates;
    virtual void *keyOf( void *item );

protected:

    TNSSortedCollection() : duplicates(False) {}

private:

    virtual int compare( void *key1, void *key2 ) = 0;

};

#endif  // Uses_TNSSortedCollection

#pragma option -Vo.
#if defined( __BCOPT__ )
#pragma option -po.
#endif


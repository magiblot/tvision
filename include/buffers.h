/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   BUFFERS.H                                                             */
/*                                                                         */
/*   Copyright (c) Borland International 1991                              */
/*   All Rights Reserved.                                                  */
/*                                                                         */
/*   defines the functions getBufMem() and freeBufMem() for use            */
/*   in allocating and freeing viedo buffers                               */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#pragma option -Vo-
#if defined( __BCOPT__ )
#pragma option -po-
#endif

#if defined( Uses_TVMemMgr ) && !defined( __TVMemMgr )
#define __TVMemMgr

const DEFAULT_SAFETY_POOL_SIZE = 4096;

class TBufListEntry
{

private:

    TBufListEntry( void*& );
    ~TBufListEntry();

    void *operator new( size_t, size_t );
    void *operator new( size_t );
    void operator delete( void * );

    TBufListEntry *next;
    TBufListEntry *prev;
    void*& owner;

    static TBufListEntry *near bufList;
    static Boolean freeHead();

    friend class TVMemMgr;
    friend void *operator new( size_t );

};

class TVMemMgr
{

public:

    TVMemMgr();

    static void resizeSafetyPool( size_t = DEFAULT_SAFETY_POOL_SIZE );
    static int safetyPoolExhausted();

    static void allocateDiscardable( void *&, size_t );
    static void freeDiscardable( void * );

private:

    static void * near safetyPool;
    static size_t near safetyPoolSize;
    static int near inited;
    static int initMemMgr();

};

#endif  // Uses_TVMemMgr

#pragma option -Vo.
#if defined( __BCOPT__ )
#pragma option -po.
#endif



/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   BUFFERS.H                                                             */
/*                                                                         */
/*   defines the functions getBufMem() and freeBufMem() for use            */
/*   in allocating and freeing viedo buffers                               */
/*                                                                         */
/* ------------------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#pragma option -Vo-
#if defined( __BCOPT__ ) && !defined (__FLAT__)
#pragma option -po-
#endif

#if defined( Uses_TVMemMgr ) && !defined( __TVMemMgr )
#define __TVMemMgr

const int DEFAULT_SAFETY_POOL_SIZE = 4096;

class TBufListEntry
{

private:

    TBufListEntry( void*&, size_t sz ) noexcept;
#if __cplusplus >= 201103L
    TBufListEntry(const TBufListEntry &) = default;
#endif
    void destroy();

    void *operator new( size_t, size_t ) noexcept;
    void *operator new( size_t ) noexcept;
    void operator delete( void * ) noexcept;

    TBufListEntry *next;
    TBufListEntry *prev;
    void*& owner;
    size_t sz;

    static TBufListEntry *_NEAR bufList;
    static Boolean freeHead();

    friend class TVMemMgr;
    friend void *operator new( size_t );
    friend void * allocBlock( size_t );

};

class TVMemMgr
{

public:

    TVMemMgr();
    ~TVMemMgr();

    static void resizeSafetyPool( size_t = DEFAULT_SAFETY_POOL_SIZE );
    static int safetyPoolExhausted();

    static void allocateDiscardable( void *&, size_t );
    static void reallocateDiscardable( void *&, size_t );
    static void freeDiscardable( void * );

private:

    static void * _NEAR safetyPool;
    static size_t _NEAR safetyPoolSize;
    static int _NEAR inited;
    static int initMemMgr();

};

#endif  // Uses_TVMemMgr

#pragma option -Vo.
#if defined( __BCOPT__ ) && !defined (__FLAT__)
#pragma option -po.
#endif



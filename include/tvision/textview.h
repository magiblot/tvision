/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   TEXTVIEW.H                                                            */
/*                                                                         */
/*   defines the classes TTextDevice and TTerminal                         */
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

#if defined( Uses_TTextDevice ) && !defined( __TTextDevice )
#define __TTextDevice

#include <tvision/compat/iostream.h>
#pragma option -Vo-
#if defined( __BCOPT__ ) && !defined (__FLAT__)
#pragma option -po-
#endif

class _FAR TRect;
class _FAR TScrollBar;

class TTextDevice : public TScroller, public streambuf
{

public:

    TTextDevice( const TRect& bounds,
                 TScrollBar *aHScrollBar,
                 TScrollBar *aVScrollBar,
                 ushort aBufSize = 256
               );
    ~TTextDevice();

    virtual int do_sputn( const char *s, int count ) = 0;
    virtual int overflow( int = EOF );

protected:

    virtual int sync();

};

#endif  // Uses_TTextDevice

#if defined( Uses_TTerminal ) && !defined( __TTerminal )
#define __TTerminal

class _FAR TRect;
class _FAR TScrollBar;

class TTerminal: public TTextDevice
{

public:

    friend void genRefs();

    TTerminal( const TRect& bounds,
               TScrollBar *aHScrollBar,
               TScrollBar *aVScrollBar,
               ushort aBufSize
             );
    ~TTerminal();

    virtual int do_sputn( const char *s, int count );

    void bufInc( ushort& val );
    Boolean canInsert( ushort amount );
    short calcWidth();
    virtual void draw();
    ushort nextLine( ushort pos );
    ushort prevLines( ushort pos, ushort lines );
    Boolean queEmpty();

protected:

    ushort bufSize;
    char *buffer;
    ushort queFront, queBack;
    void bufDec(ushort& val);
};

#endif  // Uses_TTerminal

#if defined( Uses_otstream ) && !defined( __otstream )
#define __otstream

#include <tvision/compat/iostream.h>
#pragma option -Vo-
#if defined( __BCOPT__ ) && !defined (__FLAT__)
#pragma option -po-
#endif


class otstream : public ostream
{

public:

    otstream( TTerminal * );

};


#endif

#pragma option -Vo.
#if defined( __BCOPT__ ) && !defined (__FLAT__)
#pragma option -po.
#endif

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   DRAWBUF.H                                                             */
/*                                                                         */
/*   defines the class TDrawBuffer, which provides the high-level          */
/*   interface to the Screen Manager.                                      */
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

#if defined( Uses_TDrawBuffer ) && !defined( __TDrawBuffer )
#define __TDrawBuffer

#ifndef __BORLANDC__
#include <string_view>
#endif

class TDrawBuffer
{

    friend class TSystemError;
    friend class TView;
    friend void genRefs();
#ifdef __BORLANDC__
    typedef ushort data_t;
#else
    typedef TScreenCell data_t;
#endif

public:

    void moveChar( ushort indent, char c, ushort attr, ushort count );
    void moveStr( ushort indent, const char _FAR *str, ushort attrs );
    void moveCStr( ushort indent, const char _FAR *str, ushort attrs );
    void moveBuf( ushort indent, const void _FAR *source,
                  ushort attr, ushort count );

    void putAttribute( ushort indent, ushort attr );
    void putChar( ushort indent, ushort c );
    size_t length() const;

#ifndef __BORLANDC__
    // Multibyte compatible operations

    void moveStrEx( ushort indent, std::string_view str, ushort attrs );
    void moveCStrEx( ushort indent, std::string_view str, ushort attrs );
    void moveBufEx( ushort indent, TScreenCell *source, ushort attr, ushort count );
#endif

#ifdef __FLAT__
    TDrawBuffer();
    ~TDrawBuffer();
#endif

protected:

#ifdef __FLAT__
    size_t dataLength;
    data_t *data;
#else
    data_t data[maxViewWidth];
#endif

};

#define loByte(w)    (((uchar *)&w)[0])
#define hiByte(w)    (((uchar *)&w)[1])

inline void TDrawBuffer::putAttribute( ushort indent, ushort attr )
{
#ifdef __FLAT__
    if (indent < dataLength)
#endif
    {
#ifdef __BORLANDC__
        hiByte(data[indent]) = uchar(attr);
#else
        data[indent].Attr = uchar(attr);
#endif
    }
}

inline void TDrawBuffer::putChar( ushort indent, ushort c )
{
#ifdef __FLAT__
    if (indent < dataLength)
#endif
    {
#ifdef __BORLANDC__
        loByte(data[indent]) = uchar(c);
#else
        data[indent].Char = uchar(c);
        data[indent].extraWidth = 0;
#endif
    }
}

inline size_t TDrawBuffer::length() const
{
#ifdef __FLAT__
    return dataLength;
#else
    return maxViewWidth;
#endif
}

#endif  // Uses_TDrawBuffer

#pragma option -Vo.
#if defined( __BCOPT__ ) && !defined (__FLAT__)
#pragma option -po.
#endif


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

#if defined( __BORLANDC__ )
#pragma option -Vo-
#endif
#if defined( __BCOPT__ ) && !defined (__FLAT__)
#pragma option -po-
#endif

#if defined( Uses_TDrawBuffer ) && !defined( __TDrawBuffer )
#define __TDrawBuffer

class TDrawBuffer
{

    friend class TSystemError;
    friend class TView;
    friend void genRefs();

public:

    void moveChar( ushort indent, char c, TColorAttr attr, ushort count ) noexcept;
    ushort moveStr( ushort indent, TStringView str, TColorAttr attr ) noexcept;
    ushort moveStr( ushort indent, TStringView str, TColorAttr attr, ushort maxStrWidth, ushort strIndent = 0 ) noexcept;
    ushort moveCStr( ushort indent, TStringView str, TAttrPair attrs ) noexcept;
    ushort moveCStr( ushort indent, TStringView str, TAttrPair attrs, ushort maxStrWidth, ushort strIndent = 0 ) noexcept;
    void moveBuf( ushort indent, const void _FAR *source, TColorAttr attr, ushort count ) noexcept;

    void putAttribute( ushort indent, TColorAttr attr ) noexcept;
    void putChar( ushort indent, uchar c ) noexcept;

#if defined( __FLAT__ )
    TDrawBuffer() noexcept;
    ~TDrawBuffer();
#endif

protected:

#if defined( __FLAT__ )
    TScreenCell *data;
    size_t capacity;
#else
    TScreenCell data[maxViewWidth];
    enum { capacity = maxViewWidth };
#endif

};

inline void TDrawBuffer::putAttribute( ushort indent, TColorAttr attr ) noexcept
{
    if (indent < capacity)
        ::setAttr(data[indent], attr);
}

inline void TDrawBuffer::putChar( ushort indent, uchar c ) noexcept
{
    if (indent < capacity)
        ::setChar(data[indent], c);
}

#endif  // Uses_TDrawBuffer

#if defined( __BORLANDC__ )
#pragma option -Vo.
#endif
#if defined( __BCOPT__ ) && !defined (__FLAT__)
#pragma option -po.
#endif

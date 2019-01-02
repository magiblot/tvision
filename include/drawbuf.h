/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   DRAWBUF.H                                                             */
/*                                                                         */
/*   Copyright (c) Borland International 1991                              */
/*   All Rights Reserved.                                                  */
/*                                                                         */
/*   defines the class TDrawBuffer, which provides the high-level          */
/*   interface to the Screen Manager.                                      */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#pragma option -Vo-
#if defined( __BCOPT__ )
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

    void moveChar( ushort indent, char c, ushort attr, ushort count );
    void moveStr( ushort indent, const char far *str, ushort attrs );
    void moveCStr( ushort indent, const char far *str, ushort attrs );
    void moveBuf( ushort indent, const void far *source,
                  ushort attr, ushort count );

    void putAttribute( ushort indent, ushort attr );
    void putChar( ushort indent, ushort c );

protected:

    ushort data[maxViewWidth];

};

#define loByte(w)    (((uchar *)&w)[0])
#define hiByte(w)    (((uchar *)&w)[1])

inline void TDrawBuffer::putAttribute( ushort indent, ushort attr )
{
    hiByte(data[indent]) = uchar(attr);
}

inline void TDrawBuffer::putChar( ushort indent, ushort c )
{
    loByte(data[indent]) = uchar(c);
}

#endif  // Uses_TDrawBuffer

#pragma option -Vo.
#if defined( __BCOPT__ )
#pragma option -po.
#endif


/*------------------------------------------------------------------------*/
/* filename - drivers.cpp                                                 */
/*                                                                        */
/* function(s)                                                            */
/*        moveBuf  --   moves a buffer of char/attribute pairs            */
/*        moveChar --   sets a buffer with a char/attribute pair          */
/*        moveCStr --   moves a char array into a buffer & adds an        */
/*                      attribute to each char                            */
/*------------------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TDrawBuffer
#define Uses_TScreen
#define Uses_TText
#include <tvision/tv.h>

#if !defined( __DOS_H )
#include <dos.h>
#endif  // __DOS_H

#include <string.h>

#pragma warn -asc

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  TDrawBuffer::moveBuf                                                  */
/*                                                                        */
/*  arguments:                                                            */
/*                                                                        */
/*      indent  - position within the buffer where the data is to go      */
/*                (in columns)                                            */
/*                                                                        */
/*      source  - pointer to an array of characters                       */
/*                                                                        */
/*      attr    - attribute to be used for all characters (0 to retain    */
/*                the attribute from 'source')                            */
/*                                                                        */
/*      count   - number of characters to move                            */
/*                                                                        */
/*------------------------------------------------------------------------*/

void TDrawBuffer::moveBuf( ushort indent, const void _FAR *source,
                           TColorAttr attr, ushort count ) noexcept

{
    TStringView str((const char *) source, count);
    moveStr(indent, str, attr, USHRT_MAX, 0);
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  TDrawBuffer::moveChar                                                 */
/*                                                                        */
/*  arguments:                                                            */
/*                                                                        */
/*      indent  - position within the buffer where the data is to go      */
/*                (in columns)                                            */
/*                                                                        */
/*      c       - character to be put into the buffer (0 to retain the    */
/*                already present characters)                             */
/*                                                                        */
/*      attr    - attribute to be put into the buffer (0 to retain the    */
/*                already present attributes)                             */
/*                                                                        */
/*      count   - number of character/attribute pairs to put into the     */
/*                buffer                                                  */
/*                                                                        */
/*  Comments:                                                             */
/*                                                                        */
/*      If both 'c' and 'attr' are 0, the attributes are retained         */
/*      but the characters are not.                                       */
/*                                                                        */
/*------------------------------------------------------------------------*/

void TDrawBuffer::moveChar( ushort indent, char c, TColorAttr attr, ushort count ) noexcept
{
    if (count == 0 || indent >= capacity)
        return;
    if (indent + count >= capacity)
        count = capacity - indent;
    // The following conditions are now true:
    // * indent + count <= capacity
    // * count > 0

#if !defined( __FLAT__ )
    _ES = FP_SEG( &data[indent] );
    _DI = FP_OFF( &data[indent] );

    _CX = count;

I   MOV     AL,c
I   MOV     AH,[BYTE PTR attr]
I   CLD
I   TEST    AL,AL
I   JZ      __1
I   TEST    AH,AH
I   JZ      __3
I   REP     STOSW
I   JMP     __4

__1:

I   MOV     AL,AH

__2:

I   INC     DI

__3:

I   STOSB
I   LOOP    __2

__4:
    ;
#else
    TScreenCell *dest = &data[indent];

    if (attr != 0)
        if (c != 0)
            {
            TScreenCell cell;
            ::setCell(cell, (uchar) c, attr);
            while (count--)
                *dest++ = cell;
            }
        else
            while(count--)
                ::setAttr(*dest++, attr);
    else
        while (count--)
            ::setChar(*dest++, (uchar) c);
#endif
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  TDrawBuffer::moveCStr                                                 */
/*                                                                        */
/*  arguments:                                                            */
/*                                                                        */
/*      indent  - position within the buffer where the data is to go      */
/*                (in columns)                                            */
/*                                                                        */
/*      str     - string of characters to be moved into the buffer        */
/*                                                                        */
/*      attrs   - pair of text attributes to be put into the buffer       */
/*                with each character in the string.  Initially the       */
/*                low byte is used, and a '~' in the string toggles       */
/*                between the low byte and the high byte.                 */
/*                                                                        */
/*  returns:                                                              */
/*                                                                        */
/*      number of columns in the buffer that were actually updated        */
/*                                                                        */
/*------------------------------------------------------------------------*/

ushort TDrawBuffer::moveCStr( ushort indent, TStringView str, TAttrPair attrs ) noexcept
{
    return moveCStr(indent, str, attrs, USHRT_MAX, 0);
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  TDrawBuffer::moveCStr (2)                                             */
/*                                                                        */
/*  arguments:                                                            */
/*                                                                        */
/*      indent  - position within the buffer where the data is to go      */
/*                (in columns)                                            */
/*                                                                        */
/*      str     - string of characters to be moved into the buffer        */
/*                                                                        */
/*      attrs   - pair of text attributes to be put into the buffer       */
/*                with each character in the string.  Initially the       */
/*                low byte is used, and a '~' in the string toggles       */
/*                between the low byte and the high byte.                 */
/*                                                                        */
/*      maxStrWidth - maximum amount of data to be moved (in columns)     */
/*                                                                        */
/*      strIndent - position in str where to start moving from            */
/*                  (in columns)                                          */
/*                                                                        */
/*  returns:                                                              */
/*                                                                        */
/*      number of columns in the buffer that were actually updated        */
/*                                                                        */
/*------------------------------------------------------------------------*/

ushort TDrawBuffer::moveCStr( ushort indent, TStringView str, TAttrPair attrs,
                              ushort maxStrWidth, ushort strIndent ) noexcept
{
    if (indent >= capacity || str.size() == 0 || maxStrWidth == 0)
        return 0;
    if (indent + maxStrWidth < indent || indent + maxStrWidth >= capacity)
        maxStrWidth = capacity - indent;
    // The following conditions are now true:
    // * indent + maxStrWidth <= capacity
    // * maxStrWidth > 0
    // * str.size() > 0

#if !defined( __FLAT__ )
    // Compute the end pointer here since doing it later would overwrite
    // registers already in use.
    TScreenCell _FAR * dataEnd = &data[indent + maxStrWidth];

I   CLD
I   PUSH    DS

    _DS = FP_SEG( &str[0] );
    _SI = FP_OFF( &str[0] );

    _ES = FP_SEG( &data[indent] );
    _DI = FP_OFF( &data[indent] );

    _CX = str.size();

    _BX = attrs;
    _AH = _BL;

    // Skip 'strIndent' columns in 'str'.

    _DX = strIndent;

__1:

I   TEST    DX,DX
I   JZ      __4
I   LODSB
I   CMP     AL,'~'
I   JNE     __2
I   XCHG    AH,BH
I   JMP     __3

__2:

I   DEC     DX

__3:

I   LOOP    __1
I   JMP     __7

__4:

    // Copy string.

    _DX = FP_OFF( dataEnd );

I   LODSB
I   CMP     AL,'~'
I   JNE     __5
I   XCHG    AH,BH
I   JMP     __6

__5:

I   STOSW
I   CMP     DI,DX
I   JAE     __7

__6:

I   LOOP    __4

__7:

I   POP     DS

    return (_DI - FP_OFF( &data[indent] ))/sizeof(TScreenCell);
#else
    size_t i = indent, j = 0, w = 0;
    int toggle = 1;
    TColorAttr curAttr = ((TColorAttr *) &attrs)[0];
    TSpan<TScreenCell> dest(data, indent + maxStrWidth);
    while (j < str.size())
        if (str[j] == '~')
            {
            curAttr = ((TColorAttr *) &attrs)[toggle];
            toggle = 1 - toggle;
            ++j;
            }
        else
            {
            if (strIndent <= w)
                {
                if (!TText::drawOne(dest, i, str, j, curAttr))
                    break;
                }
            else
                {
                if (!TText::next(str, j, w))
                    break;
                if (strIndent < w && i < dest.size())
                    // 'strIndent' is in the middle of a double-width character.
                    ::setCell(dest[i++], ' ', curAttr);
                }
            }
    return i - indent;
#endif
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  TDrawBuffer::moveStr                                                  */
/*                                                                        */
/*  arguments:                                                            */
/*                                                                        */
/*      indent  - character position within the buffer where the data     */
/*                is to go                                                */
/*                                                                        */
/*      str     - string of characters to be moved into the buffer        */
/*                                                                        */
/*      attr    - text attribute to be put into the buffer with each      */
/*                character in the string.                                */
/*                                                                        */
/*  returns:                                                              */
/*                                                                        */
/*      number of columns in the buffer that were actually updated        */
/*                                                                        */
/*------------------------------------------------------------------------*/

ushort TDrawBuffer::moveStr( ushort indent, TStringView str, TColorAttr attr ) noexcept
{
    return moveStr(indent, str, attr, USHRT_MAX, 0);
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  TDrawBuffer::moveStr (2)                                              */
/*                                                                        */
/*  arguments:                                                            */
/*                                                                        */
/*      indent  - position within the buffer where the data is to go      */
/*                (in columns)                                            */
/*                                                                        */
/*      str     - string of characters to be moved into the buffer        */
/*                                                                        */
/*      attr    - text attribute to be put into the buffer with each      */
/*                character in the string.                                */
/*                                                                        */
/*      maxStrWidth - maximum amount of data to be moved (in columns)     */
/*                                                                        */
/*      strIndent - position in str where to start moving from            */
/*                  (in columns)                                          */
/*                                                                        */
/*  returns:                                                              */
/*                                                                        */
/*      number of columns in the buffer that were actually updated        */
/*                                                                        */
/*------------------------------------------------------------------------*/

ushort TDrawBuffer::moveStr( ushort indent, TStringView str, TColorAttr attr,
                             ushort maxStrWidth, ushort strIndent ) noexcept
{
    if (indent >= capacity || str.size() == 0 || maxStrWidth == 0)
        return 0;
    if (indent + maxStrWidth < indent || indent + maxStrWidth >= capacity)
        maxStrWidth = capacity - indent;
    // The following conditions are now true:
    // * indent + maxStrWidth <= capacity
    // * maxStrWidth > 0

#if !defined( __FLAT__ )
    if (strIndent >= str.size())
        return 0;
    // * count > 0
    ushort count = str.size() - strIndent;
    if (count > maxStrWidth)
        count = maxStrWidth;

I   CLD
I   PUSH    DS

    _DS = FP_SEG( &str[strIndent] );
    _SI = FP_OFF( &str[strIndent] );

    _ES = FP_SEG( &data[indent] );
    _DI = FP_OFF( &data[indent] );

    _CX = count;

    _AH = attr;

I   TEST    AH, AH
I   JZ      __2

__1:

I   LODSB
I   STOSW
I   LOOP    __1
I   JMP     __3

__2:

I   MOVSB
I   INC     DI
I   LOOP    __2

__3:

I   POP     DS

    return count;
#else
    TSpan<TScreenCell> dest(data, indent + maxStrWidth);
    if (attr != 0)
        return TText::drawStr(dest, indent, str, strIndent, attr);
    else
        return TText::drawStr(dest, indent, str, strIndent);
#endif
}

#if defined( __FLAT__ )
TDrawBuffer::TDrawBuffer() noexcept
{
    // Unlike on DOS, the screen's dimensions are arbitrary, so we have to take
    // this into account and allocate the buffer dynamically. We must take the
    // largest of the screen's dimensions, since TDrawBuffer can also be used to
    // draw vertical views (e.g. TScrollBar).
    // In addition, we give some room for views that might exceed the screen size.
    capacity = 8 + max(max(TScreen::screenWidth, TScreen::screenHeight), 80);
    data = new TScreenCell[capacity];
#if !defined( __BORLANDC__ )
    // We cannot leave the buffer uninitialized because, if it ends up being
    // displayed on screen, it may mess up the screen severely.
    memset(data, 0, capacity * sizeof(TScreenCell));
#endif // __BORLANDC__
}

TDrawBuffer::~TDrawBuffer()
{
    delete[] data;
}
#endif // __FLAT__

#pragma warn .asc

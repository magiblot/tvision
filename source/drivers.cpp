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
#include <tvision\tv.h>

#if !defined( __DOS_H )
#include <Dos.h>
#endif  // __DOS_H

#pragma warn -asc

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  TDrawBuffer::moveBuf                                                  */
/*                                                                        */
/*  arguments:                                                            */
/*                                                                        */
/*      indent - character position within the buffer where the data      */
/*               is to go                                                 */
/*                                                                        */
/*      source - far pointer to an array of character/attribute pairs     */
/*                                                                        */
/*      attr   - attribute to be used for all characters (0 to retain     */
/*               the attribute from 'source')                             */
/*                                                                        */
/*      count   - number of character/attribute pairs to move             */
/*                                                                        */
/*------------------------------------------------------------------------*/

void TDrawBuffer::moveBuf( ushort indent, const void _FAR *source,
                           ushort attr, ushort count )

{
#if !defined( __FLAT__ )

I   MOV     CX, count
I   JCXZ    __5
I   PUSH    DS

    _ES = FP_SEG( &data[indent] );
    _DI = FP_OFF( &data[indent] );

//    _DS = FP_SEG( source );
//    _SI = FP_OFF( source );
I   LDS     SI, source

I   MOV     AH, [BYTE PTR attr]
I   CLD
I   OR      AH, AH
I   JE      __3

__1:

I   LODSB
I   STOSW
I   LOOP    __1
I   JMP     __4

__2:

I   INC     DI

__3:

I   MOVSB
I   LOOP    __2

__4:

I   POP     DS

__5:
        ;
#else

    register ushort *dest = &data[indent];
    register uchar _FAR *s = (uchar _FAR *)source;

    if (attr != 0)
        for (; count; --count, ++s, ++dest)
        {
            ((uchar*)dest)[0] = *s;
            ((uchar*)dest)[1] = (uchar)attr;
        }
    else
        while (count--)
            *(uchar *)dest++ = *s++;

#endif
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  TDrawBuffer::moveChar                                                 */
/*                                                                        */
/*  arguments:                                                            */
/*                                                                        */
/*      indent  - character position within the buffer where the data     */
/*                is to go                                                */
/*                                                                        */
/*      c       - character to be put into the buffer                     */
/*                                                                        */
/*      attr    - attribute to be put into the buffer                     */
/*                                                                        */
/*      count   - number of character/attribute pairs to put into the     */
/*                buffer                                                  */
/*                                                                        */
/*------------------------------------------------------------------------*/

void TDrawBuffer::moveChar( ushort indent, char c, ushort attr, ushort count )
{
#if !defined( __FLAT__ )
I   MOV     CX,count
I   JCXZ    __4

    _ES = FP_SEG( &data[indent] );
    _DI = FP_OFF( &data[indent] );

I   MOV     AL,c
I   MOV     AH,[BYTE PTR attr]
I   CLD
I   OR      AL,AL
I   JE      __1
I   OR      AH,AH
I   JE      __3
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
    register ushort *dest = &data[indent];

    if (attr != 0)
        for (; count; --count, ++dest)
            {
            if (c) ((uchar*)dest)[0] = c;
            ((uchar*)dest)[1] = (uchar)attr;
            }
    else
        while (count--)
            *(uchar *)dest++ = c;

#endif
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  TDrawBuffer::moveCStr                                                 */
/*                                                                        */
/*  arguments:                                                            */
/*                                                                        */
/*      indent  - character position within the buffer where the data     */
/*                is to go                                                */
/*                                                                        */
/*      str     - pointer to a 0-terminated string of characters to       */
/*                be moved into the buffer                                */
/*                                                                        */
/*      attrs   - pair of text attributes to be put into the buffer       */
/*                with each character in the string.  Initially the       */
/*                low byte is used, and a '~' in the string toggles       */
/*                between the low byte and the high byte.                 */
/*                                                                        */
/*------------------------------------------------------------------------*/

void TDrawBuffer::moveCStr( ushort indent, const char _FAR *str, ushort attrs)
{
#if !defined ( __FLAT__ )
I   PUSH    DS
I   LDS     SI,str
I   CLD

    _ES = FP_SEG( &data[indent] );
    _DI = FP_OFF( &data[indent] );

I   MOV     BX,attrs
I   MOV     AH,BL

__1:

I   LODSB
I   CMP     AL,'~'
I   JE      __2
I   CMP     AL,0
I   JE      __3
I   STOSW
I   JMP     __1

__2:

I   XCHG    AH,bH
I   JMP     __1

__3:

I   POP     DS

#else
    register ushort *dest = &data[indent];
    int toggle;
    uchar c, curAttr;

    for (curAttr=((uchar *)&attrs)[0], toggle=1; (c=*str) != 0; str++)
        {
        if (c == '~')
            {
            curAttr = ((uchar *)&attrs)[toggle];
            toggle = 1-toggle;
            }
        else
            {
            ((uchar*)dest)[0] = c;
            ((uchar*)dest)[1] = curAttr;
            dest++;
            }
        }
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
/*      str     - pointer to a 0-terminated string of characters to       */
/*                be moved into the buffer                                */
/*                                                                        */
/*      attr    - text attribute to be put into the buffer with each      */
/*                character in the string.                                */
/*                                                                        */
/*------------------------------------------------------------------------*/

void TDrawBuffer::moveStr( ushort indent, const char _FAR *str, ushort attr )
{
#if !defined ( __FLAT__ )
I   PUSH    DS
I   LDS     SI,str
I   CLD

    _ES = FP_SEG( &data[indent] );
    _DI = FP_OFF( &data[indent] );

I   MOV     AH, byte ptr attr
I   OR      AH, AH
I   JZ      __2

__1:

I   LODSB
I   OR      AL,AL
I   JZ      __3
I   STOSW
I   JMP     __1

__2:

I   LODSB
I   OR      AL,AL
I   JZ      __3
I   STOSB
I   INC            DI
I   JMP     __2

__3:

I   POP     DS

#else

    register ushort *dest = &data[indent];
    uchar c;

    if (attr != 0)
        for (;(c=*str) != 0; ++str, ++dest)
            {
            ((uchar*)dest)[0] = c;
            ((uchar*)dest)[1] = (uchar)attr;
            }
        else
            while (*str)
                *(uchar *)dest++ = *str++;
#endif
}
#pragma warn .asc

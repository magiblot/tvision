/*------------------------------------------------------------------------*/
/* filename - drivers.cpp                                                 */
/*                                                                        */
/* function(s)                                                            */
/*        moveBuf  --   moves a buffer of char/attribute pairs            */
/*        moveChar --   sets a buffer with a char/attribute pair          */
/*        moveCStr --   moves a char array into a buffer & adds an        */
/*                      attribute to each char                            */
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/*                                                                        */
/*    Turbo Vision -  Version 1.0                                         */
/*                                                                        */
/*                                                                        */
/*    Copyright (c) 1991 by Borland International                         */
/*    All Rights Reserved.                                                */
/*                                                                        */
/*------------------------------------------------------------------------*/

#define Uses_TDrawBuffer
#include <tv.h>

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

void TDrawBuffer::moveBuf( ushort indent, const void far *source,
                           ushort attr, ushort count )

{
asm {
    MOV     CX,count
    JCXZ    __5
    PUSH    DS
    }

    _ES = FP_SEG( &data[indent] );
    _DI = FP_OFF( &data[indent] );

    _DS = FP_SEG( source );
    _SI = FP_OFF( source );

asm {
    MOV     AH,[BYTE PTR attr]
    CLD
    OR      AH,AH
    JE      __3
    }
__1:
asm {
    LODSB
    STOSW
    LOOP    __1
    JMP     __4
    }
__2:
asm INC     DI
__3:
asm {
    STOSB
    LOOP    __2
    }
__4:
asm POP     DS
__5:
        ;
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
asm {
    MOV     CX,count
    JCXZ    __4
    }

    _ES = FP_SEG( &data[indent] );
    _DI = FP_OFF( &data[indent] );

asm {
    MOV     AL,c
    MOV     AH,[BYTE PTR attr]
    CLD
    OR      AL,AL
    JE      __1
    OR      AH,AH
    JE      __3
    REP     STOSW
    JMP     __4
    }
__1:
asm MOV     AL,AH
__2:
asm INC     DI
__3:
asm STOSB
asm LOOP    __2
__4:
    ;
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

void TDrawBuffer::moveCStr( ushort indent, const char far *str, ushort attrs )
{
asm {
    PUSH    DS
    LDS     SI,str
    CLD
    }

    _ES = FP_SEG( &data[indent] );
    _DI = FP_OFF( &data[indent] );

asm {
    MOV     BX,attrs
    MOV     AH,BL
    }
__1:
asm {
    LODSB
    CMP     AL,'~'
    JE      __2
    CMP     AL,0
    JE      __3
    STOSW
    JMP     __1
    }
__2:
asm XCHG    AH,bH
asm JMP     __1
__3:
asm POP     DS
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

void TDrawBuffer::moveStr( ushort indent, const char far *str, ushort attr )
{
asm {
    PUSH    DS
    LDS     SI,str
    CLD
    }

    _ES = FP_SEG( &data[indent] );
    _DI = FP_OFF( &data[indent] );

asm {
    MOV     BX,attr
    MOV     AH,BL
    }
__1:
asm {
    LODSB
    CMP     AL,0
    JE      __2
    STOSW
    JMP     __1
    }
__2:
asm POP     DS
}

#pragma warn .asc

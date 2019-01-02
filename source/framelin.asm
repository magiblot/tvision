;/*------------------------------------------------------------*/
;/* filename -       framelin.asm                              */
;/*                                                            */
;/* function(s)                                                */
;/*                  TFrame frameLine member function          */
;/*------------------------------------------------------------*/

;/*------------------------------------------------------------*/
;/*                                                            */
;/*    Turbo Vision -  Version 1.0                             */
;/*                                                            */
;/*                                                            */
;/*    Copyright (c) 1991 by Borland International             */
;/*    All Rights Reserved.                                    */
;/*                                                            */
;/*------------------------------------------------------------*/

        IDEAL
        MODEL  LARGE, PROLOG

        INCLUDE "TVWRITE.INC"

        PUBLIC  @TFrame@frameLine$qm11TDrawBufferssuc
        EXTRN   @TFrame@initFrame : WORD
        EXTRN   @TFrame@frameChars : WORD

        MaxViewWidth    equ     132

DATASEG

FrameMask       DB      MaxViewWidth dup(?)

CODESEG

PROC    @TFrame@frameLine$qm11TDrawBufferssuc
        ARG     thisPtr : FAR PTR, FrameBuf : FAR PTR, Y : WORD, N : WORD, Color : BYTE
        USES    SI,DI

        LES     BX,[thisPtr]
        MOV     DX,[ES:BX+TFrameSizeX]
        MOV     CX,DX
        DEC     CX
        DEC     CX
        MOV     SI,OFFSET @TFrame@initFrame
        ADD     SI,[N]
        LEA     DI,[FrameMask]
        PUSH    DS
        POP     ES
        CLD
        MOVSB
        LODSB
        REP     STOSB
        MOVSB
        LES     BX,[thisPtr]
        LES     BX,[ES:BX+TFrameOwner]
        LES     BX,[ES:BX+TGroupLast]
        DEC     DX
@1:     LES     BX,[ES:BX+TViewNext]
        CMP     BX,[WORD PTR thisPtr]
        JNE     @2
        MOV     AX,ES
        CMP     AX,[WORD PTR thisPtr+2]
        JE      @10
@2:     TEST    [WORD PTR ES:BX+TViewOptions],ofFramed
        JE      @1
        TEST    [WORD PTR ES:BX+TViewState],sfVisible
        JE      @1
        MOV     AX,[Y]
        SUB     AX,[ES:BX+TViewOriginY]
        JL      @3
        CMP     AX,[ES:BX+TViewSizeY]
        JG      @1
        MOV     AX,0005H
        JL      @4
        MOV     AX,0A03H
        JMP     @4
@3:     INC     AX
        JNE     @1
        MOV     AX,0A06H
@4:     MOV     SI,[ES:BX+TViewOriginX]
        MOV     DI,[ES:BX+TViewSizeX]
        ADD     DI,SI
        CMP     SI,1
        JG      @5
        MOV     SI,1
@5:     CMP     DI,DX
        JL      @6
        MOV     DI,DX
@6:     CMP     SI,DI
        JGE     @1
        OR      [BYTE PTR FrameMask+SI-1],AL
        XOR     AL,AH
        OR      [BYTE PTR FrameMask+DI],AL
        OR      AH,AH
        JE      @1
        MOV     CX,DI
        SUB     CX,SI
@8:     OR      [BYTE PTR FrameMask+SI],AH
        INC     SI
        LOOP    @8
        JMP     @1
@10:    INC     DX
        MOV     AH,[Color]
        MOV     BX,OFFSET @TFrame@frameChars
        MOV     CX,DX
        LEA     SI,[FrameMask]
        LES     DI,[FrameBuf]
        ADD     DI,TDrawBufferData
@11:    LODSB
        XLAT
        STOSW
        LOOP    @11
        RET
ENDP
END

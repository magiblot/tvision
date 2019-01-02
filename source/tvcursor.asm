;/*------------------------------------------------------------*/
;/* filename -       tvcursor.asm                              */
;/*                                                            */
;/* function(s)                                                */
;/*                  TView resetCursor member function         */
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

        PUBLIC  @TView@resetCursor$qv

        EXTRN   @TScreen@cursorLines : WORD

CODESEG

PROC    @TView@resetCursor$qv
        ARG     thisPtr : FAR PTR
        USES    SI,DI

        LES     DI,[thisPtr]
        
        MOV     AX,[ES:DI+TViewState]
        NOT     AX
        TEST    AX,sfVisible+sfCursorVis+sfFocused
        JNE     @@4
        MOV     AX,[ES:DI+TViewCursorY]
        MOV     DX,[ES:DI+TViewCursorX]
@@1:
        OR      AX,AX
        JL      @@4
        CMP     AX,[ES:DI+TViewSizeY]
        JGE     @@4
        OR      DX,DX
        JL      @@4
        CMP     DX,[ES:DI+TViewSizeX]
        JGE     @@4
        ADD     AX,[ES:DI+TViewOriginY]
        ADD     DX,[ES:DI+TViewOriginX]
        MOV     CX,DI
        MOV     BX,ES
        LES     DI,[ES:DI+TViewOwner]
        MOV     SI,ES
        OR      SI,DI
        JE      @@5
        TEST    [WORD PTR ES:DI+TViewState],sfVisible
        JE      @@4
        LES     DI,[ES:DI+TGroupLast]
@@2:
        LES     DI,[ES:DI+TViewNext]
        CMP     CX,DI
        JNE     @@3
        MOV     SI,ES
        CMP     BX,SI
        JNE     @@3
        LES     DI,[ES:DI+TViewOwner]
        JMP     @@1
@@3:
        TEST    [WORD PTR ES:DI+TViewState],sfVisible
        JE      @@2
        MOV     SI,[ES:DI+TViewOriginY]
        CMP     AX,SI
        JL      @@2
        ADD     SI,[ES:DI+TViewSizeY]
        CMP     AX,SI
        JGE     @@2
        MOV     SI,[ES:DI+TViewOriginX]
        CMP     DX,SI
        JL      @@2
        ADD     SI,[ES:DI+TViewSizeX]
        CMP     DX,SI
        JGE     @@2
@@4:
        MOV     CX,2000H
        JMP     @@6
@@5:
        MOV     DH,AL
        XOR     BH,BH
        MOV     AH,2
        INT     10H
        MOV     CX,[WORD PTR @TScreen@cursorLines]
        MOV     DI,DS
        MOV     ES,DI
        LES     DI,[thisPtr]
        TEST    [WORD PTR ES:DI+TViewState],sfCursorIns
        JE      @@6
        MOV     CH,0
        OR      CL,CL
        JNE     @@6
        MOV     CL,7
@@6:
        MOV     AH,1
        INT     10H
        RET

ENDP    @TView@resetCursor$qv

END

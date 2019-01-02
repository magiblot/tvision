;/*------------------------------------------------------------*/
;/* filename -       tvexposd.asm                              */
;/*                                                            */
;/* function(s)                                                */
;/*                  TView exposed member function             */
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

        PUBLIC  @TView@exposed$qv

STRUC   Fptr
        offs    DW      ?
        segm    DW      ?
ENDS

CODESEG

PROC    @TView@exposed$qv
        ARG     thisPtr : DWORD
        LOCAL   Target  : Fptr
        USES    SI,DI

        LES     DI,[thisPtr]
        TEST    [WORD PTR ES:DI+TViewState],sfExposed
        JE      @@2
        XOR     AX,AX
        CMP     AX,[ES:DI+TViewSizeX]
        JGE     @@2
        CMP     AX,[ES:DI+TViewSizeY]
        JGE     @@2
@@1:    
        XOR     BX,BX
        MOV     CX,[ES:DI+TViewSizeX]
        PUSH    AX
        CALL    @@11
        POP     AX
        JNC     @@3
        LES     DI,[thisPtr]
        INC     AX
        CMP     AX,[ES:DI+TViewSizeY]
        JL      @@1
@@2:    
        MOV     AL,0
        JMP     @@30
@@3:    
        MOV     AL,1
        JMP     @@30
@@8:    
        STC
@@9:    
        RETN
@@10:   
        LES     DI,[DWORD PTR ES:DI+TViewOwner]
        CMP     [WORD PTR ES:DI+TGroupBuffer+2],0
        JNE     @@9
@@11:   MOV     [Target.offs],DI
        MOV     [Target.segm],ES
        ADD     AX,[ES:DI+TViewOriginY]
        MOV     SI,[ES:DI+TViewOriginX]
        ADD     BX,SI
        ADD     CX,SI
        LES     DI,[ES:DI+TViewOwner]
        MOV     SI,ES
        OR      SI,DI
        JE      @@9
        CMP     AX,[ES:DI+TGroupClipAY]
        JL      @@8
        CMP     AX,[ES:DI+TGroupClipBY]
        JGE     @@8
        CMP     BX,[ES:DI+TGroupClipAX]
        JGE     @@12
        MOV     BX,[ES:DI+TGroupClipAX]
@@12:   
        CMP     CX,[ES:DI+TGroupClipBX]
        JLE     @@13
        MOV     CX,[ES:DI+TGroupClipBX]
@@13:   
        CMP     BX,CX
        JGE     @@8
        LES     DI,[ES:DI+TGroupLast]
@@20:   
        LES     DI,[ES:DI+TViewNext]
        CMP     DI,[Target.offs]
        JNE     @@21
        MOV     SI,ES
        CMP     SI,[Target.segm]
        JE      @@10
@@21:   
        TEST    [WORD PTR ES:DI+TViewState],sfVisible
        JE      @@20
        MOV     SI,[ES:DI+TViewOriginY]
        CMP     AX,SI
        JL      @@20
        ADD     SI,[ES:DI+TViewSizeY]
        CMP     AX,SI
        JGE     @@20
        MOV     SI,[ES:DI+TViewOriginX]
        CMP     BX,SI
        JL      @@22
        ADD     SI,[ES:DI+TViewSizeX]
        CMP     BX,SI
        JGE     @@20
        MOV     BX,SI
        CMP     BX,CX
        JL      @@20
        STC
        RETN
@@22:   
        CMP     CX,SI
        JLE     @@20
        ADD     SI,[ES:DI+TViewSizeX]
        CMP     CX,SI
        JG      @@23
        MOV     CX,[ES:DI+TViewOriginX]
        JMP     @@20
@@23:   
        PUSH    [Target.segm]
        PUSH    [Target.offs]
        PUSH    ES
        PUSH    DI
        PUSH    SI
        PUSH    CX
        PUSH    AX
        MOV     CX,[ES:DI+TViewOriginX]
        CALL    @@20
        POP     AX
        POP     CX
        POP     BX
        POP     DI
        POP     ES
        POP     [Target.offs]
        POP     [Target.segm]
        JC      @@20
        RETN
@@30:
        RET
ENDP

END

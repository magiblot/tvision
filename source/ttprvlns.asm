;/*------------------------------------------------------------*/
;/* filename -       ttprvlns.asm                              */
;/*                                                            */
;/* function(s)                                                */
;/*                  TTerminal prevLines member function       */
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

        PUBLIC  @TTerminal@prevLines$qusus

CODESEG

PROC    DecDI
        CMP     DI,[WORD PTR SI+TTerminalBuffer]
        JA      @@1
        ADD     DI,[WORD PTR SI+TTerminalBufSize]
@@1:    DEC     DI
        RET
ENDP


PROC    IncDI
        INC     DI
        MOV     AX,[WORD PTR SI+TTerminalBuffer]
        ADD     AX,[SI+TTerminalBufSize]
        CMP     DI,AX
        JB      @@1
        MOV     DI,[WORD PTR SI+TTerminalBuffer]
@@1:
        RET
ENDP


PROC    @TTerminal@prevLines$qusus
        ARG     thisPtr : FAR PTR, Pos : WORD, Lines : WORD
        USES    DS,SI,DI

LineSeparator   EQU 10

        LDS     SI,[thisPtr]
        LES     DI,[SI+TTerminalBuffer]
        ADD     DI,[Pos]
@@1:    MOV     CX,[Lines]
        JCXZ    @@6
        MOV     AX,[SI+TTerminalQueBack]
        ADD     AX,[WORD PTR SI+TTerminalBuffer]
        CMP     DI,AX
        JE      @@7
        CALL    DecDI
@@2:    MOV     AX,[SI+TTerminalQueBack]
        ADD     AX,[WORD PTR SI+TTerminalBuffer]
        CMP     DI,AX
        JA      @@3
        MOV     CX,DI
        SUB     CX,[WORD PTR SI+TTerminalBuffer]
        JMP     @@4
@@3:    MOV     CX,DI
        SUB     CX,AX
@@4:    MOV     AL,LineSeparator
        INC     CX
        STD
        REPNE   SCASB
        JE      @@5
        MOV     AX,DI
        SUB     AX,[WORD PTR SI+TTerminalBuffer]
        INC     AX
        CMP     AX,[SI+TTerminalQueBack]
        JE      @@8
        MOV     DI,[WORD PTR SI+TTerminalBuffer]
        ADD     DI,[WORD PTR SI+TTerminalBufSize]
        DEC     DI
        JMP     @@2
@@5:    DEC     [Lines]
        JNZ     @@2
@@6:    CALL    IncDI
        CALL    IncDI
        MOV     AX,DI
@@7:    SUB     AX,[WORD PTR SI+TTerminalBuffer]
@@8:
        CLD
        RET
        ENDP
        END

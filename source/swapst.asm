;/*------------------------------------------------------------*/
;/* filename -       swapst.asm                                */
;/*                                                            */
;/* function(s)                                                */
;/*                  TSystemError swapStatusLine function      */
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

        PUBLIC @TSystemError@swapStatusLine$qm11TDrawBuffer

        EXTRN   @TScreen@screenWidth : BYTE
        EXTRN   @TScreen@screenHeight : BYTE
        EXTRN   @TScreen@screenBuffer : DWORD

CODESEG
        
PROC    @TSystemError@swapStatusLine$qm11TDrawBuffer
        ARG     Buffer : FAR PTR

        MOV     CL,[@TScreen@screenWidth]
        XOR     CH,CH
        MOV     AL,[@TScreen@screenHeight]
        DEC     AL
        MUL     CL
        SHL     AX,1
        LES     DI,[@TScreen@screenBuffer]
        ADD     DI,AX
        PUSH    DS
        LDS     SI,[Buffer]
@@1:    MOV     AX,[ES:DI]
        MOVSW
        MOV     [DS:SI-2],AX
        LOOP    @@1
        POP     DS
        RET
ENDP
END

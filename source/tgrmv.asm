;/*------------------------------------------------------------*/
;/* filename -       tgrmv.asm                                 */
;/*                                                            */
;/* function(s)                                                */
;/*                  TGroup removeView member function         */
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

        PUBLIC  @TGroup@removeView$qn5TView

CODESEG

PROC    @TGroup@removeView$qn5TView
        ARG     thisPtr : DWORD, P : DWORD
        USES    SI,DI,DS

        LDS     SI,[thisPtr]
        LES     DI,[P]
        LDS     SI,[DS:SI+TGroupLast]
        PUSH    BP
        MOV     AX,DS
        OR      AX,SI
        JE      @@7
        MOV     AX,SI
        MOV     DX,DS
        MOV     BP,ES
@@1:    
        MOV     BX,[WORD PTR DS:SI+TViewNext]
        MOV     CX,[WORD PTR DS:SI+TViewNext+2]
        CMP     CX,BP
        JE      @@5
@@2:    
        CMP     CX,DX
        JE      @@4
@@3:    
        MOV     SI,BX
        MOV     DS,CX
        JMP     @@1
@@4:    
        CMP     BX,AX
        JNE     @@3
        JMP     @@7
@@5:    
        CMP     BX,DI
        JNE     @@2
        MOV     BX,[WORD PTR ES:DI+TViewNext]
        MOV     CX,[WORD PTR ES:DI+TViewNext+2]
        MOV     [WORD PTR DS:SI+TViewNext],BX
        MOV     [WORD PTR DS:SI+TViewNext+2],CX
        CMP     DX,BP
        JNE     @@7
        CMP     AX,DI
        JNE     @@7
        CMP     CX,BP
        JNE     @@6
        CMP     BX,DI
        JNE     @@6
        XOR     SI,SI
        MOV     DS,SI
@@6:    
        POP     BP
        PUSH    BP
        LES     DI,[thisPtr]
        MOV     [WORD PTR ES:DI+TGroupLast],SI
        MOV     [WORD PTR ES:DI+TGroupLast+2],DS
@@7:    
        POP     BP
        RET
ENDP
END

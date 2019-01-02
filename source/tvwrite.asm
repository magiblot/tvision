;/*------------------------------------------------------------*/
;/* filename -       tvwrite.asm                               */
;/*                                                            */
;/* function(s)                                                */
;/*                  TView write member functions              */
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

        PUBLIC  @TView@writeBuf$qssssnxv
        PUBLIC  @TView@writeView$qv
        PUBLIC  @TView@writeChar$qsszcucs
        PUBLIC  @TView@writeLine$qssssnxv
        PUBLIC  @TView@writeStr$qssnxzcuc

        EXTRN   @THWMouse@show$qv : PROC
        EXTRN   @THWMouse@hide$qv : PROC
        EXTRN   @TEventQueue@mouseIntFlag : BYTE
        EXTRN   @TEventQueue@curMouse : WORD
        EXTRN   _shadowSize : WORD
        EXTRN   _shadowAttr : BYTE
        EXTRN   @TView@mapColor$quc : PROC
        EXTRN   @TScreen@screenBuffer : DWORD
        EXTRN   @TScreen@checkSnow : BYTE

STRUC   Fptr
        offs    DW      ?
        segm    DW      ?
ENDS

STRUC   WriteArgs
        Self    DD      ?
        Target  Fptr    ?
        Buffer  Fptr    ?
        Offset  DW      ?
ENDS

DATASEG

wArgs   WriteArgs       ?

IFDEF 0; PROTECT
        EXTRN _MONOSEG:WORD
        EXTRN _COLRSEG:WORD
ENDIF

CODESEG

; Write to view
; In    AX    = Y coordinate
;       BX    = X coordinate
;       CX    = Count
;       ES:DI = Buffer Pointer

PROC    @TView@writeView$qv near

        MOV     [wArgs.Offset],BX
        MOV     [wArgs.Buffer.offs],DI
        MOV     [wArgs.Buffer.segm],ES
        ADD     CX,BX
        XOR     DX,DX
        LES     DI,[wArgs.Self]
        OR      AX,AX
        JL      @@3
        CMP     AX,[ES:DI+TViewSizeY]
        JGE     @@3
        OR      BX,BX
        JGE     @@1
        XOR     BX,BX
@@1:
        CMP     CX,[ES:DI+TViewSizeX]
        JLE     @@2
        MOV     CX,[ES:DI+TViewSizeX]
@@2:
        CMP     BX,CX
        JL      @@10
@@3:
        RETN
@@10:
        TEST    [WORD PTR ES:DI+TViewState],sfVisible
        JE      @@3
        CMP     [WORD PTR ES:DI+TViewOwner+2],0
        JE      @@3
        MOV     [wArgs.Target.offs],DI
        MOV     [wArgs.Target.segm],ES
        ADD     AX,[ES:DI+TViewOriginY]
        MOV     SI,[ES:DI+TViewOriginX]
        ADD     BX,SI
        ADD     CX,SI
        ADD     [wArgs.Offset],SI
        LES     DI,[ES:DI+TViewOwner]
        CMP     AX,[ES:DI+TGroupClipAY]
        JL      @@3
        CMP     AX,[ES:DI+TGroupClipBY]
        JGE     @@3
        CMP     BX,[ES:DI+TGroupClipAX]
        JGE     @@11
        MOV     BX,[ES:DI+TGroupClipAX]
@@11:   
        CMP     CX,[ES:DI+TGroupClipBX]
        JLE     @@12
        MOV     CX,[ES:DI+TGroupClipBX]
@@12:   
        CMP     BX,CX
        JGE     @@3
        LES     DI,[ES:DI+TGroupLast]
@@20:   
        LES     DI,[ES:DI+TViewNext]
        CMP     DI,[wArgs.Target.offs]
        JNE     @@21
        MOV     SI,ES
        CMP     SI,[wArgs.Target.segm]
        JNE     @@21
        JMP     @@40
@@21:   
        TEST    [WORD PTR ES:DI+TViewState],sfVisible
        JE      @@20
        MOV     SI,[ES:DI+TViewOriginY]
        CMP     AX,SI
        JL      @@20
        ADD     SI,[ES:DI+TViewSizeY]
        CMP     AX,SI
        JL      @@23
        TEST    [WORD PTR ES:DI+TViewState],sfShadow
        JE      @@20
        ADD     SI,[_shadowSize+TPointY]
        CMP     AX,SI
        JGE     @@20
        MOV     SI,[ES:DI+TViewOriginX]
        ADD     SI,[_shadowSize+TPointX]
        CMP     BX,SI
        JGE     @@22
        CMP     CX,SI
        JLE     @@20
        CALL    @@30
@@22:   
        ADD     SI,[ES:DI+TViewSizeX]
        JMP     @@26
@@23:   
        MOV     SI,[ES:DI+TViewOriginX]
        CMP     BX,SI
        JGE     @@24
        CMP     CX,SI
        JLE     @@20
        CALL    @@30
@@24:   
        ADD     SI,[ES:DI+TViewSizeX]
        CMP     BX,SI
        JGE     @@25
        CMP     CX,SI
        JLE     @@31
        MOV     BX,SI
@@25:   
        TEST    [WORD PTR ES:DI+TViewState],sfShadow
        JE      @@20
        PUSH    SI
        MOV     SI,[ES:DI+TViewOriginY]
        ADD     SI,[_shadowSize+TPointY]
        CMP     AX,SI
        POP     SI
        JL      @@27
        ADD     SI,[_shadowSize+TPointX]
@@26:   
        CMP     BX,SI
        JGE     @@27
        INC     DX
        CMP     CX,SI
        JLE     @@27
        CALL    @@30
        DEC     DX
@@27:   
        JMP     @@20
@@30:
        PUSH    [wArgs.Target.segm]
        PUSH    [wArgs.Target.offs]
        PUSH    [wArgs.Offset]
        PUSH    ES
        PUSH    DI
        PUSH    SI
        PUSH    DX
        PUSH    CX
        PUSH    AX
        MOV     CX,SI
        CALL    @@20
        POP     AX
        POP     CX
        POP     DX
        POP     SI
        POP     DI
        POP     ES
        POP     [wArgs.Offset]
        POP     [wArgs.Target.offs]
        POP     [wArgs.Target.segm]
        MOV     BX,SI
@@31:   
        RETN
@@40:   
        LES     DI,[ES:DI+TViewOwner]
        MOV     SI,[ES:DI+TGroupBuffer+2]
        OR      SI,SI
        JE      @@44
IFDEF 0; PROTECT
        PUSH    DS              ; cannot predict addresses of video screen
        PUSH    AX
        MOV     AX,DGROUP
        MOV     DS,AX
        POP     AX
        CMP     SI,[_MONOSEG]
        JNE     @@P01
@@P00:  POP     DS
        JMP     SHORT @@41
@@P01:  CMP     SI,[_COLRSEG]
        JE      @@P00
        POP     DS
ELSE
        CMP     SI,[WORD PTR @TScreen@screenBuffer+2]
        JE      @@41
ENDIF
        CALL    @@50
        JMP     @@44
@@41:   
        CLI
        CMP     AX,[WORD PTR @TEventQueue@curMouse+MsEventWhereY]
        JNE     @@42
        CMP     BX,[WORD PTR @TEventQueue@curMouse+MsEventWhereX]
        JA      @@42
        CMP     CX,[WORD PTR @TEventQueue@curMouse+MsEventWhereX]
        JA      @@43
@@42:   
        MOV     [@TEventQueue@mouseIntFlag],0
        STI
        CALL    @@50
        CMP     [@TEventQueue@mouseIntFlag],0
        JE      @@44
@@43:   
        STI
        CALL    @THWMouse@hide$qv
        CALL    @@50
        CALL    @THWMouse@show$qv
@@44:   
        CMP     [BYTE PTR ES:DI+TGroupLockFlag],0
        JNE     @@31
        JMP     @@10
@@50:
        PUSH    ES
        PUSH    DS
        PUSH    DI
        PUSH    CX
        PUSH    AX
        MUL     [BYTE PTR ES:DI+TViewSizeX]
        ADD     AX,BX
        SHL     AX,1
        ADD     AX,[ES:DI+TGroupBuffer]
        MOV     DI,AX
        MOV     ES,SI
        XOR     AL,AL
        CMP     SI,[WORD PTR @TScreen@screenBuffer+2]
        JNE     @@51
        MOV     AL,[@TScreen@checkSnow]
@@51:
        MOV     AH,[_shadowAttr]
        SUB     CX,BX
        MOV     SI,BX
        SUB     SI,[wArgs.Offset]
        SHL     SI,1
        ADD     SI,[wArgs.Buffer.offs]
        MOV     DS,[wArgs.Buffer.segm]
        CLD
        OR      AL,AL
        JNE     @@60
        OR      DX,DX
        JNE     @@52
        REP     MOVSW
        JMP     @@70
@@52:
        LODSB
        INC     SI
        STOSW
        LOOP    @@52
        JMP     @@70
@@60:   PUSH    DX
        PUSH    BX
        OR  DX,DX
        MOV DX,03DAH
        JNE @@65
@@61:   LODSW
        MOV BX,AX
@@62:   IN  AL,DX
        TEST    AL,1
        JNE @@62
        CLI
@@63:   IN  AL,DX
        TEST    AL,1
        JE  @@63
        MOV AX,BX
        STOSW
        STI
        LOOP    @@61
        JMP @@68
@@65:   LODSB
        MOV BL,AL
        INC SI
@@66:   IN  AL,DX
        TEST    AL,1
        JNE @@66
        CLI
@@67:   IN  AL,DX
        TEST    AL,1
        JE  @@67
        MOV AL,BL
        STOSW
        STI
        LOOP    @@65
@@68:   POP BX
        POP DX
@@70:   
        MOV     SI,ES
        POP     AX
        POP     CX
        POP     DI
        POP     DS
        POP     ES
        RETN
ENDP

PROC    @TView@writeBuf$qssssnxv
        ARG     thisPtr : DWORD, X : WORD, Y : WORD, W : WORD, H : WORD, Buf : DWORD
        USES    SI,DI

        MOV     AX,[WORD PTR thisPtr]
        MOV     [WORD PTR wArgs.Self],AX
        MOV     AX,[WORD PTR thisPtr+2]
        MOV     [(WORD PTR wArgs.Self)+2],AX

        CMP     [H],0
        JLE     @@2
@@1:    
        MOV     AX,[Y]
        MOV     BX,[X]
        MOV     CX,[W]
        LES     DI,[Buf]
        CALL    @TView@writeView$qv
        MOV     AX,[W]
        SHL     AX,1
        ADD     [WORD PTR Buf],AX
        INC     [Y]
        DEC     [H]
        JNE     @@1
@@2:
        RET
ENDP

PROC    @TView@writeChar$qsszcucs
        ARG     thisPtr : DWORD, X : WORD, Y : WORD, C : WORD, Color : WORD, Count : WORD
        USES    SI,DI

        MOV     AX,[WORD PTR thisPtr]
        MOV     [WORD PTR wArgs.Self],AX
        MOV     AX,[WORD PTR thisPtr+2]
        MOV     [(WORD PTR wArgs.Self)+2],AX

        PUSH    [WORD PTR Color]
        PUSH    [WORD PTR thisPtr+2]
        PUSH    [WORD PTR thisPtr]
        CALL    @TView@mapColor$quc
        ADD             SP,6
        MOV     AH,AL
        MOV     AL,[BYTE PTR C]
        MOV     CX,[Count]
        OR      CX,CX
        JLE     @@2
        CMP     CX,256
        JLE     @@1
        MOV     CX,256
@@1:
        MOV     DI,CX
        SHL     DI,1
        SUB     SP,DI
        PUSH    DI
        MOV     DI,SP
        ADD     DI,2
        PUSH    SS
        POP     ES
        MOV     DX,CX
        CLD
        REP     STOSW
        MOV     CX,DX
        MOV     DI,SP
        ADD     DI,2
        MOV     AX,[Y]
        MOV     BX,[X]
        CALL    @TView@writeView$qv
        POP     DI
        ADD     SP,DI
@@2:
        RET
ENDP

PROC    @TView@writeLine$qssssnxv
        ARG     thisPtr : DWORD, X : WORD, Y : WORD, W : WORD, H : WORD, Buf : DWORD
        USES    SI,DI

        MOV     AX,[WORD PTR thisPtr]
        MOV     [WORD PTR wArgs.Self],AX
        MOV     AX,[WORD PTR thisPtr+2]
        MOV     [(WORD PTR wArgs.Self)+2],AX

        CMP     [H],0
        JLE     @@2
@@1:
        MOV     AX,[Y]
        MOV     BX,[X]
        MOV     CX,[W]
        LES     DI,[Buf]
        CALL    @TView@writeView$qv
        INC     [Y]
        DEC     [H]
        JNE     @@1
@@2:
        RET
ENDP

PROC    @TView@writeStr$qssnxzcuc
        ARG     thisPtr : DWORD, X : WORD, Y : WORD, Strng : DWORD, Color : WORD
        LOCAL   ssize : WORD
        USES    SI,DI

        MOV     AX,[WORD PTR thisPtr]
        MOV     [WORD PTR wArgs.Self],AX
        MOV     AX,[WORD PTR thisPtr+2]
        MOV     [(WORD PTR wArgs.Self)+2],AX

        MOV     DI,[WORD PTR Strng]
        OR      DI,[WORD PTR Strng+2]
        JZ      @@2
        LES     DI,[Strng]
        XOR     AX,AX

        CLD
        MOV     CX,0FFFFh
        REPNE   SCASB
        XCHG    AX,CX
        NOT     AX
        DEC     AX
        CMP     AX,0
        JE      @@2             ; don't write zero length string
        MOV     SI,AX           ; save char count
        SHL     AX,1
        SUB     SP,AX           ; make room for attributed string
        MOV     [ssize],AX

        PUSH    [WORD PTR Color]
        PUSH    [WORD PTR thisPtr+2]
        PUSH    [WORD PTR thisPtr]
        CALL    @TView@mapColor$quc
        ADD     SP,6
        MOV     AH,AL           ; attribute into AH
        MOV     CX,SI           ; char count into CX

        MOV     BX,DS
        LDS     SI,[Strng]
        MOV     DI,SP
        PUSH    SS
        POP     ES
        MOV     DX,CX
@@1:
        LODSB
        STOSW
        LOOP    @@1
        MOV     DS,BX
        MOV     CX,DX
        MOV     DI,SP
        MOV     AX,[Y]
        MOV     BX,[X]
        CALL    @TView@writeView$qv
        ADD     SP,[ssize]
        JMP     @@2
@@3:
        MOV     DS,BX
@@2:
        RET
ENDP

END


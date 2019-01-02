;/*------------------------------------------------------------*/
;/* filename -       sysint.asm                                */
;/*                                                            */
;/* function(s)                                                */
;/*                  TSystemError member function              */
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

        TITLE   SYSINT
        MODEL   LARGE, PROLOG

        PUBLIC  @TSystemError@suspend$qv
        PUBLIC  @TSystemError@resume$qv

        EXTRN   @TSystemError@sysErrorFunc:CODEPTR

IFDEF   PROTECT

        ;EXTRN   TDHasKybd:BYTE
        EXTRN   _biosSeg:WORD
ENDIF

        LOCALS  @@

; Keyboard scan codes

scSpaceKey      EQU     39H
scInsKey        EQU     52H
scDelKey        EQU     53H

; Keyboard shift flags

kbShiftKey      EQU     03H
kbCtrlKey       EQU     04H
kbAltKey        EQU     08H

; ROM BIOS workspace

KeyFlags        EQU     (BYTE PTR 17H)
KeyBufHead      EQU     (WORD PTR 1AH)
KeyBufTail      EQU     (WORD PTR 1CH)
KeyBufOrg       EQU     (WORD PTR 1EH)
KeyBufEnd       EQU     (WORD PTR 3EH)

; DOS function call classes

cNothing        EQU     0       ;No check needed
cName           EQU     2       ;Check name at DS:DX
cHandle         EQU     4       ;Check handle in BX
cDrive          EQU     6       ;Check drive in DL

; Data segment

DATASEG

; Externals

        EXTRN   @TSystemError@ctrlBreakHit:BYTE
        EXTRN   @TSystemError@saveCtrlBreak:BYTE
        EXTRN   @TSystemError@inIDE:BYTE

IFDEF PROTECT

; DS-based variables

SaveInt09       DD      ?       ;Saved INT 09H vector
SaveInt1B       DD      ?       ;Saved INT 1BH vector
SaveInt21       DD      ?       ;Saved INT 21H vector
SaveInt23       DD      ?       ;Saved INT 23H vector
SaveInt24       DD      ?       ;Saved INT 24H vector
SaveInt10       DD      ?       ;Saved INT 10H vector

ENDIF

; Code segment

CODESEG

IFDEF PROTECT
ELSE

; CS-based variables

SaveInt09       DD      ?       ;Saved INT 09H vector
SaveInt1B       DD      ?       ;Saved INT 1BH vector
SaveInt21       DD      ?       ;Saved INT 21H vector
SaveInt23       DD      ?       ;Saved INT 23H vector
SaveInt24       DD      ?       ;Saved INT 24H vector

ENDIF

; Keyboard conversion table

KeyConvertTab   LABEL   BYTE

        DB      scSpaceKey,kbAltKey
        DW      0200H
        DB      scInsKey,kbCtrlKey
        DW      0400H
        DB      scInsKey,kbShiftKey
        DW      0500H
        DB      scDelKey,kbCtrlKey
        DW      0600H
        DB      scDelKey,kbShiftKey
        DW      0700H

KeyConvertCnt   EQU     ($-KeyConvertTab)/4

; DOS function call class table

FuncClassTab    LABEL   BYTE

        DB      cDrive          ;36H - Get disk free space
        DB      cNothing
        DB      cNothing
        DB      cName           ;39H - Make directory
        DB      cName           ;3AH - Remove directory
        DB      cName           ;3BH - Change directory
        DB      cName           ;3CH - Create file
        DB      cName           ;3DH - Open file
        DB      cHandle         ;3EH - Close file
        DB      cHandle         ;3FH - Read file
        DB      cHandle         ;40H - Write file
        DB      cName           ;41H - Delete file
        DB      cHandle         ;42H - Seek file
        DB      cName           ;43H - Change file attributes
        DB      cNothing
        DB      cNothing
        DB      cNothing
        DB      cDrive          ;47H - Get current directory
        DB      cNothing
        DB      cNothing
        DB      cNothing
        DB      cName           ;4BH - Load or execute program
        DB      cNothing
        DB      cNothing
        DB      cName           ;4EH - Find first
        DB      cNothing
        DB      cNothing
        DB      cNothing
        DB      cNothing
        DB      cNothing
        DB      cNothing
        DB      cNothing
        DB      cName           ;56H - Rename file
        DB      cHandle         ;57H - Get/Set file date and time

; Function check routines table

FuncCheckTab    LABEL   WORD

        DW      CheckNothing
        DW      CheckName
        DW      CheckHandle
        DW      CheckDrive

; Install system error handlers

@TSystemError@resume$qv proc far
        PUSH    SI
        PUSH    DI
        MOV     AX,3300H
        INT     21H
        MOV     @TSystemError@saveCtrlBreak,DL
        MOV     AX,3301H
        MOV     DL,0
        INT     21H
        PUSH    DS
IFDEF PROTECT
if 0

        JMP     SHORT @@P00

PROTINSTALL:
        INT     21H                     ; get current vector
        XCHG    BX,CX
        MOV     DS:[BX],CX              ; save
        MOV     DS:[BX+2],ES
        MOV     AH,25H                  ; set new vector
        PUSH    DS
        PUSH    CS
        POP     DS
        INT     21H
        POP     DS
        RETN

@@P00:  
        MOV     AX,SEG DGROUP
        MOV     DS,AX

        ;CMP     TDHasKybd,0
        ;JNZ     @@P01

        LEA     CX,SaveInt09
        LEA     DX,Int09Handler
        MOV     AX,3509H
        CALL    NEAR PTR PROTINSTALL
@@P01:
        LEA     CX,SaveInt1B
        LEA     DX,Int1BHandler
        MOV     AX,351BH
        CALL    NEAR PTR PROTINSTALL

        LEA     CX,SaveInt21
        LEA     DX,Int21Handler
        MOV     AX,3521H
        CALL    NEAR PTR PROTINSTALL

        LEA     CX,SaveInt23
        LEA     DX,Int23Handler
        MOV     AX,3523H
        CALL    NEAR PTR PROTINSTALL

        LEA     CX,SaveInt24
        LEA     DX,Int24Handler
        MOV     AX,3524H
        CALL    NEAR PTR PROTINSTALL

        LEA     CX,SaveInt10
        LEA     DX,Int10Handler
        MOV     AX,3510H
        CALL    NEAR PTR PROTINSTALL

        MOV     AH,0BH
        INT     21H

        PUSH    DS
        MOV     DX,word ptr [SaveInt10]
        MOV     DS,word ptr [SaveInt10+2]
        MOV     AX,2510H
        INT     21H
        POP     DS
endif
ELSE
        XOR     AX,AX
        MOV     DS,AX
        MOV     DI,OFFSET SaveInt09
        PUSH    CS
        POP     ES
        CLD
        CLI
        MOV     SI,09H*4
        MOVSW
        MOVSW
        MOV     SI,1BH*4
        MOVSW
        MOVSW
        MOV     SI,21H*4
        MOVSW
        MOVSW
        MOV     SI,23H*4
        MOVSW
        MOVSW
        MOVSW
        MOVSW

        MOV     AX,SEG @TSystemError@inIDE
        MOV     ES,AX
        CMP     ES:@TSystemError@inIDE,0
        JNE     @@2

@@1:    MOV     WORD PTR DS:[09H*4+0],OFFSET Int09Handler
        MOV     WORD PTR DS:[09H*4+2],CS
@@2:    MOV     WORD PTR DS:[1BH*4+0],OFFSET Int1BHandler
        MOV     WORD PTR DS:[1BH*4+2],CS
        MOV     AX,DS:[410H]
        AND     AX,0C1H
        DEC     AX
        JNE     @@3
        MOV     WORD PTR DS:[21H*4+0],OFFSET Int21Handler
        MOV     WORD PTR DS:[21H*4+2],CS
@@3:    MOV     WORD PTR DS:[23H*4+0],OFFSET Int23Handler
        MOV     WORD PTR DS:[23H*4+2],CS
        MOV     WORD PTR DS:[24H*4+0],OFFSET Int24Handler
        MOV     WORD PTR DS:[24H*4+2],CS
        STI
        MOV     AX,CS
        XCHG    AX,WORD PTR DS:[10H*4+2]
        PUSH    AX
        MOV     AX,OFFSET CS:Int10Handler
        XCHG    AX,WORD PTR DS:[10H*4+0]
        PUSH    AX
        MOV     AH,0BH
        INT     21H
        POP     DS:WORD PTR [10H*4+0]
        POP     DS:WORD PTR [10H*4+2]
ENDIF
        POP     DS
        POP     DI
        POP     SI
        RET
@TSystemError@resume$qv endp

; Remove system error handlers

@TSystemError@suspend$qv proc far

        PUSH    SI
        PUSH    DI
        PUSH    DS

IFDEF PROTECT
if 0
        PUSH    DS
        MOV     DX,word ptr [SaveInt09]
        MOV     DS,word ptr [SaveInt09+2]
        MOV     AX,2509H
        INT     21H
        POP     DS

        PUSH    DS
        MOV     DX,word ptr [SaveInt1B]
        MOV     DS,word ptr [SaveInt1B+2]
        MOV     AX,251BH
        INT     21H
        POP     DS

        PUSH    DS
        MOV     DX,word ptr [SaveInt21]
        MOV     DS,word ptr [SaveInt21+2]
        MOV     AX,2521H
        INT     21H
        POP     DS

        PUSH    DS
        MOV     DX,word ptr [SaveInt23]
        MOV     DS,word ptr [SaveInt23+2]
        MOV     AX,2523H
        INT     21H
        POP     DS

        PUSH    DS
        MOV     DX,word ptr [SaveInt24]
        MOV     DS,word ptr [SaveInt24+2]
        MOV     AX,2524H
        INT     21H
        POP     DS
endif
ELSE
        MOV     SI,OFFSET SaveInt09
        PUSH    CS
        POP     DS
        XOR     AX,AX
        MOV     ES,AX
        CLD
        CLI
        MOV     DI,09H*4
        MOVSW
        MOVSW
        MOV     DI,1BH*4
        MOVSW
        MOVSW
        MOV     DI,21H*4
        MOVSW
        MOVSW
        MOV     DI,23H*4
        MOVSW
        MOVSW
        MOVSW
        MOVSW
        STI
ENDIF
        POP     DS
        MOV     AX,3301H
        MOV     DL,@TSystemError@saveCtrlBreak
        INT     21H
        POP     DI
        POP     SI
        RET
@TSystemError@suspend$qv endp

; INT 09H handler signature

        DB      'TVI9'

; INT 09H handler

Int09Handler proc far

        PUSH    DS
        PUSH    DI
        PUSH    AX
IFDEF PROTECT
        MOV     AX,SEG DGROUP
        MOV     DS,AX
        MOV     DS,DS:_biosSeg
ELSE
        MOV     AX,40H
        MOV     DS,AX
ENDIF
        MOV     DI,DS:KeyBufTail
        IN      AL,60H
        MOV     AH,DS:KeyFlags
        PUSHF
        CALL    SaveInt09
        TEST    AL,80H
        JNE     @@9
        PUSH    SI
        PUSH    CX
        MOV     SI,OFFSET CS:KeyConvertTab
        MOV     CX,KeyConvertCnt
@@1:    CMP     AL,CS:[SI]
        JNE     @@2
        TEST    AH,CS:[SI+1]
        JNE     @@3
@@2:    ADD     SI,4
        LOOP    @@1
        JMP     SHORT @@8
@@3:    CMP     DI,DS:KeyBufTail
        JNE     @@5
        MOV     AX,DI
        INC     AX
        INC     AX
        CMP     AX,OFFSET KeyBufEnd
        JNE     @@4
        MOV     AX,OFFSET KeyBufOrg
@@4:    CMP     AX,DS:KeyBufHead
        JE      @@8
        MOV     DS:KeyBufTail,AX
        MOV     DI,AX
@@5:    MOV     AX,CS:[SI+2]
        MOV     DS:[DI],AX
@@8:    POP     CX
        POP     SI
@@9:    POP     AX
        POP     DI
        POP     DS
        IRET
Int09Handler endp

; INT 1BH handler

Int1BHandler proc far

        PUSH    DS
        PUSH    AX
        XOR     AX,AX
        MOV     DS,AX
        AND     BYTE PTR DS:[471H],7FH
        MOV     AX,SEG DGROUP
        MOV     DS,AX
        MOV     @TSystemError@ctrlBreakHit,1
        POP     AX
        POP     DS
        IRET
Int1BHandler endp

; INT 21H handler

Int21Handler proc far

        PUSHF
        STI
        CMP     AH,36H
        JB      @@1
        CMP     AH,57H
        JA      @@1
        PUSH    DX
        PUSH    BX
        MOV     BL,AH
        XOR     BH,BH
        MOV     BL,CS:FuncClassTab[BX-36H]
        CALL    CS:FuncCheckTab[BX]
        POP     BX
        POP     DX
        JC      @@2
@@1:    POPF
        JMP     SaveInt21
@@2:    POPF
        STI
        CMP     AH,36H
        MOV     AX,0FFFFH
        JE      @@3
        MOV     AX,5
@@3:    STC
        RETF    2
Int21Handler endp

; Check filename

CheckName:

        MOV     BX,DX
        MOV     DX,[BX]
        AND     DL,1FH
        DEC     DL
        CMP     DH,':'
        JE      CheckAbsDrive
        JMP     SHORT CheckCurDrive

; Check handle

CheckHandle:

        MOV     BX,SP
        MOV     BX,SS:[BX+2]
        PUSH    AX
        MOV     AX,4400H
        PUSHF
        CALL    SaveInt21
        POP     AX
        OR      DL,DL
        JNS     CheckAbsDrive
        JMP     SHORT CheckNothing

; Check drive

CheckDrive:

        DEC     DL
        JNS     CheckAbsDrive

; Check current drive

CheckCurDrive:

        PUSH    AX
        MOV     AH,19H
        PUSHF
        CALL    SaveInt21
        MOV     DL,AL
        POP     AX

; Check absolute drive
; In    DL = Drive (0=A, 1=B, etc)
; Out   CF = 1 if drive swap failed

CheckAbsDrive:

        CMP     DL,2
        JAE     CheckNothing
        PUSH    DS
        PUSH    AX

IFDEF PROTECT
        MOV     AX,SEG DGROUP
        MOV     DS,AX
        MOV     DS,DS:_biosSeg
ELSE
        MOV     AX,40H
        MOV     DS,AX
ENDIF

        MOV     AL,DS:[104H]
        CMP     AL,0FFH
        JE      @@1
        CMP     DL,AL
        JE      @@1
        PUSH    ES
        PUSH    DS
        PUSH    DI
        PUSH    SI
        PUSH    DX
        PUSH    CX
        MOV     AX,SEG DGROUP
        MOV     DS,AX
        ASSUME  DS:DGROUP
        MOV     AX,15
        PUSH    AX
        PUSH    DX
        MOV     AX,SEG @TSystemError@sysErrorFunc
        MOV     ES,AX
        CALL    [ES:@TSystemError@sysErrorFunc]
        POP     CX
        POP     DX
        POP     SI
        POP     DI
        POP     DS
        POP     ES
        NEG     AX
        JC      @@1
        MOV     DS:[504H],DL
@@1:    POP     AX
        POP     DS

; No check required

CheckNothing:

        RET

; INT 23H and temporary INT 10H handler

Int10Handler:
Int23Handler:

        IRET

; INT 24H handler

Int24Handler proc far

        STI                             ;Enable interrupts
        PUSH    ES                      ;Save registers
        PUSH    DS
        PUSH    BP
        PUSH    DI
        PUSH    SI
        PUSH    DX
        PUSH    CX
        PUSH    BX
        AND     DI,0FFH                 ;Error code in low byte
        PUSH    DI                      ;Save error code
        CMP     DI,9                    ;Printer out of paper
        JE      @@0                     ;Yes, @@0
        TEST    AH,80H                  ;Disk error?
        JE      @@1                     ;Yes, @@1
        MOV     DI,13                   ;Bad memory image of FAT
        MOV     DS,BP                   ;Point DS:SI to device header
        TEST    BYTE PTR DS:[SI+5],80H  ;Block device?
        JE      @@1                     ;Yes, @@0
        INC     DI                      ;Device access error
@@0:    MOV     AL,0FFH                 ;No drive code
@@1:    MOV     DX,SEG DGROUP           ;Setup DS
        MOV     DS,DX
        PUSH    AX                      ;Push drive code
        PUSH    DI                      ;Push error code
        MOV     AX,SEG @TSystemError@sysErrorFunc
        MOV     ES,AX
        CALL    [ES:@TSystemError@sysErrorFunc] ;Call system error handler
        ADD     SP,4                    ;Clean up arguments
        POP     DI                      ;Restore error code
        OR      AX,AX                   ;Zero if retry
        MOV     AX,1                    ;Retry return code
        JE      @@3                     ;Jump if retry
        ADD     SP,(8+3)*2              ;Remove saved regs and INT
        POP     AX                      ;Get INT 21H AX register
        ADD     DI,19                   ;Return AX = 19..31
        CMP     AH,39H                  ;DOS 2.0 style function?
        JAE     @@2                     ;Yes, @@1
        MOV     DI,0FFFFH               ;Return AX = 0FFFFH
@@2:    MOV     AH,54H                  ;Dummy function call to get
        INT     21H                     ;DOS into a stable state
        MOV     AX,DI                   ;Get return code
        MOV     BP,SP                   ;Set CF in return flags
        OR      BYTE PTR [BP+20],1
@@3:    POP     BX                      ;Restore registers
        POP     CX
        POP     DX
        POP     SI
        POP     DI
        POP     BP
        POP     DS
        POP     ES
        IRET
Int24Handler endp

        END

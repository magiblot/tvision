;/*------------------------------------------------------------*/
;/* filename -       edits.asm                                 */
;/*                                                            */
;/* function(s)                                                */
;/*                  TEditor member functions                  */
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

        PUBLIC @TEditor@bufChar$qus
        PUBLIC @TEditor@bufPtr$qus
        PUBLIC @TEditor@formatLine$qnvusius
        PUBLIC @TEditor@lineEnd$qus
        PUBLIC @TEditor@lineStart$qus
        PUBLIC @TEditor@nextChar$qus
        PUBLIC @TEditor@prevChar$qus

CODESEG

;char TEditor::bufChar( ushort P )
PROC    @TEditor@bufChar$qus

        ARG     thisPtr : FAR PTR, P : WORD
        USES    DI

        LES DI,[thisPtr]
        MOV BX,[P]
        CMP BX,[ES:DI+TEditorCurPtr]
        JB  @@1
        ADD BX,[ES:DI+TEditorGapLen]
@@1:    LES DI,[ES:DI+TEditorBuffer]
        MOV AL,[ES:DI+BX]
        RET
ENDP

;function TEditor.bufPtr(P: Word): Word; assembler;
PROC    @TEditor@bufPtr$qus

        ARG     thisPtr : FAR PTR, P : WORD
        USES    DI

        LES DI,[thisPtr]
        MOV AX,[P]
        CMP AX,[ES:DI+TEditorCurPtr]
        JB  @@1
        ADD AX,[ES:DI+TEditorGapLen]
@@1:    RET
ENDP

;void TEditor::formatLine( void *DrawBuf,
;                          ushort LinePtr,
;                          int Width,
;                          ushort Colors
;                        )
PROC    @TEditor@formatLine$qnvusius

        ARG     thisPtr : FAR PTR, DrawBuf : FAR PTR, LinePtr : WORD, W : WORD, Colors : WORD
        USES    DS,SI,DI

        LDS BX,[thisPtr]
        LES DI,[DrawBuf]
        MOV SI,[LinePtr]
        XOR DX,DX
        CLD
        MOV AH,[BYTE PTR Colors]
        MOV CX,[DS:BX+TEditorSelStart]
        CALL    @@10
        MOV AH,[BYTE PTR Colors+1]
        MOV CX,[DS:BX+TEditorCurPtr]
        CALL    @@10
        ADD SI,[DS:BX+TEditorGapLen]
        MOV CX,[DS:BX+TEditorSelEnd]
        ADD CX,[DS:BX+TEditorGapLen]
        CALL    @@10
        MOV AH,[BYTE PTR Colors]
        MOV CX,[DS:BX+TEditorBufSize]
        CALL    @@10
        JMP @@31
@@10:   SUB CX,SI
        JA  @@11
        RETN
@@11:   LDS BX,[DS:BX+TEditorBuffer]
        ADD SI,BX
        MOV BX,[W]
@@12:   LODSB
        CMP AL,' '
        JB  @@20
@@13:   STOSW
        INC DX
@@14:   CMP DX,BX
        JAE @@30
        LOOP    @@12
        LDS BX,[thisPtr]
        SUB SI,[WORD PTR DS:BX+TEditorBuffer]
        RETN
@@20:   CMP AL,0DH
        JE  @@30
        CMP AL,09H
        JNE @@13
        MOV AL,' '
@@21:   STOSW
        INC DX
        TEST    DL,7
        JNE @@21
        JMP @@14
@@30:   POP CX
@@31:   MOV AL,' '
        MOV CX,[W]
        SUB CX,DX
        JBE @@32
        REP STOSW
@@32:   RET
ENDP

;function TEditor.lineEnd(P: Word): Word; assembler;
PROC    @TEditor@lineEnd$qus

        ARG     thisPtr : FAR PTR, P : WORD
        USES    DS,SI,DI

        LDS SI,[thisPtr]
        LES BX,[DS:SI+TEditorBuffer]
        MOV DI,[P]
        MOV AL,0DH
        CLD
        MOV CX,[DS:SI+TEditorCurPtr]
        SUB CX,DI
        JBE @@1
        ADD DI,BX
        REPNE   SCASB
        JE  @@2
        MOV DI,[DS:SI+TEditorCurPtr]
@@1:    MOV CX,[DS:SI+TEditorBufLen]
        SUB CX,DI
        JCXZ    @@4
        ADD BX,[DS:SI+TEditorGapLen]
        ADD DI,BX
        REPNE   SCASB
        JNE @@3
@@2:    DEC DI
@@3:    SUB DI,BX
@@4:    MOV AX,DI
        RET
ENDP

;function TEditor.lineStart(P: Word): Word; assembler;
PROC    @TEditor@lineStart$qus

        ARG     thisPtr : FAR PTR, P : WORD
        USES    DS,SI,DI

        LDS SI,[thisPtr]
        LES BX,[DS:SI+TEditorBuffer]
        MOV DI,[P]
        MOV AL,0DH
        STD
        MOV CX,DI
        SUB CX,[DS:SI+TEditorCurPtr]
        JBE @@1
        ADD BX,[DS:SI+TEditorGapLen]
        ADD DI,BX
        DEC DI
        REPNE   SCASB
        JE  @@2
        SUB BX,[DS:SI+TEditorGapLen]
        MOV DI,[DS:SI+TEditorCurPtr]
@@1:    MOV CX,DI
        JCXZ    @@4
        ADD DI,BX
        DEC DI
        REPNE   SCASB
        JNE @@3
@@2:    INC DI
        INC DI
        SUB DI,BX
        CMP DI,[DS:SI+TEditorCurPtr]
        JE  @@4
        CMP DI,[DS:SI+TEditorBufLen]
        JE  @@4
        CMP [BYTE PTR ES:BX+DI],0AH
        JNE @@4
        INC DI
        JMP @@4
@@3:    XOR DI,DI
@@4:    MOV AX,DI
        CLD
        RET
ENDP

;function TEditor.nextChar(P: Word): Word; assembler;
PROC    @TEditor@nextChar$qus

        ARG     thisPtr : FAR PTR, P : WORD
        USES    DS,SI,DI

        LDS SI,[thisPtr]
        MOV DI,[P]
        CMP DI,[DS:SI+TEditorBufLen]
        JE  @@2
        INC DI
        CMP DI,[DS:SI+TEditorBufLen]
        JE  @@2
        LES BX,[DS:SI+TEditorBuffer]
        CMP DI,[DS:SI+TEditorCurPtr]
        JB  @@1
        ADD BX,[DS:SI+TEditorGapLen]
@@1:    CMP [WORD PTR ES:BX+DI-1],0A0DH
        JNE @@2
        INC DI
@@2:    MOV AX,DI
        RET
ENDP

;function TEditor.prevChar(P: Word): Word; assembler;
PROC    @TEditor@prevChar$qus

        ARG     thisPtr : FAR PTR, P : WORD
        USES    DS,SI,DI

        LDS SI,[thisPtr]
        MOV DI,[P]
        OR  DI,DI
        JE  @@2
        DEC DI
        JE  @@2
        LES BX,[DS:SI+TEditorBuffer]
        CMP DI,[DS:SI+TEditorCurPtr]
        JB  @@1
        ADD BX,[DS:SI+TEditorGapLen]
@@1:    CMP [WORD PTR ES:BX+DI-1],0A0DH
        JNE @@2
        DEC DI
@@2:    MOV AX,DI
        RET
ENDP

END


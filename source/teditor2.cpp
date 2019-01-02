/*------------------------------------------------------------*/
/* filename - teditor2.cpp                                    */
/*                                                            */
/* function(s)                                                */
/*            TEditor member functions                        */
/*------------------------------------------------------------*/

/*------------------------------------------------------------*/
/*                                                            */
/*    Turbo Vision -  Version 1.0                             */
/*                                                            */
/*                                                            */
/*    Copyright (c) 1991 by Borland International             */
/*    All Rights Reserved.                                    */
/*                                                            */
/*------------------------------------------------------------*/

#define Uses_TKeys
#define Uses_TEditor
#define Uses_TIndicator
#define Uses_TEvent
#define Uses_TScrollBar
#define Uses_TFindDialogRec
#define Uses_TReplaceDialogRec
#define Uses_opstream
#define Uses_ipstream
#include <tv.h>

#if !defined( __STRING_H )
#include <string.h>
#endif  // __STRING_H

#if !defined( __CTYPE_H )
#include <ctype.h>
#endif  // __CTYPE_H

#if !defined( __DOS_H )
#include <Dos.h>
#endif  // __DOS_H

inline int isWordChar( int ch )
{
    return isalnum(ch) || ch == '_';
}

#pragma warn -asc

int countLines( void *buf, ushort count )
{
asm {
    LES DI,buf
    MOV CX,count
    XOR DX,DX
    MOV AL,0Dh
    CLD
    }
__1:
asm {
    JCXZ __2
    REPNE SCASB
    JNE __2
    INC DX
    JMP __1
    }
__2:
    return _DX;
}

ushort scan( const void *block, ushort size, const char *str )
{
    ushort len = strlen( str );
asm {
    PUSH DS
    LES DI,block
    LDS SI,str
    MOV CX,size
    JCXZ __3
    CLD
    MOV AX,len
    CMP AX,1
    JB __5
    JA __1
    LODSB           // searching for a single character
    REPNE SCASB
    JNE __3
    JMP __5
    }
__1:
asm {
    MOV BX,AX
    DEC BX
    MOV DX,CX
    SUB DX,AX
    JB  __3
    LODSB
    INC DX
    INC DX
    }
__2:
asm {
    DEC DX
    MOV CX,DX
    REPNE SCASB
    JNE __3
    MOV DX,CX
    MOV CX,BX
    REP CMPSB
    JE  __4
    SUB CX,BX
    ADD SI,CX
    ADD DI,CX
    INC DI
    OR  DX,DX
    JNE __2
    }
__3:
asm {
    XOR AX,AX
    JMP __6
    }
__4:
asm SUB DI,BX
__5:
asm {
    MOV AX,DI
    SUB AX,WORD PTR block
    }
__6:
asm {
    DEC AX
    POP DS
    }
    return _AX;
}

ushort iScan( const void *block, ushort size, const char *str )
{
    char s[256];
    ushort len = strlen( str );
asm {
    PUSH DS
    MOV AX,SS
    MOV ES,AX
    LEA DI,s
    LDS SI,str
    MOV AX,len;
    MOV CX,AX
    MOV BX,AX
    JCXZ __9
    }
__1:
asm {
    LODSB
    CMP AL,'a'
    JB  __2
    CMP AL,'z'
    JA  __2
    SUB AL,20h
    }
__2:
asm {
    STOSB
    LOOP __1
    SUB DI,BX
    LDS SI,block
    MOV CX,size
    JCXZ __8
    CLD
    SUB CX,BX
    JB  __8
    INC CX
    }
__4:
asm {
    MOV AH,ES:[DI]
    AND AH,0xDF
    }
__5:
asm {
    LODSB
    AND AL,0xDF
    CMP AL,AH
    LOOPNE  __5
    JNE __8
    DEC SI
    MOV DX,CX
    MOV CX,BX
    }
__6:
asm {
    REPE CMPSB
    JE  __10
    MOV AL,DS:[SI-1]
    CMP AL,'a'
    JB  __7
    CMP AL,'z'
    JA  __7
    SUB AL,20h
    }
__7:
asm {
    CMP AL,ES:[DI-1]
    JE  __6
    SUB CX,BX
    ADD SI,CX
    ADD DI,CX
    INC SI
    MOV CX,DX
    JNE __4
    }
__8:
asm {
    XOR AX,AX
    JMP __11
    }
__9:
asm {
    MOV AX, 1
    JMP __11
    }
__10:
asm {
    SUB SI,BX
    MOV AX,SI
    SUB AX,wORD PTR block
    INC AX
    }
__11:
asm {
    DEC AX
    POP DS
    }
    return _AX;
}

#pragma warn .asc

Boolean TEditor::hasSelection()
{
    return Boolean(selStart != selEnd);
}

void TEditor::hideSelect()
{
    selecting = False;
    setSelect(curPtr, curPtr, False);
}

void TEditor::initBuffer()
{
    buffer = new char[bufSize];
}

Boolean TEditor::insertBuffer( char *p,
                               ushort offset,
                               ushort length,
                               Boolean allowUndo,
                               Boolean selectText
                             )
{
    selecting = False;
    ushort selLen = selEnd - selStart;
    if( selLen == 0 && length == 0 )
        return True;

    ushort delLen = 0;
    if( allowUndo == True )
        if( curPtr == selStart )
            delLen = selLen;
        else
            if( selLen > insCount )
                delLen = selLen - insCount;

    long newSize = long(bufLen + delCount - selLen + delLen) + length;

    if( newSize > bufLen + delCount )
        if( newSize > 0xFFE0l || setBufSize(ushort(newSize)) == False )
            {
            editorDialog( edOutOfMemory );
            return False;
            }

    ushort selLines = countLines( &buffer[bufPtr(selStart)], selLen );
    if( curPtr == selEnd )
        {
        if( allowUndo == True )
            {
            if( delLen > 0 )
                memmove( 
                         &buffer[curPtr + gapLen - delCount - delLen],
                         &buffer[selStart],
                         delLen
                       );
            insCount -= selLen - delLen;
            }
        curPtr = selStart;
        curPos.y -= selLines;
        }
    if( delta.y > curPos.y )
        {
        delta.y -= selLines;
        if( delta.y < curPos.y )
            delta.y = curPos.y;
        }

    if( length > 0 )
        memmove(
                &buffer[curPtr],
                &p[offset],
                length
               );

    ushort lines = countLines( &buffer[curPtr], length );
    curPtr += length;
    curPos.y += lines;
    drawLine = curPos.y;
    drawPtr = lineStart(curPtr);
    curPos.x = charPos(drawPtr, curPtr);
    if( selectText == False )
        selStart = curPtr;
    selEnd = curPtr;
    bufLen += length - selLen;
    gapLen -= length - selLen;
    if( allowUndo == True )
        {
        delCount += delLen;
        insCount += length;
        }
    limit.y += lines - selLines;
    delta.y = max(0, min(delta.y, limit.y - size.y));
    if( isClipboard() == False )
        modified = True;
    setBufSize(bufLen + delCount);
    if( selLines == 0 && lines == 0 )
        update(ufLine);
    else
        update(ufView);
    return True;
}

Boolean TEditor::insertFrom( TEditor *editor )
{
    return insertBuffer( editor->buffer,
                         editor->bufPtr(editor->selStart),
                         editor->selEnd - editor->selStart,
                         canUndo,
                         isClipboard()
                        );
}

Boolean TEditor::insertText( const void *text, ushort length, Boolean selectText )
{
  return insertBuffer( (char *)text, 0, length, canUndo, selectText);
}

Boolean TEditor::isClipboard()
{
    return Boolean(clipboard == this);
}

ushort TEditor::lineMove( ushort p, int count )
{
    ushort i = p;
    p = lineStart(p);
    int pos = charPos(p, i);
    while( count != 0 )
        {
        i = p;
        if( count < 0 )
            {
            p = prevLine(p);
            count++;
            }
        else
            {
            p = nextLine(p);
            count--;
            }
        }
    if( p != i )
        p = charPtr(p, pos);
    return p;
}

void TEditor::lock()
{
    lockCount++;
}

void TEditor::newLine()
{
    const char crlf[] = "\x0D\x0A";
    ushort p = lineStart(curPtr);
    ushort i = p;
    while( i < curPtr &&
           ( (buffer[i] == ' ') || (buffer[i] == '\x9'))
         )
         i++;
    insertText(crlf, 2, False);
    if( autoIndent == True )
        insertText( &buffer[p], i - p, False);
}

ushort TEditor::nextLine( ushort p )
{
    return nextChar(lineEnd(p));
}

ushort TEditor::nextWord( ushort p )
{
    while( p < bufLen && isWordChar(bufChar(p)) != 0 )
        p = nextChar(p);
    while( p < bufLen && isWordChar(bufChar(p)) == 0 )
        p = nextChar(p);
    return p;
}

ushort TEditor::prevLine( ushort p )
{
  return lineStart(prevChar(p));
}

ushort TEditor::prevWord( ushort p )
{
    while( p > 0 && isWordChar(bufChar(prevChar(p))) == 0 )
        p = prevChar(p);
    while( p > 0 && isWordChar(bufChar(prevChar(p))) != 0 )
        p = prevChar(p);
    return p;
}

void TEditor::replace()
{
    TReplaceDialogRec replaceRec( findStr, replaceStr, editorFlags );
    if( editorDialog( edReplace, &replaceRec ) != cmCancel )
        {
        strcpy( findStr, replaceRec.find );
        strcpy( replaceStr, replaceRec.replace );
        editorFlags = replaceRec.options | efDoReplace;
        doSearchReplace();
        }

}

void TEditor::scrollTo( int x, int y )
{
    x = max(0, min(x, limit.x - size.x));
    y = max(0, min(y, limit.y - size.y));
    if( x != delta.x || y != delta.y )
        {
        delta.x = x;
        delta.y = y;
        update(ufView);
        }
}

Boolean TEditor::search( const char *findStr, ushort opts )
{
    ushort pos = curPtr;
    ushort i;
    do  {
        if( (opts & efCaseSensitive) != 0 )
            i = scan( &buffer[bufPtr(pos)], bufLen - pos, findStr);
        else
            i = iScan( &buffer[bufPtr(pos)], bufLen - pos, findStr);

        if( i != sfSearchFailed )
            {
            i += pos;
            if( (opts & efWholeWordsOnly) == 0 ||
                !(
                    ( i != 0 && isWordChar(bufChar(i - 1)) != 0 ) ||
                    ( i + strlen(findStr) != bufLen &&
                        isWordChar(bufChar(i + strlen(findStr)))
                    )
                 ))
                {
                lock();
                setSelect(i, i + strlen(findStr), False);
                trackCursor(Boolean(!cursorVisible()));
                unlock();
                return True;
                }
            else
                pos = i + 1;
            }
        } while( i != sfSearchFailed );
    return False;
}

void TEditor::setBufLen( ushort length )
{
    bufLen = length;
    gapLen = bufSize - length;
    selStart = 0;
    selEnd = 0;
    curPtr = 0;
    delta.x = 0;
    delta.y = 0;
    curPos = delta;
    limit.x = maxLineLength;
    limit.y = countLines( &buffer[gapLen], bufLen ) + 1;
    drawLine = 0;
    drawPtr = 0;
    delCount = 0;
    insCount = 0;
    modified = False;
    update(ufView);
}

Boolean TEditor::setBufSize( ushort newSize )
{
    return Boolean(newSize <= bufSize);
}

void TEditor::setCmdState( ushort command, Boolean enable )
{
    TCommandSet s;
    s += command;
    if( enable == True && (state & sfActive) != 0 )
        enableCommands(s);
    else
        disableCommands(s);
}

void TEditor::setCurPtr( ushort p, uchar selectMode )
{
    ushort anchor;
    if( (selectMode & smExtend) == 0 )
        anchor = p;
    else if( curPtr == selStart )
        anchor = selEnd;
    else
        anchor = selStart;

    if( p < anchor )
        {
        if( (selectMode & smDouble) != 0 )
            {
            p = prevLine(nextLine(p));
            anchor = nextLine(prevLine(anchor));
            }
        setSelect(p, anchor, True);
        }
    else
        {
        if( (selectMode & smDouble) != 0 )
            {
            p = nextLine(p);
            anchor = prevLine(nextLine(anchor));
            }
        setSelect(anchor, p, False);
        }
}

void TEditor::setSelect( ushort newStart, ushort newEnd, Boolean curStart )
{
    ushort p;
    if( curStart != 0 )
        p = newStart;
    else
        p = newEnd;

    uchar flags = ufUpdate;

    if( newStart != selStart || newEnd != selEnd )
        if( newStart != newEnd || selStart != selEnd )
            flags = ufView;

    if( p != curPtr )
        {
        if( p > curPtr )
            {
            ushort l = p - curPtr;
            memmove( &buffer[curPtr], &buffer[curPtr + gapLen], l);
            curPos.y += countLines(&buffer[curPtr], l);
            curPtr = p;
            }
        else
            {
            ushort l = curPtr - p;
            curPtr = p;
            curPos.y -= countLines(&buffer[curPtr], l);
            memmove( &buffer[curPtr + gapLen], &buffer[curPtr], l);
            }
        drawLine = curPos.y;
        drawPtr = lineStart(p);
        curPos.x = charPos(drawPtr, p);
        delCount = 0;
        insCount = 0;
        setBufSize(bufLen);
    }
    selStart = newStart;
    selEnd = newEnd;
    update(flags);
}

void TEditor::setState( ushort aState, Boolean enable )
{
    TView::setState(aState, enable);
    switch( aState )
        {
        case sfActive:
            if( hScrollBar != 0 )
                hScrollBar->setState(sfVisible, enable);
            if( vScrollBar != 0 )
                vScrollBar->setState(sfVisible, enable);
            if( indicator != 0 )
                indicator->setState(sfVisible, enable);
            updateCommands();
            break;

        case sfExposed:
            if( enable == True )
                unlock();
        }
}

void TEditor::startSelect()
{
    hideSelect();
    selecting = True;
}

void TEditor::toggleInsMode()
{
    overwrite = Boolean(!overwrite);
    setState(sfCursorIns, Boolean(!getState(sfCursorIns)));
}

void TEditor::trackCursor( Boolean center )
{
    if( center == True )
        scrollTo( curPos.x - size.x + 1, curPos.y - size.y / 2);
    else
        scrollTo( max(curPos.x - size.x + 1, min(delta.x, curPos.x)),
                  max(curPos.y - size.y + 1, min(delta.y, curPos.y)));
}

void TEditor::undo()
{
    if( delCount != 0 || insCount != 0 )
        {
        selStart = curPtr - insCount;
        selEnd = curPtr;
        ushort length = delCount;
        delCount = 0;
        insCount = 0;
        insertBuffer(buffer, curPtr + gapLen - length, length, False, True);
        }
}

void TEditor::unlock()
{
    if( lockCount > 0 )
        {
        lockCount--;
        if( lockCount == 0 )
            doUpdate();
        }
}

void TEditor::update( uchar aFlags )
{
    updateFlags |= aFlags;
    if( lockCount == 0 )
        doUpdate();
}

void TEditor::updateCommands()
{
    setCmdState( cmUndo, Boolean( delCount != 0 || insCount != 0 ) );
    if( isClipboard() == False )
        {
        setCmdState(cmCut, hasSelection());
        setCmdState(cmCopy, hasSelection());
        setCmdState(cmPaste,
                    Boolean(clipboard != 0 && (clipboard->hasSelection())) );
        }
    setCmdState(cmClear, hasSelection());
    setCmdState(cmFind, True);
    setCmdState(cmReplace, True);
    setCmdState(cmSearchAgain, True);
}

Boolean TEditor::valid( ushort )
{
  return isValid;
}

void TEditor::write( opstream& os )
{
    TView::write( os );
    os << hScrollBar << vScrollBar << indicator
       << bufSize << (int)canUndo;
}

void *TEditor::read( ipstream& is )
{
    TView::read( is );
    int temp;
    is >> hScrollBar >> vScrollBar >> indicator
       >> bufSize >> temp;
    canUndo = Boolean(temp);
    selecting = False;
    overwrite = False;
    autoIndent = False; 
    lockCount = 0;
    keyState = 0;
    initBuffer();
    if( buffer != 0 )
        isValid = True;
    else
        {
        TEditor::editorDialog( edOutOfMemory, 0 );
        bufSize = 0;
        }
    lockCount = 0; 
    lock();
    setBufLen( 0 );
    return this;
}

TStreamable *TEditor::build()
{
    return new TEditor( streamableInit );
}

TEditor::TEditor( StreamableInit ) : TView( streamableInit )
{
}



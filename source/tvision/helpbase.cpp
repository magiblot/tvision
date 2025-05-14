/*------------------------------------------------------------*/
/* filename -       HelpBase.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  Member function(s) of following classes   */
/*                      THelpTopic                            */
/*                      THelpIndex                            */
/*                      THelpFile                             */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TStreamableClass
#define Uses_TPoint
#define Uses_TStreamable
#define Uses_ipstream
#define Uses_opstream
#define Uses_iopstream
#define Uses_TRect
#define Uses_TText
#include <tvision/tv.h>

#if !defined( __HELPBASE_H )
#include "tvision/helpbase.h"
#endif  // __HELPBASE_H

#if !defined( __UTIL_H )
#include "tvision/util.h"
#endif  // __UTIL_H

#if !defined( __STRING_H )
#include <string.h>
#endif  // __STRING_H

#if !defined( __LIMITS_H )
#include <limits.h>
#endif  // __LIMITS_H

#if !defined( __STAT_H )
#include <sys/stat.h>
#endif  // __STAT_H

#if !defined( __CTYPE_H )
#include <ctype.h>
#endif  // __CTYPE_H

#if !defined( __IOSTREAM_H )
#include <iostream.h>
#endif  // __IOSTREAM_H

#pragma warn -dsz

TCrossRefHandler crossRefHandler = notAssigned;

// THelpTopic

const char * const _NEAR THelpTopic::name = "THelpTopic";

void THelpTopic::write( opstream& os )
{
    writeParagraphs( os );
    writeCrossRefs( os );

}

void *THelpTopic::read( ipstream& is )
{
    readParagraphs( is );
    readCrossRefs( is );
    width = 0;
    lastLine = INT_MAX;
    return this;
}

TStreamable *THelpTopic::build()
{
    return new THelpTopic( streamableInit );
}


TStreamableClass RHelpTopic( THelpTopic::name,
                                  THelpTopic::build,
                                  __DELTA(THelpTopic)
                                );

THelpTopic::THelpTopic() noexcept : TObject()
{
    paragraphs = 0;
    numRefs = 0;
    crossRefs = 0;
    width = 0;
    lastOffset = 0;
    lastLine = INT_MAX;
    lastParagraph = 0;
}

void THelpTopic::readParagraphs( ipstream& s )
{
    int  i;
    ushort size;
    TParagraph **pp;
    int temp;

    s >> i;
    pp = &paragraphs;
    while (i > 0)
    {
        s >> size;
        *pp = new TParagraph;
        (*pp)->text = new char[size + 1];
        (*pp)->size = (ushort) size;
        s >> temp;
        (*pp)->wrap = Boolean(temp);
        s.readBytes((*pp)->text, (*pp)->size);
        (*pp)->text[(*pp)->size] = '\0';
        pp = &((*pp)->next);
        --i;
    }
    *pp = 0;
}

void THelpTopic::readCrossRefs( ipstream& s )
{
    int i;
    TCrossRef *crossRefPtr;

    s >> numRefs;
    crossRefs = new TCrossRef[numRefs];
    for (i = 0; i < numRefs; ++i)
        {
        crossRefPtr = (TCrossRef *)crossRefs + i;
        s >> crossRefPtr->ref >> crossRefPtr->offset >> crossRefPtr->length;
        }
}

void THelpTopic::disposeParagraphs() noexcept
{
    TParagraph *p, *t;

    p = paragraphs;
    while (p != 0)
        {
        t = p;
        p = p->next;
        delete[] t->text;
        delete t;
        }
}


THelpTopic::~THelpTopic()
{
    TCrossRef *crossRefPtr;

    disposeParagraphs();
    if (crossRefs != 0)
       {
       crossRefPtr = (TCrossRef *)crossRefs;
       delete [] crossRefPtr;
       }
}

void THelpTopic::addCrossRef( TCrossRef ref ) noexcept
{
    TCrossRef *p;
    TCrossRef *crossRefPtr;

    p =  new TCrossRef[numRefs+1];
    if (numRefs > 0)
        {
        crossRefPtr = crossRefs;
        memmove(p, crossRefPtr, numRefs * sizeof(TCrossRef));
        delete [] crossRefPtr;
        }
    crossRefs = p;
    crossRefPtr = crossRefs + numRefs;
    *crossRefPtr = ref;
    ++numRefs;
}


void THelpTopic::addParagraph( TParagraph *p ) noexcept
{
    TParagraph  *pp, *back;

    if (paragraphs == 0)
        paragraphs = p;
    else
        {
        pp = paragraphs;
        back = pp;
        while (pp != 0)
            {
            back = pp;
            pp = pp->next;
            }
        back->next = p;
        }
    p->next = 0;
}

void THelpTopic::getCrossRef( int i, TPoint& loc, uchar& length,
                              int& ref ) noexcept
{
    int curOffset, offset, paraOffset;
    TParagraph *p;
    int line;
    TCrossRef *crossRefPtr;

    paraOffset = 0;
    curOffset = 0;
    line = 0;
    crossRefPtr = crossRefs + i;
    offset = crossRefPtr->offset;
    p = paragraphs;
    do  {
        int lineOffset = curOffset;
        wrapText(p->text, p->size, curOffset, p->wrap);
        ++line;
        if (offset <= paraOffset + curOffset)
            {
            int refOffset = offset - (paraOffset + lineOffset) - 1;
            TStringView textBefore(&p->text[lineOffset], refOffset);
            TStringView refText(&p->text[lineOffset + refOffset], crossRefPtr->length);
            loc.x = strwidth(textBefore);
            loc.y = line;
            length = strwidth(refText);
            ref = crossRefPtr->ref;
            return;
            }
        if (curOffset >= p->size)
            {
            paraOffset += p->size;
            p = p->next;
            curOffset = 0;
            }
        } while (True);
}

TStringView THelpTopic::getLine( int line ) noexcept
{
    int offset, i;
    TParagraph *p;

    if (lastLine < line)
        {
        i = line;
        line -= lastLine;
        lastLine = i;
        offset = lastOffset;
        p = lastParagraph;
        }
    else
        {
        p = paragraphs;
        offset = 0;
        lastLine = line;
        }
    while (p != 0)
        {
        while (offset < p->size)
            {
            --line;
            TStringView lineText = wrapText(p->text, p->size, offset, p->wrap);
            if (line == 0)
                {
                lastOffset = offset;
                lastParagraph = p;
                return lineText;
                }
            }
        p = p->next;
        offset = 0;
        }
    return TStringView();
}

int THelpTopic::getNumCrossRefs() noexcept
{
    return numRefs;
}

int THelpTopic::longestLineWidth() noexcept
{
    int maxWidth = 0;
    int lineCount = numLines();

    for (int i = 1; i <= lineCount; ++i)
        {
        TStringView line = getLine(i);
        int lineWidth = strwidth(line);
        if (lineWidth > maxWidth)
            maxWidth = lineWidth;
        }

    return maxWidth;
}

int THelpTopic::numLines() noexcept
{
    int offset, lines;
    TParagraph *p;

    offset = 0;
    lines = 0;
    p = paragraphs;
    while (p != 0)
        {
        offset = 0;
        while (offset < p->size)
            {
            ++lines;
            wrapText(p->text, p->size, offset, p->wrap);
            }
        p = p->next;
        }
    return lines;
}

void THelpTopic::setCrossRef( int i, TCrossRef& ref ) noexcept
{
    TCrossRef *crossRefPtr;

    if (i < numRefs)
        {
        crossRefPtr = crossRefs + i;
        *crossRefPtr = ref;
        }
}


void THelpTopic::setNumCrossRefs( int i ) noexcept
{
    TCrossRef  *p, *crossRefPtr;

    if (numRefs == i)
        return;
    p = new TCrossRef[i];
    if (numRefs > 0)
        {
        crossRefPtr = crossRefs;
        if (i > numRefs)
            memmove(p, crossRefPtr, numRefs * sizeof(TCrossRef));
        else
            memmove(p, crossRefPtr, i * sizeof(TCrossRef));

        delete [] crossRefPtr;
        }
    crossRefs = p;
    numRefs = i;
}


void THelpTopic::setWidth( int aWidth ) noexcept
{
    width = aWidth;
}

void THelpTopic::writeParagraphs( opstream& s )
{
    int i;
    TParagraph  *p;
    int temp;

    p = paragraphs;
    for (i = 0; p != 0; ++i)
        p = p->next;
    s << i;
    for(p = paragraphs; p != 0; p = p->next)
        {
        s << p->size;
        temp = int(p->wrap);
        s << temp;
        s.writeBytes(p->text, p->size);
        }
}


void THelpTopic::writeCrossRefs( opstream& s )
{
    int i;
    TCrossRef *crossRefPtr;

    s << numRefs;
    if (crossRefHandler == notAssigned)
        {
        for(i = 0; i < numRefs; ++i)
            {
            crossRefPtr = crossRefs + i;
            s << crossRefPtr->ref << crossRefPtr->offset << crossRefPtr->length;
            }
        }
    else
        for (i = 0; i < numRefs; ++i)
            {
            crossRefPtr = crossRefs + i;
            (*crossRefHandler)(s, crossRefPtr->ref);
            s << crossRefPtr->offset << crossRefPtr->length;
            }
}

Boolean isBlank( char ch ) noexcept
{
    if (isspace((uchar)ch))
        return True;
    else
        return False;
}

static TStringView getLineAtOffset( char *text, int textSize, int offset ) noexcept
{
    char *lineStart = &text[offset];
    char *lineEnd = (char *) memchr(lineStart, '\n', textSize - offset);
    if (lineEnd == 0)
        lineEnd = &text[textSize];
    else
        ++lineEnd; // Point past '\n'.
    return TStringView(lineStart, lineEnd - lineStart);
}

static TStringView discardTrailingWhitespaces( TStringView str ) noexcept
{
    size_t newSize = str.size();
    while( newSize > 0 )
        {
        if( !isBlank(str[newSize - 1]) )
            break;
        --newSize;
        }
    return str.substr(0, newSize);
}

TStringView THelpTopic::wrapText( char *text, int textSize, int& offset, Boolean wrap ) noexcept
{
    TStringView line = getLineAtOffset(text, textSize, offset);
    if( wrap )
        {
        size_t wrappedSize = TText::scroll(line, width, False);
        if( 0 < wrappedSize && wrappedSize < line.size() )
            {
            size_t newSize = wrappedSize;
            // Omit the last word if it was cut off by wrapping.
            while( newSize > 0 && !isBlank(line[newSize]) )
                --newSize;
            // Unless it fills the whole line.
            if( newSize == 0 )
                newSize = wrappedSize;
            // If a space follows, keep it so 'offset' points past it.
            if( (int) newSize < textSize && isBlank(line[newSize]) )
                ++newSize;
            line = line.substr(0, newSize);
            }
        }
    offset += line.size();
    return discardTrailingWhitespaces(line);
}

// THelpIndex

const char * const _NEAR THelpIndex::name = "THelpIndex";

void THelpIndex::write( opstream& os )
{
    int32_t *indexArrayPtr;

    os << size;
    for (int i = 0; i < size; ++i)
        {
        indexArrayPtr = index + i;
        os << *indexArrayPtr;
        }
}

void *THelpIndex::read( ipstream& is )
{
    int32_t *indexArrayPtr;

    is >> size;
    if (size == 0)
        index = 0;
    else
        {
        index =  new int32_t[size];
        for(int i = 0; i < size; ++i)
            {
            indexArrayPtr = index + i;
            is >> *indexArrayPtr;
            }
        }
    return this;
}

TStreamable *THelpIndex::build()
{
    return new THelpIndex( streamableInit );
}

TStreamableClass RHelpIndex( THelpIndex::name,
                             THelpIndex::build,
                             __DELTA(THelpIndex)
                           );

THelpIndex::~THelpIndex()
{
    delete [] index;
}


THelpIndex::THelpIndex(void) noexcept : TObject ()
{
    size = 0;
    index = 0;
}

int32_t THelpIndex::position(int i) noexcept
{
    int32_t *indexArrayPtr;

    if (i < size)
        {
        indexArrayPtr = index + i;
        return (*indexArrayPtr);
        }
    else
        return -1;
}

void THelpIndex::add( int i, int32_t val )
{
    int delta = 10;
    int32_t *p;
    int newSize;
    int32_t *indexArrayPtr;

    if (i >= size)
        {
        newSize = (i + delta) / delta * delta;
        p = new int32_t[newSize];
        if (p != 0)
            {
            if (index != 0)
                memcpy(p, index, size * sizeof(int32_t));
            memset(p+size, 0xFF, (newSize - size) * sizeof(int32_t));
            }
        delete[] index;
        index = p;
        size = newSize;
        }
    indexArrayPtr = index + i;
    *indexArrayPtr = val;
}

// THelpFile

THelpFile::THelpFile( iopstream &s )
{
    int32_t magic;
    int32_t size;

    magic = 0;
    s.seekg(0, ios::end);
    size = s.tellg();
    s.seekg(0);
    if (size > (int) sizeof(magic))
        s >> magic;
    if (magic == magicHeader && size >= 12)
        {
        s.seekg(8);
        s >> indexPos;
        s.seekg(indexPos);
        s >> index;
        modified = False;
        }
    else
        {
        indexPos = 12;
        index = new THelpIndex;
        modified = True;
        }
    stream = &s;
}

THelpFile::~THelpFile(void)
{
    int32_t magic, size;

    if (modified == True)
        {
        ensureStreamSize(*stream, indexPos);
        stream->seekp(indexPos);
        *stream << index;
        magic = magicHeader;
        stream->seekp(0, ios::end);
        size = stream->tellp() - (streamoff) 8;
        stream->seekp(0);
        *stream << magic;
        *stream << size;
        *stream << indexPos;
        }
    delete stream;
    delete index;
}

THelpTopic *THelpFile::getTopic( int i )
{
    int32_t pos;
    THelpTopic *topic = 0;

    pos = index->position(i);
    if (pos > 0 )
        {
        stream->seekg(pos);
        *stream >> topic;
        return topic;
        }
    else return(invalidTopic());
}

THelpTopic *THelpFile::invalidTopic()
{
    THelpTopic *topic;
    TParagraph *para;

    topic = new THelpTopic;
    para = new TParagraph;
    para->text = newStr(invalidContext);
    para->size = strlen(invalidContext);
    para->wrap = False;
    para->next = 0;
    topic->addParagraph(para);
    return topic;
}

void THelpFile::recordPositionInIndex( int i )
{
    index->add(i, indexPos);
    modified = True;
}

void THelpFile::putTopic( THelpTopic *topic )
{
    ensureStreamSize(*stream, indexPos);
    stream->seekp(indexPos);
    *stream << topic;
    indexPos = stream->tellp();
    modified = True;
}

void THelpFile::ensureStreamSize( iopstream &stream, int desiredSize )
// When the stream is backed by a regular file, 'seekp' can be used to extend
// the file size. However, this behaviour is not guaranteed by the Standard and
// it certainly won't work with devices such as a std::stringbuf. Therefore,
// it's safer to extend the stream by manually writing null bytes.
{
    stream.seekg(0, ios::end);
    streampos currentSize = stream.tellg();
    if (currentSize < desiredSize)
        {
        stream.seekp(0, ios::end);
        for (int i = currentSize; i < desiredSize; ++i)
            stream.writeByte('\0');
        }
}

void notAssigned( opstream& , int )
{
}



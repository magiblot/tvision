/*------------------------------------------------------------*/
/* filename -       HelpBase.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  Member function(s) of following classes   */
/*                      THelpTopic                            */
/*                      THelpIndex                            */
/*                      THelpFile                             */
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

#define Uses_TStreamableClass
#define Uses_TPoint
#define Uses_TStreamable
#define Uses_ipstream
#define Uses_opstream
#define Uses_fpstream
#define Uses_TRect
#include <tv.h>

#if !defined( __HELP_H )
#include "HelpBase.h"
#endif  // __HELP_H

#if !defined( __UTIL_H )
#include "Util.h"
#endif  // __UTIL_H

#if !defined( __STRING_H )
#include <string.h>
#endif  // __STRING_H

#if !defined( __LIMITS_H )
#include <limits.h>
#endif  // __LIMITS_H

#if !defined( __STAT_H )
#include <sys\stat.h>
#endif  // __STAT_H

#if !defined( __CTYPE_H )
#include <ctype.h>
#endif  // __CTYPE_H

#if !defined( __IO_H )
#include <io.h>
#endif  // __IO_H

#pragma warn -dsz

TCrossRefHandler crossRefHandler = notAssigned;

// THelpTopic
const char * const near THelpTopic::name = "THelpTopic";

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

THelpTopic::THelpTopic() : TObject()
{
    paragraphs = 0;
    numRefs = 0;
    crossRefs = 0;
    width = 0;
    lastOffset = 0;
    lastLine = INT_MAX;
    lastParagraph = 0;
};

void THelpTopic::readParagraphs( ipstream& s )
{
    int  i, size;
    TParagraph **pp;
    int temp;

    s >> i;
    pp = &paragraphs;
    while ( i > 0)
    {
        s >> size;
        *pp = new TParagraph;
        (*pp)->text = new char[size];
        (*pp)->size = (ushort) size;
    s >> temp;
        (*pp)->wrap = Boolean(temp);
        s.readBytes((*pp)->text, (*pp)->size);
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
        s.readBytes(crossRefPtr, sizeof(TCrossRef));
        }
}

void THelpTopic::disposeParagraphs()
{
    TParagraph *p, *t;

    p = paragraphs;
    while (p != 0)
        {
        t = p;
        p = p->next;
        delete t->text; 
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
       delete [numRefs] crossRefPtr;
       }
}

void THelpTopic::addCrossRef( TCrossRef ref )
{
    TCrossRef *p;
    TCrossRef *crossRefPtr;

    p =  new TCrossRef[numRefs+1];
    if (numRefs > 0)
        {
        crossRefPtr = crossRefs;
        memmove(p, crossRefPtr, numRefs * sizeof(TCrossRef));
        delete [numRefs] crossRefPtr;
        }
    crossRefs = p;
    crossRefPtr = crossRefs + numRefs;
    *crossRefPtr = ref;
    ++numRefs;
}


void THelpTopic::addParagraph( TParagraph *p )
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
         int& ref )
{
    int oldOffset, curOffset, offset, paraOffset;
    TParagraph *p;
    int line;
    TCrossRef *crossRefPtr;

    paraOffset = 0;
    curOffset = 0;
    oldOffset = 0;
    line = 0;
    crossRefPtr = crossRefs + i;
    offset = crossRefPtr->offset;
    p = paragraphs;
    while (paraOffset + curOffset < offset)
        {
        oldOffset = paraOffset + curOffset;
        wrapText(p->text, p->size, curOffset, p->wrap);
        ++line;
        if (curOffset >= p->size)
            {
            paraOffset += p->size;
            p = p->next;
            curOffset = 0;
            }
        }
    loc.x = offset - oldOffset - 1;
    loc.y = line;
    length = crossRefPtr->length;
    ref = crossRefPtr->ref;
}

char *THelpTopic::getLine( int line )
{
    int offset, i;
    TParagraph *p;
    char buffer[256];

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
    buffer[0] = 0;
    while (p != 0)
    {
        while (offset < p->size)
        {
            --line;
            strcpy(buffer, wrapText(p->text, p->size, offset, p->wrap));
            if (line == 0)
                {
                lastOffset = offset;
                lastParagraph = p;
                return buffer;
                }
        }
        p = p->next;
        offset = 0;
    }
    buffer[0] = 0;
    return buffer;
}

int THelpTopic::getNumCrossRefs()
{
    return numRefs;
}

int THelpTopic::numLines()
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

void THelpTopic::setCrossRef( int i, TCrossRef& ref )
{
    TCrossRef *crossRefPtr;

    if (i < numRefs)
        {
        crossRefPtr = crossRefs + i;
        *crossRefPtr = ref;
        }
}       


void THelpTopic::setNumCrossRefs( int i )
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

        delete [numRefs] crossRefPtr; 
        }
    crossRefs = p;
    numRefs = i;
}


void THelpTopic::setWidth( int aWidth )
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
            crossRefHandler(s, crossRefPtr->ref);
            s << crossRefPtr->offset << crossRefPtr->length;
            }
}

Boolean isBlank( char ch )
{
    if (isspace(ch))
        return True;
    else
        return False;
}

int scan( char *p, int offset, char c)
{
    char *temp1, *temp2;
    
    temp1 = p + offset;
    temp2 = strchr(temp1, c);
    if (temp2 == 0)
       return 256;
    else
       {
       if ((int)(temp2 - temp1) <= 256 )
         return (int) (temp2 - temp1) + 1;
       else
         return 256;
       }
}

void textToLine( void *text, int offset, int length, char *line )
{
    strncpy(line, (char *)text+offset, length);
    line[length] = 0;
}

char *THelpTopic::wrapText( char *text, int size,
          int& offset, Boolean wrap )
{
    char line[256];
    int i;

    i = scan(text, offset, '\n');
    if (i + offset > size )
        i = size - offset;
    if ((i >= width) && (wrap == True))
        {
        i = offset + width;
        if (i > size)
            i = size;
        else
            {
            while((i > offset) && !(isBlank(text[i])))
                --i;
            if (i == offset)
                i = offset + width;
            else
                ++i;
            }
        if (i == offset)
        i = offset + width;
        i -= offset;
        }
    textToLine(text, offset, i, line);
    if (line[strlen(line) - 1] == '\n')
        line[strlen(line) - 1] = 0;
    offset += i;
    return line;
}

// THelpIndex 

const char * const near THelpIndex::name = "THelpIndex";

void THelpIndex::write( opstream& os )
{
    long *indexArrayPtr;

    os << size;
    for (int i = 0; i < size; ++i)
        {
        indexArrayPtr = index + i; 
        os << *indexArrayPtr;
        }
}

void *THelpIndex::read( ipstream& is )
{
    long *indexArrayPtr;

    is >> size;
    if (size == 0)
        index = 0;
    else
        {
        index =  new long[size];
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
    delete [size] index;
}


THelpIndex::THelpIndex(void): TObject ()
{
    size = 0;
    index = 0;
}

long THelpIndex::position(int i)
{
    long *indexArrayPtr;

    if (i < size)
        {
        indexArrayPtr = index + i;
        return (*indexArrayPtr);
        }
    else
        return -1;
}

void THelpIndex::add( int i, long val )
{
    int delta = 10;
    long *p;
    int newSize;
    long *indexArrayPtr;

    if (i >= size)
        {
        newSize = (i + delta) / delta * delta;
        p = new long[newSize];
        if (p != 0)
            {
            memmove(p, index, size * sizeof(long));
            memset(p+size, 0xFF, (newSize - size) * sizeof(long));
            }
        if (size > 0)
            {
            delete [size] index;
            }
        index = p;
        size = newSize;
        }
    indexArrayPtr = index + i;
    *indexArrayPtr = val;
}

// THelpFile

THelpFile::THelpFile( fpstream&  s )
{
    long magic;
    int handle;
    long size;

    magic = 0;
    s.seekg(0);
    handle = s.rdbuf()->fd();
    size = filelength(handle);
    if (size > sizeof(magic))
        s >> magic;
    if (magic != magicHeader)
        {
        indexPos = 12;
        s.seekg(indexPos);
        index =  new THelpIndex;
        modified = True;
        }
    else
        {
        s.seekg(8);
        s >> indexPos;
        s.seekg(indexPos);
        s >> index; 
        modified = False;
        }
    stream = &s;
}

THelpFile::~THelpFile(void)
{
    long magic, size;
    int handle;

    if (modified == True)
        {
        stream->seekp(indexPos);
        *stream << index;
        stream->seekp(0);
        magic = magicHeader;
        handle = stream->rdbuf()->fd();
        size = filelength(handle) - 8;
        *stream << magic;
        *stream << size;
        *stream << indexPos;
        }
    delete stream;
    delete index;
}

THelpTopic *THelpFile::getTopic( int i )
{
    long pos;
    THelpTopic *topic;

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
    char invalidText[] = "\n No help available in this context.";

    topic =  new THelpTopic;
    para =  new TParagraph;
    para->text = newStr(invalidText);
    para->size = strlen(invalidText);
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
    stream->seekp(indexPos);
    *stream << topic;
    indexPos = stream->tellp();
    modified = True;
}

void notAssigned( opstream& , int )
{
}



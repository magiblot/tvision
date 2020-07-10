/*------------------------------------------------------------*/
/* filename -       edits.cpp                                 */
/*                                                            */
/* function(s)                                                */
/*                  TEditor member functions                  */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TEditor
#include <tvision/tv.h>

#include <ctype.h>

char TEditor::bufChar( uint P )
{
    return buffer[bufPtr(P)];
}

uint TEditor::bufPtr( uint P )
{
    return P < curPtr ? P : P + gapLen;
}

void TEditor::formatLine( ushort *DrawBuf,
                          uint P,
                          int Width,
                          ushort Colors
                        )
{
    const struct { uchar color; uint end; } ranges[] =
    {
        // The attributes for normal text are in the lower byte of 'Colors'.
        // The attributes for text selection are in the upper byte.
        { uchar(Colors), selStart },
        { uchar(Colors >> 8), selEnd },
        { uchar(Colors), bufLen }
    };

    uchar Color = uchar(Colors);
    uint X = 0;

    for (int r = 0; r < 3; ++r)
    {
        Color = ranges[r].color;
        while (P < ranges[r].end && X < Width)
        {
            uchar Char = bufChar(P++);
            if (Char == '\r' || Char == '\n')
                goto fill;
            if (Char == '\t') {
                do {
                    DrawBuf[X++] = (Color << 8) | ' ';
                } while (X%8 != 0 && X < Width);
            } else {
                DrawBuf[X++] = (Color << 8) | Char;
            }
        }
    }
fill:
    while (X < Width)
        DrawBuf[X++] = (Color << 8) | ' ';
}

uint TEditor::lineEnd( uint P )
{
    char c;
    for (uint i = P; i < bufLen; ++i)
        if ((c = bufChar(i)) == '\r' || c == '\n')
            return i;
    return bufLen;
}

uint TEditor::lineStart( uint P )
{
    char c;
    for (int i = P - 1; i >= 0; --i)
        if ((c = bufChar(i)) == '\r')
        {
            if ( i + 1 != curPtr && i + 1 != bufLen
                 && bufChar(i + 1) == '\n' )
                return i + 2;
            return i + 1;
        }
        else if (c == '\n')
            return i + 1;
    return 0;
}

uint TEditor::nextChar( uint P )
{
    if (P + 1 < bufLen)
    {
        if (bufChar(P) == '\r' && bufChar(P + 1) == '\n')
            return P + 2;
        return P + 1;
    }
    return bufLen;
}

uint TEditor::prevChar( uint P )
{
    if (P > 1)
    {
        if (bufChar(P - 2) == '\r' && bufChar(P - 1) == '\n')
            return P - 2;
        return P - 1;
    }
    return 0;
}

extern "C" {

int countLines( const char *buf, uint count )
{
    int lines = 0;
    for (uint i = 0; i < count; ++i)
        if (buf[i] == '\r')
        {
            ++lines;
            if (i+1 < count && buf[i+1] == '\n')
                ++i;
        }
        else if (buf[i] == '\n')
            ++lines;
    return lines;
}

uint scan( const char *block, uint size, const char *str )
{
    uint i = 0, len = strlen(str);
    if (len > 0)
        while (i < size)
        {
            uint begin = i, j = 0;
            while (i < size && block[i++] == str[j])
                if (++j == len)
                    return begin;
        }
    return sfSearchFailed;
}

uint iScan( const char *block, uint size, const char *str )
{
    uint i = 0, len = strlen(str);
    if (len > 0)
        while (i < size)
        {
            uint begin = i, j = 0;
            while (i < size && toupper(block[i++]) == toupper(str[j]))
                if (++j == len)
                    return begin;
        }
    return sfSearchFailed;
}

}

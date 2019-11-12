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

#ifndef __BORLANDC__

#include <utility>
#include <assert.h>

char TEditor::bufChar( ushort P )
{
    return buffer[bufPtr(P)];
}

ushort TEditor::bufPtr( ushort P )
{
    return P < curPtr ? P : P + gapLen;
}

#define loByte(w)    (((uchar *)&w)[0])
#define hiByte(w)    (((uchar *)&w)[1])

void TEditor::formatLine( ushort *DrawBuf,
                          ushort P,
                          int Width,
                          ushort Colors
                        )
{
    ushort ColorChar;
    uchar &Char = loByte(ColorChar);
    uchar &Color = hiByte(ColorChar);
    int Limit;
    int X = 0;

    for (const auto& [c, l] : { std::make_pair(loByte(Colors), selStart),
                                std::make_pair(hiByte(Colors), selEnd),
                                std::make_pair(loByte(Colors), bufLen) })
    {
        Color = c; Limit = l;
        while (P < Limit && X < Width)
        {
            Char = bufChar(P++);
            if (Char == '\r')
                goto fill;
            if (Char == '\t') {
                Char = ' ';
                do {
                    DrawBuf[X++] = ColorChar;
                } while (X%8 != 0 && X < Width);
            } else {
                DrawBuf[X++] = ColorChar;
            }
        }
    }
fill:
    Char = ' ';
    while (X < Width)
        DrawBuf[X++] = ColorChar;
}

ushort TEditor::lineEnd( ushort P )
{
    for (ushort i = P; i < bufLen; ++i)
        if (bufChar(i) == '\r')
            return i;
    return bufLen;
}

ushort TEditor::lineStart( ushort P )
{
    for (int i = P - 1; i >= 0; --i)
        if (bufChar(i) == '\r')
        {
            if ( i + 1 != curPtr && i + 1 != bufLen
                 && bufChar(i + 1) == '\n' )
                return i + 2;
            return i + 1;
        }
    return 0;
}

ushort TEditor::nextChar( ushort P )
{
    if (P + 1 < bufLen)
    {
        if (bufChar(P) == '\r' && bufChar(P + 1) == '\n')
            return P + 2;
        return P + 1;
    }
    return bufLen;
}

ushort TEditor::prevChar( ushort P )
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
            ++lines;
    return lines;
}

ushort scan( const char *block, uint size, const char *str )
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

ushort iScan( const char *block, uint size, const char *str )
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

#endif

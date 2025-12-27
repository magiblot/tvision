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
#define Uses_TText
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

static inline TColorAttr getColorAt( uint P, TAttrPair colors, TEditor &editor )
{
    if (editor.selStart <= P && P < editor.selEnd)
        return colors >> 8;
    return colors;
}

void TEditor::formatLine( TDrawBuffer &b,
                          uint linePtr,
                          int hScroll,
                          int width,
                          TAttrPair colors )
{
    hScroll = max(hScroll, 0);
    width = max(width, 0);

    uint P = linePtr;
    int pos = 0;
    int x = 0;
    while (P < bufLen)
    {
        uint nextP = P;
        int nextPos = pos;
        nextCharAndPos(nextP, nextPos);

        // Only break when exceeding 'width', so that combining characters in
        // the last visible column can be drawn properly.
        if (x > width || (x == width && pos < nextPos))
            break;

        char buf[maxCharSize];
        uint charLen = nextP - P;
        getText(P, TSpan<char>(buf, charLen));

        if (buf[0] == '\r' || buf[0] == '\n')
            break;

        // Only draw text beyond position 'hScroll'.
        if (nextPos > hScroll)
        {
            TColorAttr color = getColorAt(P, colors, *this);
            // If pos < hScroll, we are dealing with an incomplete tabulator or
            // double-width character which we have to represent as spaces.
            int charWidth = nextPos - max(pos, hScroll);
            if (buf[0] == '\t' || pos < hScroll)
                b.moveChar(x, ' ', color, charWidth);
            else
                b.moveStr(x, TStringView(buf, charLen), color);

            x += charWidth;
        }

        P = nextP;
        pos = nextPos;
    }

    if (x < width)
    {
        TColorAttr colorAfter = getColorAt(P, colors, *this);
        b.moveChar(x, ' ', colorAfter, width - x);
    }
}

uint TEditor::lineEnd( uint P )
{
    for (uint i = P; i < bufLen; ++i)
    {
        char c = bufChar(i);
        if (c == '\r' || c == '\n')
            return i;
    }
    return bufLen;
}

uint TEditor::lineStart( uint P )
{
    uint i = P;
    while (i--)
    {
        char c = bufChar(i);
        if (c == '\r')
        {
            if ( i + 1 != curPtr && i + 1 != bufLen &&
                 bufChar(i + 1) == '\n' )
                return i + 2;
            return i + 1;
        }
        else if (c == '\n')
            return i + 1;
    }
    return 0;
}

uint TEditor::nextChar( uint P )
{
    if (P + 1 < bufLen)
    {
        if (bufChar(P) == '\r' && bufChar(P + 1) == '\n')
            return P + 2;
        if (encoding == encSingleByte)
            return P + 1;
        else
        {
            char buf[maxCharSize];
            uint count = getText(P, TSpan<char>(buf, maxCharSize));
            return P + TText::next(TStringView(buf, count));
        }
    }
    return bufLen;
}

uint TEditor::prevChar( uint P )
{
    if (P > 1)
    {
        if (bufChar(P - 2) == '\r' && bufChar(P - 1) == '\n')
            return P - 2;
        if (encoding == encSingleByte)
            return P - 1;
        else
        {
            char buf[maxCharSize];
            uint count = min(maxCharSize, P);
            getText(P - count, TSpan<char>(buf, count));
            return P - TText::prev(TStringView(buf, count), count);
        }
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
            uint j = i, k = 0;
            while (j < size && block[j++] == str[k])
                if (++k == len)
                    return i;
            ++i;
        }
    return sfSearchFailed;
}

uint iScan( const char *block, uint size, const char *str )
{
    uint i = 0, len = strlen(str);
    if (len > 0)
        while (i < size)
        {
            uint j = i, k = 0;
            while (j < size && toupper((uchar) block[j++]) == toupper((uchar) str[k]))
                if (++k == len)
                    return i;
            ++i;
        }
    return sfSearchFailed;
}

}

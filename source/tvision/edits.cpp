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

static inline TColorAttr getColorAt( uint P, TEditor &editor, TAttrPair colors )
{
    if (editor.selStart <= P && P < editor.selEnd)
        return colors >> 8;
    return colors;
}

static void formatText( TSpan<TScreenCell> cells, size_t &pos,
                        TEditor &editor, uint &P, TAttrPair colors )
{
    char buf[maxCharSize];
    while (P < editor.bufLen)
    {
        uint count = editor.getText(P, TSpan<char>(buf, maxCharSize));
        if (buf[0] == '\r' || buf[0] == '\n')
            break;
        TColorAttr color = getColorAt(P, editor, colors);
        if (buf[0] == '\t')
        {
            if (pos >= cells.size())
                break;
            do
            {
                ::setCell(cells[pos++], ' ', color);
            } while ((pos % 8 != 0) && pos < cells.size());
            ++P;
        }
        else
        {
            // Let 'drawOne' decide whether to keep copying text, since it will
            // properly handle double-width and combining characters near the
            // end of the output buffer.
            size_t i = 0;
            if (!TText::drawOne(cells, pos, TStringView(buf, count), i, color))
                break;
            P += i;
        }
    }
}

void TEditor::formatLine( TScreenCell *drawBuf,
                          uint P,
                          int aWidth,
                          TAttrPair colors )
{
    size_t width = max(aWidth, 0);
    TSpan<TScreenCell> cells(drawBuf, width);

    size_t pos = 0;
    formatText(cells, pos, *this, P, colors);

    TColorAttr fill = getColorAt(P, *this, colors);
    while (pos < cells.size())
        ::setCell(cells[pos++], ' ', fill);
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

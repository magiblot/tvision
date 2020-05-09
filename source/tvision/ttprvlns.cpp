/*------------------------------------------------------------*/
/* filename -       ttprvlns.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TTerminal prevLines member function       */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TTerminal
#include <tvision/tv.h>

static Boolean backwardsFindLF( const char *buffer, ushort &p, ushort count )
{
    ushort pos = p;
    while (count--)
        if (buffer[pos--] == '\n')
            return p = pos, True;
    return p = pos, False;
}

ushort TTerminal::prevLines( ushort pos, ushort lines )
{
    if (lines != 0)
    {
        if (pos == queBack)
            return queBack;
        bufDec(pos);
        while (lines > 0)
        {
            // TTerminal uses a circular buffer. 'count' here measures how
            // many bytes can be read before reaching the 'queue back'
            // or the beginning of the buffer.
            ushort count = (pos > queBack ? pos - queBack : pos) + 1;
            if (backwardsFindLF(buffer, pos, count))
                --lines;
            else if (ushort(pos + 1) == queBack) // 'queue back' reached.
                return queBack;
            else
                pos = bufSize - 1;
        };
    }
    bufInc(pos);
    bufInc(pos);
    return pos;
}

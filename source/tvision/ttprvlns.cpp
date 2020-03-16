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

ushort TTerminal::prevLines( ushort pos, ushort lines )
{
    if (lines != 0)
    {
        if (pos == queBack)
            return queBack;
        pos = (pos > 0 ? pos : bufSize) - 1;
        while (lines > 0)
        {
            ushort count = (pos > queBack ? pos - queBack : pos) + 1;
            char c = '\0';
            while (count-- && (c = buffer[pos--]) != '\n');
            if (c == '\n')
                --lines;
            else if (ushort(pos + 1) == queBack)
                return queBack;
            else pos = bufSize - 1;
        };
    }
    pos += 2;
    if (pos >= bufSize)
        pos -= bufSize;
    return pos;
}

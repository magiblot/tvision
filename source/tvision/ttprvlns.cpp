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
    ushort ecx;
    if (lines != 0)
    {
        if (pos == queBack)
            return queBack;
        pos = (pos > 0 ? pos : bufSize) - 1;
        while (lines > 0)
        {
            ecx = (pos > queBack ? pos - queBack : pos) + 1;
            while (ecx-- && buffer[pos--] != '\n');
            if (buffer[ushort(pos + 1)] == '\n')
                --lines;
            else if (ushort(pos + 1) == queBack)
                return queBack;
            else pos = bufSize - 1;
        };
    }
    pos = (pos + 2 < bufSize) ? pos + 2 : 0;
    return pos;
}

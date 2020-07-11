/*------------------------------------------------------------*/
/* filename -       misc.cpp                                  */
/*                                                            */
/* function(s)                                                */
/*          message -- sends a message to an object           */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TView
#define Uses_TEvent
#define Uses_TObject
#define Uses_TVMemMgr
#include <tvision/tv.h>
#include <stddef.h>
#include <string.h>

void *message( TView *receiver, ushort what, ushort command, void *infoPtr)
{
    if( receiver == 0 )
        return 0;

    TEvent event;
    event.what = what;
    event.message.command = command;
    event.message.infoPtr = infoPtr;
    receiver->handleEvent( event );
    if( event.what == evNothing )
        return event.message.infoPtr;
    else
        return 0;
}

Boolean lowMemory()
{
    return Boolean(TVMemMgr::safetyPoolExhausted());
}

char *strnzcpy( char *dest, const char *src, size_t n )
{
    // Same as strncpy, but always adds a terminator.
    if (n)
    {
        strncpy( dest, src, n-1 );
        dest[n-1] = '\0';
    }
    return dest;
}

unsigned int fast_utoa ( unsigned int value, char *buffer ) {
    // Copyright(c) 2014-2016 Milo Yip (https://github.com/miloyip/itoa-benchmark)
    unsigned int digits =
        value < 10          ? 1
      : value < 100         ? 2
      : value < 1000        ? 3
      : value < 10000       ? 4
      : value < 100000      ? 5
      : value < 1000000     ? 6
      : value < 10000000    ? 7
      : value < 100000000   ? 8
      : value < 1000000000  ? 9
                            : 10;
    buffer += digits;
    *buffer = '\0';

    do {
        *--buffer = char(value % 10) + '0';
        value /= 10;
    } while (value > 0);

    return digits;
}

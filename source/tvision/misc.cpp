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
    strncpy( dest, src, n-1 );
    dest[n-1] = '\0';
    return dest;
}

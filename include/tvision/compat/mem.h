/*  mem.h

    Memory manipulation functions

    Copyright (c) 1987, 1992 by Borland International
    All Rights Reserved.
*/

#ifdef __BORLANDC__
#include <mem.h>
#else

#ifndef TVISION_COMPAT_MEM_H
#define TVISION_COMPAT_MEM_H

#ifdef _MSC_VER
#include <corecrt.h>
#endif

#include "_defs.h"
#include "_null.h"

#include <cstddef>
#include <cstring>

inline void movmem(const void *__src, void *__dest, unsigned __length)
{
    memmove(__dest, __src, __length);
}

#endif // TVISION_COMPAT_MEM_H

#endif // __BORLANDC__

#ifndef ASSERT_H
#define ASSERT_H

#include <cassert>

#include <cstdio>
#include <unistd.h>

#define BREAK \
{ \
    printf("\rBreakpoint hit at %s, press enter to continue.\n", __func__); \
    char _[80]; \
    /* Take read() from the global namespace to prevent it to be confused with
     * some member function wherever this is included in. */ \
    ::read(0, &_, sizeof(_)); \
}

#else

#include_next <assert.h>

#endif

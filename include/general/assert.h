#ifndef ASSERT_H
#define ASSERT_H

#include <cassert>

#include <unistd.h>

static void PressEnter()
{
    write(1, "Press enter to continue:\n", 25);
    char _[80];
    read(0, &_, sizeof(_));
}

#define BREAK PressEnter();

#else

#include_next <assert.h>

#endif

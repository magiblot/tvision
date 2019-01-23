#ifndef ASSERT_H
#define ASSERT_H

#include <cassert>

#include <signal.h>
#define BREAK raise(SIGSTOP)

#else

#include_next <assert.h>

#endif

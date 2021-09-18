#ifndef SIGWINCH_H
#define SIGWINCH_H

#include <tvision/tv.h>

#ifdef _TV_UNIX

struct TEvent;

class SigwinchAware {

    static int fd[2];
    static void handler(int s) noexcept;
    static void (*oldHandler)(int);

    static bool hit;
    static bool push() noexcept;
    static bool pop() noexcept;

protected:

    SigwinchAware() noexcept;

    static bool winchEvent(TEvent &ev) noexcept;
    static int winchFd() noexcept;

};

#endif // _TV_UNIX

#endif // SIGWINCH_H

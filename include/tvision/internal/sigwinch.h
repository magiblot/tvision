#ifndef SIGWINCH_H
#define SIGWINCH_H

#include <tvision/tv.h>

#ifdef _TV_UNIX

struct TEvent;

class SigwinchAware {

    static int fd[2];
    static void handler(int s);
    static void (*oldHandler)(int);

    static bool hit;
    static bool push();
    static bool pop();

protected:

    SigwinchAware();

    static bool winchEvent(TEvent &ev);
    static int winchFd();

};

#endif // _TV_UNIX

#endif // SIGWINCH_H

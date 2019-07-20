#ifndef SIGWINCH_H
#define SIGWINCH_H

#define Uses_TEvent
#include <tvision/tv.h>

#include <internal/platform.h>

class SigwinchAware {

    static int fd[2];
    static void handler(int s);
    static void (*oldHandler)(int);

protected:

    SigwinchAware();

    bool winchEvent(TEvent &ev);
    int winchFd();

};

#endif

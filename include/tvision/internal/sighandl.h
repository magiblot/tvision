#ifndef SIGHANDL_H
#define SIGHANDL_H

#include <tvision/tv.h>

#ifdef _TV_UNIX

#include <signal.h>

class TSignalHandler {

    static void SigHandler(int s, siginfo_t* si, ucontext_t* context);
    static struct sigaction& oldAction(int s);
    static void printSignalMsg(int s, siginfo_t* si, ucontext_t* context);
    static void clearStdin();

public:

    TSignalHandler();
    ~TSignalHandler();

};

#endif // _TV_UNIX

#endif // SIGHANDL_H

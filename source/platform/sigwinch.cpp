#define Uses_TEvent
#include <tvision/tv.h>

#include <internal/sigwinch.h>
#include <signal.h>
#include <unistd.h>

int SigwinchAware::fd[2];
void (*SigwinchAware::oldHandler) (int);

void SigwinchAware::handler(int s)
{
    write(fd[1], &s, sizeof(int));
    oldHandler(s);
}

SigwinchAware::SigwinchAware()
{
    static bool firstTime = true;
    if (firstTime) {
        firstTime = false;
        pipe(fd);
        struct sigaction sa, oldsa;
        sa.sa_handler = handler;
        sa.sa_flags = SA_RESTART;
        sigfillset(&sa.sa_mask);
        sigaction(SIGWINCH, &sa, &oldsa);
        oldHandler = oldsa.sa_handler;
    }
}

bool SigwinchAware::winchEvent(TEvent &ev)
{
    int s;
    read(fd[0], &s, sizeof(int));
    if (s == SIGWINCH)
    {
        ev.what = evCommand;
        ev.message.command = cmScreenChanged;
        return true;
    }
    return false;
}

int SigwinchAware::winchFd()
{
    return fd[0];
}

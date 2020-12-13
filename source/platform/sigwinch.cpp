#define Uses_TEvent
#define Uses_TPoint
#include <tvision/tv.h>

#ifdef _TV_UNIX

#include <internal/sigwinch.h>
#include <internal/stdioctl.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

int SigwinchAware::fd[2] = {-1, -1};
void (*SigwinchAware::oldHandler) (int);
bool SigwinchAware::hit = false;

bool SigwinchAware::push()
{
    if (!hit)
    {
        hit = true;
        char c = 0;
        int rr = ::write(fd[1], &c, sizeof(char));
        (void) rr;
        return true;
    }
    return false;
}

bool SigwinchAware::pop()
{
    if (hit)
    {
        char c;
        int rr = ::read(fd[0], &c, sizeof(char));
        (void) rr;
        hit = false;
        return true;
    }
    return false;
}

void SigwinchAware::handler(int)
{
    // Write something to the pipe, so that it is seen as ready to read by
    // FdInputStrategy.
    push();
    // Don't call the previous SIGWINCH handler. Unfortunately, Ncurses
    // clears the screen when doing so. Whichever processing was necessary
    // will have to be done manually after winchEvent().
}

SigwinchAware::SigwinchAware()
{
    static bool firstTime = true;
    if (firstTime)
    {
        firstTime = false;
        // Make the pipe non-blocking, so that read() never gets stuck.
        int rr = pipe(fd);
        (void) rr;
        for (int d : fd)
        {
            int flags = fcntl(d, F_GETFL);
            fcntl(d, F_SETFL, flags | O_NONBLOCK);
        }
        // Set the SIGWINCH handler, and save the previous one.
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
    if (pop())
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

#endif // _TV_UNIX

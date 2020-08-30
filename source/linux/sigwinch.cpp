#define Uses_TEvent
#include <tvision/tv.h>

#ifdef _TV_UNIX

#include <internal/sigwinch.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

int SigwinchAware::fd[2];
void (*SigwinchAware::oldHandler) (int);

void SigwinchAware::handler(int s)
{
    // Write something to the pipe, so that it is seen as ready to read by
    // FdInputStrategy.
    if (pipeEmpty())
    {
        char c = 0;
        write(fd[1], &c, sizeof(char));
    }
    // Call the previous SIGWINCH handler, so as not to break
    // the original behaviour (e.g. Ncurses notices resolution change).
    oldHandler(s);
}

bool SigwinchAware::pipeEmpty()
{
    struct pollfd p = { .fd = fd[0], .events = POLLIN };
    poll(&p, 1, 0);
    return p.revents == 0;
}

SigwinchAware::SigwinchAware()
{
    static bool firstTime = true;
    if (firstTime)
    {
        firstTime = false;
        // Make the pipe non-blocking, so that read() never gets stuck.
        pipe2(fd, O_NONBLOCK);
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
    ev.what = evCommand;
    ev.message.command = cmScreenChanged;
    return true;
}

void SigwinchAware::winchClear()
{
    // The underlying class may or may not call winchEvent() as many times
    // as signals were caught. So it has to be responsible for invoking winchClear()
    // when it believes it has no more resize events to process.
    char c;
    read(fd[0], &c, sizeof(char));
}

int SigwinchAware::winchFd()
{
    return fd[0];
}

#endif // _TV_UNIX

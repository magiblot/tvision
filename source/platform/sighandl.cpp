#define Uses_TProgram
#include <tvision/tv.h>

#ifdef _TV_UNIX

#include <internal/sighandl.h>
#include <unistd.h>
#include <fcntl.h>
#include <cctype>
#include <cstdlib>
#include <cstdio>

TSignalHandler::TSignalHandler()
{
    struct sigaction sa = {};
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = (void (*) (int, siginfo_t*, void*)) SigHandler;
    sigaction(SIGSEGV, &sa, &oldAction(SIGSEGV));
    sigaction(SIGILL, &sa, &oldAction(SIGILL));
}

TSignalHandler::~TSignalHandler()
{
    sigaction(SIGSEGV, &oldAction(SIGSEGV), 0);
    sigaction(SIGILL, &oldAction(SIGILL), 0);
}

void TSignalHandler::SigHandler(int s, siginfo_t* si, ucontext_t* context)
{
    // Save and disable the handler, to avoid recursion if something goes wrong.
    struct sigaction sa;
    sigaction(s, &oldAction(s), &sa);
    bool restoreHandler = true;

    TProgram::application->suspend();
    printSignalMsg(s, si, context);

    constexpr char DFLT = 'e', AGAIN = '\0';
    char c;
    do {
        printf("(E)xit, (S)uspend, (D)ie? (default: E) ");
        fflush(stdout);
        clearStdin();
        c = (read(0, &c, 1) > 0) ? tolower(c) : DFLT;
        clearStdin();
        if (c == 'e' || c == '\n')
            exit(1);
        else if (c == 's')
            raise(SIGTSTP); // Suspend process.
        else if (c == 'd')
            restoreHandler = false;
        else
            c = AGAIN;
    } while (c == AGAIN);

    TProgram::application->resume();
    TProgram::application->redraw();

    if (restoreHandler)
        sigaction(s, &sa, 0);
}

struct sigaction& TSignalHandler::oldAction(int s)
{
    static struct sigaction oldsegv, oldill;
    return s == SIGSEGV ? oldsegv : oldill;
}

void TSignalHandler::printSignalMsg(int s, siginfo_t* si, ucontext_t* context)
{
    if (s == SIGSEGV)
        printf("\r\nOops, a segmentation fault (SIGSEGV) was caught!"
               "\r\nDereferenced address: %p", si->si_addr);
    else
        printf("\r\nOops, an illegal instruction (SIGILL) was caught!");
    printf("\r\nWhat would you like to do?"
           "\r\n");
}

void TSignalHandler::clearStdin()
{
    int flags = fcntl(0, F_GETFL), i;
    fcntl(0, F_SETFL, O_NONBLOCK);
    while (read(0, &i, sizeof(i)) > 0);
    fcntl(0, F_SETFL, flags);
}

#endif // _TV_UNIX

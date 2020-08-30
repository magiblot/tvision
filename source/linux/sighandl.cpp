#define Uses_TProgram
#include <tvision/tv.h>

#ifdef _TV_UNIX

#include <internal/sighandl.h>
#include <unistd.h>
#include <fcntl.h>
#include <cctype>

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

#if defined(__x86_64__)
#define REG_IP REG_RIP
#elif defined(__i386__)
#define REG_IP REG_EIP
#else
#undef REG_IP
#endif

void TSignalHandler::SigHandler(int s, siginfo_t* si, ucontext_t* context)
{
    TProgram::application->suspend();
    printSignalMsg(s, si, context);
    const char DFLT = 'c', AGAIN = '\0';
    char c;
    do {
        printf("(C)ontinue, (S)uspend, (D)ie? (default: C) ");
        fflush(stdout);
        clearStdin();
        c = (read(0, &c, 1) > 0) ? tolower(c) : DFLT;
        clearStdin();
        if (c == 'c' || c == '\n')
            context->uc_mcontext.gregs[REG_IP]++; // Increase instruction pointer.
        else if (c == 's')
            raise(SIGTSTP); // Suspend process.
        else if (c == 'd')
            sigaction(s, &oldAction(s), 0); // Restore default handler (abort).
        else
            c = AGAIN;
    } while (c == AGAIN);
    TProgram::application->resume();
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
    printf("\r\nInstruction pointer: %p"
           "\r\nWhat would you like to do?"
           "\r\n", (void*) context->uc_mcontext.gregs[REG_IP]);
}

void TSignalHandler::clearStdin()
{
    int flags = fcntl(0, F_GETFL), i;
    fcntl(0, F_SETFL, O_NONBLOCK);
    while (read(0, &i, sizeof(i)) > 0);
    fcntl(0, F_SETFL, flags);
}

#endif // _TV_UNIX

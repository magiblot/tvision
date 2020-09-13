#include <internal/stdiorec.h>

#ifdef _TV_UNIX

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

StdioRecovery StdioRecovery::bak;

StdioRecovery::StdioRecovery()
{
    for (int i = 0; i < 3; ++i)
        fds[i] = dup(i);
}

StdioRecovery::~StdioRecovery()
{
    for (int i = 0; i < 3; ++i)
        close(fds[i]);
}

void StdioRecovery::recover()
{
    for (int i = 0; i < 3; ++i)
        if (!fdValid(i))
            dup2(bak.fds[i], i);
}

bool StdioRecovery::fdValid(int fd)
{
    return fcntl(fd, F_GETFD) != -1 || errno != EBADF;
}

#endif // _TV_UNIX

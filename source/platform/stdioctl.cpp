#include <internal/stdioctl.h>
#include <internal/getenv.h>

#ifdef _TV_UNIX

#include <unistd.h>
#include <fcntl.h>

StdioCtl StdioCtl::instance;

void StdioCtl::setUp()
{
    int ttyfd = -1;
    if (!getEnv<const char*>("TVISION_USE_STDIO"))
        ttyfd = ::open("/dev/tty", O_RDWR);

    if (ttyfd != -1)
    {
        for (auto &fd : fds)
            fd = ttyfd;
        FILE *ttyfile = ::fdopen(ttyfd, "r+");
        for (auto &file : files)
            file = ttyfile;
    }
    else
    {
        for (int i = 0; i < 3; ++i)
            fds[i] = i;
        files[0] = stdin;
        files[1] = stdout;
        files[2] = stderr;
    }
}

void StdioCtl::tearDown()
{
    // Closing a FILE more than once is UB, so we must be careful.
    for (int i = 0; i < 3; ++i)
    {
        FILE *file = files[i];
        if (file)
        {
            for (int j = i; j < 3; ++j)
                if (files[j] == file)
                    files[j] = nullptr;
            ::fflush(file);
            ::fclose(file);
        }
    }
    // There is no such issue with fds. We can close them even if the above
    // already closed them.
    for (int i = 0; i < 3; ++i)
    {
        ::close(fds[i]);
        fds[i] = -1;
    }
}

#endif // _TV_UNIX

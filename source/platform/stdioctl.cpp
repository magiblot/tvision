#include <internal/stdioctl.h>
#include <internal/getenv.h>

#ifdef _TV_UNIX

#include <unistd.h>
#include <fcntl.h>

StdioCtl StdioCtl::instance;

void StdioCtl::setUp()
{
    ttyfd = -1;
    infile = outfile = nullptr;
    if (!getEnv<const char*>("TVISION_USE_STDIO"))
        ttyfd = ::open("/dev/tty", O_RDWR);

    if (ttyfd != -1)
    {
        for (auto &fd : fds)
            fd = ttyfd;
        infile = ::fdopen(ttyfd, "r");
        files[0] = infile;
        outfile = ::fdopen(ttyfd, "w");
        files[1] = outfile;
        files[2] = outfile;
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
    if (ttyfd != -1)
    {
        ::fflush(infile);
        ::fflush(outfile);
        ::close(ttyfd);
        ::fclose(infile);
        ::fclose(outfile);
        ttyfd = -1;
        infile = outfile = nullptr;
        for (int i = 0; i < 3; ++i)
        {
            fds[i] = -1;
            files[i] = nullptr;
        }
    }
}

#endif // _TV_UNIX

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

#elif defined(_WIN32)

namespace stdioctl
{

    static bool isValid(HANDLE h)
    {
        return h && h != INVALID_HANDLE_VALUE;
    }

    static bool isConsole(HANDLE h)
    {
        DWORD mode;
        return GetConsoleMode(h, &mode);
    }

} // namespace stdioctl

StdioCtl StdioCtl::instance;

void StdioCtl::setUp()
{
    // The console can be accessed in two ways: through GetStdHandle() or through
    // CreateFile(). GetStdHandle() will be unable to return a console handle
    // if standard handles have been redirected.
    //
    // Additionally, we want to spawn a new console when none is visible to the user.
    // This might happen under two circumstances:
    //
    // 1. The console crashed. This is easy to detect because all console operations
    //    fail on the console handles.
    // 2. The console exists somehow but cannot be made visible, not even by doing
    //    GetConsoleWindow() and then ShowWindow(SW_SHOW). This is what happens
    //    under Git Bash without pseudoconsole support. In this case, none of the
    //    standard handles is a console, yet the handles returned by CreateFile()
    //    still work.
    //
    // So, in order to find out if a console needs to be allocated, we
    // check whether at least of the standard handles is a console. If none
    // of them is, we allocate a new console. Yes, this will always spawn a
    // console if all three standard handles are redirected, but this is not
    // a common use case.
    //
    // Then comes the question of whether to access the console through GetStdHandle()
    // or through CreateFile(). CreateFile() has the advantage of not being affected
    // by standard handle redirection. However, I have found that some terminal
    // emulators (i.e. ConEmu) behave unexpectedly when using screen buffers
    // opened with CreateFile(). So we will use the standard handles whenever possible.
    //
    // It is worth mentioning that the handles returned by CreateFile() have to be
    // closed, but the ones returned by GetStdHandle() must not. So we have to remember
    // this information for each console handle.
    //
    // We also need to remember whether we allocated a console or not, so that
    // we can free it when tearing down. If we don't, weird things may happen.

    using namespace stdioctl;
    tearDown();

    static constexpr struct { DWORD std; int index; } channels[] =
    {
        {STD_INPUT_HANDLE, input},
        {STD_OUTPUT_HANDLE, startupOutput},
        {STD_ERROR_HANDLE, startupOutput},
    };
    bool haveConsole = false;
    for (const auto &c : channels)
    {
        HANDLE h = GetStdHandle(c.std);
        if (isConsole(h))
        {
            haveConsole = true;
            if (!isValid(cn[c.index].handle))
                cn[c.index] = {h, false};
        }
    }
    if (!haveConsole)
    {
        FreeConsole();
        AllocConsole();
        ownsConsole = true;
    }
    if (!isValid(cn[input].handle))
    {
        cn[input].handle = CreateFile(
            "CONIN$",
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ,
            nullptr,
            OPEN_EXISTING,
            0,
            0);
        cn[input].owning = true;
    }
    if (!isValid(cn[startupOutput].handle))
    {
        cn[startupOutput].handle = CreateFile(
            "CONOUT$",
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_WRITE,
            nullptr,
            OPEN_EXISTING,
            0,
            0);
        cn[startupOutput].owning = true;
    }
    cn[activeOutput].handle = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        CONSOLE_TEXTMODE_BUFFER,
        nullptr);
    cn[activeOutput].owning = true;
    {
        CONSOLE_SCREEN_BUFFER_INFO sbInfo {};
        GetConsoleScreenBufferInfo(cn[startupOutput].handle, &sbInfo);
        // Force the screen buffer size to match the window size.
        // The Console API guarantees this, but some implementations
        // are not compliant (e.g. Wine).
        sbInfo.dwSize.X = sbInfo.srWindow.Right - sbInfo.srWindow.Left + 1;
        sbInfo.dwSize.Y = sbInfo.srWindow.Bottom - sbInfo.srWindow.Top + 1;
        SetConsoleScreenBufferSize(cn[activeOutput].handle, sbInfo.dwSize);
    }
    SetConsoleActiveScreenBuffer(cn[activeOutput].handle);
}

void StdioCtl::tearDown()
{
    using namespace stdioctl;
    if (isValid(cn[startupOutput].handle))
        SetConsoleActiveScreenBuffer(cn[startupOutput].handle);
    for (auto &c : cn)
        if (isValid(c.handle))
        {
            if (c.owning)
                CloseHandle(c.handle);
            c.handle = INVALID_HANDLE_VALUE;
        }
    if (ownsConsole)
    {
        FreeConsole();
        ownsConsole = false;
    }
}

#endif // _TV_UNIX

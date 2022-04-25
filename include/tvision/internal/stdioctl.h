#ifndef TVISION_STDIOCTL_H
#define TVISION_STDIOCTL_H

#include <tvision/tv.h>
#include <stdio.h>

#ifdef _WIN32
#include <tvision/compat/windows/windows.h>
#endif

namespace tvision
{

class StdioCtl final
{
#ifdef _WIN32
    enum { input = 0, startupOutput = 1, activeOutput = 2 };
    struct
    {
        HANDLE handle {INVALID_HANDLE_VALUE};
        bool owning {false};
    } cn[3];
    bool ownsConsole {false};
#else
    int ttyfd {-1};
    int fds[2] {-1, -1};
    FILE *infile {nullptr};
    FILE *outfile {nullptr};
#endif // _WIN32


public:

    StdioCtl() noexcept;
    ~StdioCtl();

    void write(const char *data, size_t bytes) const noexcept;
    TPoint getSize() const noexcept;
    TPoint getFontSize() const noexcept;

#ifdef _WIN32
    HANDLE in() const noexcept { return cn[input].handle; }
    HANDLE out() const noexcept { return cn[activeOutput].handle; }
#else
    int in() const noexcept { return fds[0]; }
    int out() const noexcept { return fds[1]; }
    FILE *fin() const noexcept { return infile; }
    FILE *fout() const noexcept { return outfile; }
#ifdef __linux__
    bool isLinuxConsole() const noexcept;
#endif
#endif // _WIN32
};

} //namespace tvision

#endif // TVISION_STDIOCTL_H

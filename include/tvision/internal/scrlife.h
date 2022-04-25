#ifndef TVISION_SCRLIFE_H
#define TVISION_SCRLIFE_H

#include <tvision/tv.h>

#ifdef _TV_UNIX

namespace tvision
{

class StderrRedirector
{
    enum { maxStderrSize = 16*1024*1024 };

    static char stderrBuffer alignas(4096) [4096];
    static const char truncationMsg[];

    int ttyFd {-1};
    int fileFd {-1};

public:

    StderrRedirector() noexcept;
    ~StderrRedirector();
};

class ScreenLifetime
{
    StderrRedirector sr;
};

} // namespace tvision

#endif // _TV_UNIX

#endif // TVISION_SCRLIFE_H

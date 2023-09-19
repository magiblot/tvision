#ifndef TVISION_ERREDIR_H
#define TVISION_ERREDIR_H

#include <tvision/tv.h>

#ifdef _TV_UNIX

namespace tvision
{

class StderrRedirector
{
    int ttyFd {-1};
    int bufFd[2] {-1, -1};

public:

    StderrRedirector() noexcept;
    ~StderrRedirector();
};

} // namespace tvision

#endif // _TV_UNIX

#endif // TVISION_ERREDIR_H

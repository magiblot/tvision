#ifndef TVISION_STDIOREC_H
#define TVISION_STDIOREC_H

#include <tvision/tv.h>

#ifdef _TV_UNIX

class StdioRecovery {

    int fds[3];

    StdioRecovery();
    ~StdioRecovery();

    static StdioRecovery bak;
    static bool fdValid(int fd);

public:

    static void recover();

};

#else

class StdioRecovery {

public:

    static void recover() {}

};

#endif // _TV_UNIX

#endif // TVISION_STDIOREC_H

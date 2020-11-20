#ifndef TVISION_STDIOCTL_H
#define TVISION_STDIOCTL_H

#include <tvision/tv.h>
#include <cstdio>

#ifdef _TV_UNIX

class StdioCtl {

    int fds[3];
    FILE *files[3];
    int ttyfd;
    FILE *infile, *outfile;

    StdioCtl() { setUp(); }
    ~StdioCtl() { tearDown(); }

    void setUp();
    void tearDown();

    static StdioCtl instance;

public:

    static int in() { return instance.fds[0]; }
    static int out() { return instance.fds[1]; }
    static int err() { return instance.fds[2]; }
    static FILE *fin() { return instance.files[0]; }
    static FILE *fout() { return instance.files[1]; }
    static FILE *ferr() { return instance.files[2]; }

};

#else

class StdioCtl {

public:

    static int in() { return 0; }
    static int out() { return 1; }
    static int err() { return 2; }
    static FILE *fin() { return stdin; }
    static FILE *fout() { return stdout; }
    static FILE *ferr() { return stderr; }

};

#endif // _TV_UNIX

#endif // TVISION_STDIOCTL_H

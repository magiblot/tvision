#ifndef WINWIDTH_H
#define WINWIDTH_H

#ifdef _WIN32

#include <tvision/tv.h>
#include <tvision/compat/win.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <mutex>
#include <unordered_map>

class WinWidth {

    // Since there is no alternative to wcwidth and the console API allows
    // having buffers out of sight, character widths are measured by printing
    // to a console buffer and taking the cursor position.

    // A separate state is stored for every thread so that width() is both
    // thread-safe and lock-free.

    std::unordered_map<uint32_t, short> results;
    HANDLE cnHandle {INVALID_HANDLE_VALUE};

    WinWidth();
    ~WinWidth();

    int calcWidth(TStringView);
    void setUp();
    void tearDown();
    static bool valid(HANDLE h);

    static std::vector<WinWidth*> states;
    static std::mutex m;
    static thread_local WinWidth state;


public:

    static int mbcwidth(TStringView);
    static void clearState();

};

inline bool WinWidth::valid(HANDLE h)
{
    // INVALID_HANDLE_VALUE because it's what CreateConsoleScreenBuffer returns.
    return h != INVALID_HANDLE_VALUE;
}

inline int WinWidth::mbcwidth(TStringView mbc)
// At most 4 characters will be read from 'mbc', because it is expected to
// contain exactly one UTF-8 sequence. The result won't be what you expect
// if you pass a longer string.
{
    return state.calcWidth(mbc);
}

#endif // _WIN32

#endif // WINWIDTH_H

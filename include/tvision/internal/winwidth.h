#ifndef WINWIDTH_H
#define WINWIDTH_H

#ifdef _WIN32

#include <tvision/borland/win.h>
#include <string_view>
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

    HANDLE cnHandle {0};
    std::unordered_map<uint32_t, short> results;

    ~WinWidth();

    int calcWidth(std::string_view);
    void setUp();
    void tearDown();

    static thread_local WinWidth state;
    static std::vector<WinWidth*> states;
    static std::mutex m;


public:

    static int mbcwidth(std::string_view);
    static void resetState();

};

inline int WinWidth::mbcwidth(std::string_view mbc)
// At most 4 characters will be read from 'mbc', because it is expected to
// contain exactly one UTF-8 sequence. The result won't be what you expect
// if you pass a longer string.
{
    return state.calcWidth(mbc);
}

#endif // _WIN32

#endif // WINWIDTH_H

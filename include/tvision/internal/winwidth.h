#ifndef WINWIDTH_H
#define WINWIDTH_H

#ifdef _WIN32

#include <tvision/tv.h>
#include <tvision/compat/win.h>
#include <atomic>
#include <unordered_map>

class WinWidth
{
    // Since there is no equivalent to wcwidth and the console API allows
    // having buffers out of sight, character widths are measured by printing
    // to a console buffer and taking the cursor position.

    // A separate state is stored for every thread so that mbcwidth() is both
    // thread-safe and lock-free.

    static std::atomic<size_t> lastReset;
    static thread_local WinWidth localInstance;

    std::unordered_map<uint32_t, short> results;
    HANDLE cnHandle {INVALID_HANDLE_VALUE};
    size_t currentReset {lastReset};

    int calcWidth(TStringView) noexcept;
    void setUp() noexcept;
    void tearDown() noexcept;

    ~WinWidth();

public:

    static int mbcwidth(TStringView) noexcept;
    static void reset() noexcept;
};

inline int WinWidth::mbcwidth(TStringView mbc) noexcept
// At most 4 characters will be read from 'mbc', because it is expected to
// contain exactly one UTF-8 sequence. The result won't be what you expect
// if you pass a longer string.
{
    return localInstance.calcWidth(mbc);
}

inline void WinWidth::reset() noexcept
{
    ++lastReset;
}

#endif // _WIN32

#endif // WINWIDTH_H

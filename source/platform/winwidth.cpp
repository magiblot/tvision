#ifdef _WIN32

#include <internal/winwidth.h>

std::atomic<size_t> WinWidth::lastReset {0};
WinWidth thread_local WinWidth::localInstance;

WinWidth::~WinWidth()
{
    tearDown();
}

void WinWidth::setUp() noexcept
{
    if (cnHandle == INVALID_HANDLE_VALUE || currentReset != lastReset)
    {
        tearDown();
        currentReset = lastReset;
        cnHandle = CreateConsoleScreenBuffer(
            GENERIC_READ | GENERIC_WRITE,
            0,
            0,
            CONSOLE_TEXTMODE_BUFFER,
            0);
        CONSOLE_CURSOR_INFO info = {1, FALSE};
        SetConsoleCursorInfo(cnHandle, &info);
    }
}

void WinWidth::tearDown() noexcept
{
    if (cnHandle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(cnHandle);
        cnHandle = INVALID_HANDLE_VALUE;
    }
    results.clear();
}

int WinWidth::calcWidth(TStringView mbc) noexcept
{
    setUp();
    uint32_t key = 0;
    memcpy(&key, mbc.data(), min<size_t>(mbc.size(), sizeof(key)));
    auto it = results.find(key);
    if (it == results.end())
    {
        short res = -1;
        if (cnHandle != INVALID_HANDLE_VALUE)
        {
            SetConsoleCursorPosition(cnHandle, {0, 0});
            // I have seen WriteConsole return error despite the text being
            // printed... so don't check the return value.
            WriteConsole(cnHandle, mbc.data(), mbc.size(), 0, 0);
            CONSOLE_SCREEN_BUFFER_INFO sbInfo;
            // If something is actually wrong, this will fail for sure.
            if (GetConsoleScreenBufferInfo(cnHandle, &sbInfo))
                res = sbInfo.dwCursorPosition.X;
            // Memoize the result.
            results.emplace(key, res);
        }
        return res;
    }
    else
        return it->second;
}

#endif // _WIN32

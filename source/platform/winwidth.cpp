#ifdef _WIN32

#include <internal/winwidth.h>
#include <algorithm>

std::vector<WinWidth*> WinWidth::states;
std::mutex WinWidth::m;
// Order matters! This relies on the above variables being initialized.
WinWidth thread_local WinWidth::state;

WinWidth::WinWidth()
{
    auto &&lock = std::lock_guard<std::mutex>(m);
    states.push_back(this);
}

WinWidth::~WinWidth()
{
    {
        auto &&lock = std::lock_guard<std::mutex>(m);
        // In theory this object should appear only once in the list, but whatever.
        auto it = states.cbegin();
        while (it != states.cend())
            if (*it == this)
                it = states.erase(it);
            else
                ++it;
    }
    tearDown();
}

void WinWidth::clearState()
{
    auto &&lock = std::lock_guard<std::mutex>(m);
    for (auto *state : states)
        state->tearDown();
}

void WinWidth::setUp()
{
    if (valid(cnHandle))
        tearDown();
    cnHandle = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE,
        0,
        0,
        CONSOLE_TEXTMODE_BUFFER,
        0);
    CONSOLE_CURSOR_INFO info = {1, FALSE};
    SetConsoleCursorInfo(cnHandle, &info);
}

void WinWidth::tearDown()
{
    if (cnHandle != INVALID_HANDLE_VALUE)
        CloseHandle(cnHandle);
    cnHandle = INVALID_HANDLE_VALUE;
    results.clear();
}

int WinWidth::calcWidth(TStringView mbc)
{
    if (!valid(cnHandle))
        setUp();
    uint32_t key = 0;
    memcpy(&key, mbc.data(), std::min(mbc.size(), sizeof(key)));
    auto it = results.find(key);
    if (it == results.end())
    {
        short res = -1;
        if (valid(cnHandle))
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

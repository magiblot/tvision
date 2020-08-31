#ifdef _WIN32

#include <internal/winwidth.h>
#include <algorithm>

WinWidth thread_local WinWidth::state;

std::vector<WinWidth*> WinWidth::states;
std::mutex WinWidth::m;

WinWidth::~WinWidth()
{
    auto &&lock = std::scoped_lock(m);
    // In theory this object should appear only once in the list, but whatever.
    auto it = states.cbegin();
    while (it != states.cend())
        if (*it == this)
            it = states.erase(it);
        else
            ++it;
    tearDown();
}

void WinWidth::resetState()
{
    auto &&lock = std::scoped_lock(m);
    for (auto *state : states)
        state->setUp();
}

void WinWidth::setUp()
{
    if (!cnHandle) {
        // First time.
        auto &&lock = std::scoped_lock(m);
        states.push_back(this);
    } else {
        tearDown();
    }
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
    CloseHandle(cnHandle);
    cnHandle = 0;
    results.clear();
}

int WinWidth::calcWidth(std::string_view mbc)
{
    if (!cnHandle)
        setUp();
    uint32_t key = 0;
    memcpy(&key, mbc.data(), std::min(mbc.size(), sizeof(key)));
    auto it = results.find(key);
    if (it == results.end()) {
        short res = -1;
        if (cnHandle) {
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
    } else
        return it->second;
}

#endif // _WIN32

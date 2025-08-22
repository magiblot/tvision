#ifdef _WIN32

#include <internal/winwidth.h>
#include <internal/utf8.h>

namespace tvision
{

std::atomic<size_t> WinWidth::lastReset {0};
std::atomic<bool> WinWidth::isLegacyConsole {false};
WinWidth thread_local &WinWidth::localInstance = *new WinWidth;

// MinGW: work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=83562
// by deleting 'localInstance' through a static reference.
struct WinWidth::Destructor
{
    ~Destructor()
    {
        delete &localInstance;
    }
} static thread_local destructor;

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
        // Allocating a buffer in order to print characters is only necessary
        // when not in the legacy console.
        if (!isLegacyConsole)
        {
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

int WinWidth::calcWidth(uint32_t u32) noexcept
{
    static_assert(sizeof(uint16_t) == sizeof(wchar_t), "");

    setUp();

    auto it = results.find(u32);
    if (it == results.end())
    {
        uint16_t u16[3];
        int len = utf32To16(u32, u16);
        if (cnHandle == INVALID_HANDLE_VALUE)
            // In the legacy console, each code unit takes one cell.
            return len;
        int res = -1;
        if (len > 0)
        {
            // We print an additional character so that we can distinguish
            // actual double-width characters from the ones affected by
            // https://github.com/microsoft/terminal/issues/11756.
            u16[len] = '#';
            SetConsoleCursorPosition(cnHandle, {0, 0});
            WriteConsoleW(cnHandle, (wchar_t *) u16, len + 1, 0, 0);
            CONSOLE_SCREEN_BUFFER_INFO sbInfo;
            if ( GetConsoleScreenBufferInfo(cnHandle, &sbInfo) &&
                    (res = sbInfo.dwCursorPosition.X - 1) > 1 )
            {
                COORD coord {1, sbInfo.dwCursorPosition.Y};
                DWORD count = 0; wchar_t charAfter;
                ReadConsoleOutputCharacterW(cnHandle, &charAfter, 1, coord, &count);
                if (count == 1 && charAfter == '#')
                    res = -1;
            }
        }
        // Memoize the result.
        results.emplace(u32, res);
        return res;
    }
    else
        return it->second;
}

} // namespace tvision

#endif // _WIN32

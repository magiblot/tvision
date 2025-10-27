#include <internal/platform.h>
#include <internal/win32con.h>
#include <internal/winwidth.h>
#include <internal/codepage.h>
#include <internal/conctl.h>

#include <locale.h>
#include <wctype.h>
#include <wchar.h>

namespace tvision
{

// These functions are used directly by TText. They are stored in an independent
// global object so that they can be used regardless of whether the global
// Platform instance has been initialized.
// In addition, they are implemented in a separate translation unit so that
// an application can link against it without linking against the entire
// Platform class and its dependencies (e.g., a command-line utility that
// uses TText).

Platform::CharOps Platform::charOps =
{
    &initCharWidth,
    &initCharToLower,
};

void Platform::initLocale() noexcept
{
    static int init = [] ()
    {
        // We rely on the C runtime character conversion methods, which depend
        // on the C locale.
#ifdef _WIN32
        // On Windows, we set the ".utf8" locale explicitly, which also enables
        // UTF-8 support for filesystem-related functions. This is also done
        // in Win32ConsoleAdapter.
        // See: https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale#utf-8-support
        setlocale(LC_ALL, ".utf8");
#else
        setlocale(LC_ALL, "");
#endif // _WIN32
        CpTranslator::init();

        (void) init;
        return 0;
    }();
}

#ifdef _WIN32
static int windowsCharWidth(uint32_t wc) noexcept
{
    return WinWidth::width(wc);
}
#else
static int unixCharWidth(uint32_t wc) noexcept
{
    return wcwidth(wc);
}
#   ifdef __linux__
static int linuxConsoleCharWidth(uint32_t wc) noexcept
{
    // The Linux Console does not support zero-width characters. It assumes
    // all characters are either single or double-width. Additionally, the
    // double-width characters are the same as in the wcwidth() implementation by
    // Markus Kuhn from 2007-05-26 (https://www.cl.cam.ac.uk/~mgk25/ucs/wcwidth.c).
    return 1 +
        (wc >= 0x1100 &&
         (wc <= 0x115f ||
          wc == 0x2329 || wc == 0x232a ||
          (wc >= 0x2e80 && wc <= 0xa4cf &&
           wc != 0x303f) ||
          (wc >= 0xac00 && wc <= 0xd7a3) ||
          (wc >= 0xf900 && wc <= 0xfaff) ||
          (wc >= 0xfe10 && wc <= 0xfe19) ||
          (wc >= 0xfe30 && wc <= 0xfe6f) ||
          (wc >= 0xff00 && wc <= 0xff60) ||
          (wc >= 0xffe0 && wc <= 0xffe6) ||
          (wc >= 0x20000 && wc <= 0x2fffd) ||
          (wc >= 0x30000 && wc <= 0x3fffd)));
}
#   endif // __linux__
#endif // _WIN32

static uint32_t charToLower(uint32_t ch) noexcept
{
    // Skip the conversion if the character does not fit in a single wchar_t.
    if (sizeof(uint32_t) > sizeof(wchar_t) && ch > WCHAR_MAX)
        return ch;
    return (uint32_t) towlower((wchar_t) ch);
}

void Platform::initCharOps() noexcept
{
    static int init = [] ()
    {
        initLocale();

        CharOps charOps {};
#ifdef _WIN32
        charOps.width = windowsCharWidth;
#else
        charOps.width = unixCharWidth;
#   ifdef __linux__
        auto &con = ConsoleCtl::getInstance();
        if (con.isLinuxConsole())
            charOps.width = linuxConsoleCharWidth;
#   endif // __linux__
#endif // _WIN32

        charOps.toLower = charToLower;

        Platform::charOps = charOps;

        (void) init;
        return 0;
    }();
}

int Platform::initCharWidth(uint32_t wc) noexcept
{
    initCharOps();
    return charOps.width(wc);
}

uint32_t Platform::initCharToLower(uint32_t wc) noexcept
{
    initCharOps();
    return charOps.toLower(wc);
}

} // namespace tvision

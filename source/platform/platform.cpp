#include <internal/platform.h>
#include <internal/unixcon.h>
#include <internal/linuxcon.h>
#include <internal/win32con.h>
#include <internal/stdioctl.h>
#include <internal/ncurdisp.h>
#include <internal/ansidisp.h>
#include <internal/ncursinp.h>
#include <internal/getenv.h>
#include <locale.h>
#include <stdio.h>

// This is used by TText. It is a global function pointer (instead of a
// Platform instance method) so that it can be still used after
// Platform::instance has been destroyed.
int (*Platform::charWidth)(TStringView, char32_t) noexcept = &Platform::errorCharWidth;

int Platform::errorCharWidth(TStringView, char32_t) noexcept
{
    fputs( "Cannot measure character widths before the platform module is "
           "loaded.\nAvoid invoking TText methods during static initialization.\n",
           stderr );
    abort();
}

Platform Platform::instance;

Platform::Platform() noexcept
{
#ifdef _WIN32
    setlocale(LC_ALL, ".utf8");
    charWidth = &Win32ConsoleStrategy::charWidth;
#else
    setlocale(LC_ALL, "");
    charWidth =
#ifdef __linux__
        io.isLinuxConsole() ? &LinuxConsoleStrategy::charWidth :
#endif // __linux__
        &UnixConsoleStrategy::charWidth;
#endif // _WIN32
}

Platform::~Platform()
{
    restoreConsole();
}

ConsoleStrategy &Platform::createConsole() noexcept
{
#ifdef _WIN32
    return Win32ConsoleStrategy::create(io);
#else
    NcursesDisplay *display;
    if (getEnv<TStringView>("TVISION_DISPLAY") == "ncurses")
        display = new NcursesDisplay(io);
    else
        display = new AnsiDisplay<NcursesDisplay>(io);
#ifdef __linux__
    if (io.isLinuxConsole())
        return LinuxConsoleStrategy::create(io, *display, *new NcursesInput(io, *display, false));
#endif // __linux__
    return *new UnixConsoleStrategy(*display, *new NcursesInput(io, *display, true));
#endif // _WIN32
}

void Platform::setUpConsole() noexcept
{
    auto &doAdd = *[] (void *self, EventSource &source) {
        ((Platform *) self)->waiter.addSource(source);
    };
    if (console == &dummyConsole)
    {
        console = &createConsole();
        console->forEachSource(this, doAdd);
        reloadScreenInfo();
    }
}

void Platform::restoreConsole() noexcept
{
    auto &doRemove = *[] (void *self, EventSource &source) {
        ((Platform *) self)->waiter.removeSource(source);
    };
    if (console != &dummyConsole)
    {
        flushScreen();
        auto *c = console;
        console = &dummyConsole;
        c->forEachSource(this, doRemove);
        delete c;
    }
}

void Platform::checkConsole() noexcept
{
    if (!console->isAlive())
    {
        // The console likely crashed (Windows).
        restoreConsole();
        setUpConsole();
    }
}





#include <internal/platform.h>
#include <internal/unixcon.h>
#include <internal/linuxcon.h>
#include <internal/win32con.h>
#include <internal/ncurdisp.h>
#include <internal/ansidisp.h>
#include <internal/ncursinp.h>
#include <internal/getenv.h>

// These methods are defined in a separate transaction unit so that the
// Platform can be referenced by the application without having to link all the
// console strategies.

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

void Platform::checkConsole() noexcept
{
    if (!console->isAlive())
    {
        // The console likely crashed (Windows).
        restoreConsole();
        setUpConsole();
    }
}

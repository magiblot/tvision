#include <internal/platform.h>
#include <internal/unixcon.h>
#include <internal/linuxcon.h>
#include <internal/win32con.h>
#include <internal/ncurdisp.h>
#include <internal/ansidisp.h>
#include <internal/ncursinp.h>
#include <internal/sighandl.h>
#include <internal/scrlife.h>
#include <internal/getenv.h>

namespace tvision
{

// These methods are defined in a separate transaction unit so that the
// Platform can be referenced by the application without having to link all the
// console strategies.

ConsoleStrategy &Platform::createConsole() noexcept
{
#ifdef _WIN32
    return Win32ConsoleStrategy::create();
#else
    ScreenLifetime &scrl = *new ScreenLifetime;
    NcursesDisplay *display;
    if (getEnv<TStringView>("TVISION_DISPLAY") == "ncurses")
        display = new NcursesDisplay(io);
    else
        display = new AnsiDisplay<NcursesDisplay>(io);
#ifdef __linux__
    if (io.isLinuxConsole())
        return LinuxConsoleStrategy::create(io, scrl, *display, *new NcursesInput(io, *display, false));
#endif // __linux__
    return *new UnixConsoleStrategy(scrl, *display, *new NcursesInput(io, *display, true));
#endif // _WIN32
}

void Platform::setUpConsole() noexcept
{
    auto doAdd = [] (void *self, EventSource &source) {
        ((Platform *) self)->waiter.addSource(source);
    };
    console.lock([&] (ConsoleStrategy *&c) {
        if (c == &dummyConsole)
        {
            c = &createConsole();
            SignalHandler::enable(signalCallback);
            c->forEachSource(this, *(void (*)(void *, EventSource &)) doAdd);
        }
    });
}

void Platform::checkConsole() noexcept
{
    console.lock([&] (ConsoleStrategy *c) {
        if (!c->isAlive())
        {
            // The console likely crashed (Windows).
            restoreConsole();
            setUpConsole();
        }
    });
}

bool Platform::getEvent(TEvent &ev) noexcept
{
    if ( waiter.getEvent(ev)
         && (ev.what != evCommand || ev.message.command != cmScreenChanged) )
        return true;
    if (screenChanged())
    {
        ev.what = evCommand;
        ev.message.command = cmScreenChanged;
        return true;
    }
    return false;
}

void Platform::signalCallback(bool enter) noexcept
{
    if (!instance.console.lockedByThisThread())
    {
        // FIXME: these are not signal safe!
        if (enter)
            instance.restoreConsole();
        else
            instance.setUpConsole();
    }
}

} // namespace tvision

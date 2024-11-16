#define Uses_TScreen
#include <tvision/tv.h>

#include <internal/platform.h>
#include <internal/unixcon.h>
#include <internal/linuxcon.h>
#include <internal/win32con.h>
#include <internal/ncurdisp.h>
#include <internal/ansiwrit.h>
#include <internal/ncursinp.h>
#include <internal/sighandl.h>
#include <internal/conctl.h>
#include <internal/termio.h>

namespace tvision
{

// These methods are defined in a separate transaction unit so that the
// Platform can be referenced by the application without having to link all the
// console adapters.

ConsoleAdapter &Platform::createConsole() noexcept
{
#ifdef _WIN32
    return Win32ConsoleAdapter::create();
#else
    auto &con = ConsoleCtl::getInstance();
    InputState &inputState = *new InputState;
    NcursesDisplay &display = NcursesDisplay::create(con);
#ifdef __linux__
    if (con.isLinuxConsole())
        return LinuxConsoleAdapter::create(con, displayBuf, inputState, display, *new NcursesInput(con, display, inputState, false));
#endif // __linux__
    return UnixConsoleAdapter::create(con, displayBuf, inputState, display, *new NcursesInput(con, display, inputState, true));
#endif // _WIN32
}

void Platform::setUpConsole(ConsoleAdapter *&c) noexcept
{
    if (c == &dummyConsole)
    {
        c = &createConsole();
        SignalHandler::enable(signalCallback);
        for (auto *source : c->sources)
            if (source)
                waiter.addSource(*source);
    }
}

void Platform::checkConsole() noexcept
{
    console.lock([&] (ConsoleAdapter *&c) {
        if (!c->isAlive())
        {
            // The console likely crashed (Windows).
            restoreConsole(c);
            setUpConsole(c);
        }
    });
}

void Platform::waitForEvents(int ms) noexcept
{
    checkConsole();

    int waitTimeoutMs = ms;
    // When the DisplayBuffer has pending changes, ensure we wake up so that
    // they can be flushed in time.
    int flushTimeoutMs = displayBuf.timeUntilPendingFlushMs();
    if (ms < 0)
        waitTimeoutMs = flushTimeoutMs;
    else if (flushTimeoutMs >= 0)
        waitTimeoutMs = min(ms, flushTimeoutMs);

    waiter.waitForEvents(waitTimeoutMs);
}

ushort Platform::getScreenMode() noexcept
{
    return console.lock([] (ConsoleAdapter *c) {
        ushort mode;

        int colorCount = c->display.getColorCount();
        if (colorCount == 0)
            mode = TDisplay::smMono;
        else
            mode = TDisplay::smCO80;

        if (colorCount >= 256)
            mode |= TDisplay::smColor256;
        if (colorCount >= 256*256*256)
            mode |= TDisplay::smColorHigh;

        TPoint fontSize = c->display.getFontSize();
        if (fontSize.x > 0 && fontSize.y > 0 && fontSize.x >= fontSize.y)
            mode |= TDisplay::smFont8x8;

        return mode;
    });
}

void Platform::signalCallback(bool enter) noexcept
{
    if (instance && !instance->console.lockedByCurrentThread())
    {
        // FIXME: these are not signal safe!
        if (enter)
            instance->restoreConsole();
        else
            instance->setUpConsole();
    }
}

} // namespace tvision

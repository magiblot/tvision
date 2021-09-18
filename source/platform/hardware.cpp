#define Uses_TKeys
#define Uses_TEvent
#define Uses_TScreen
#define Uses_THardwareInfo
#include <tvision/tv.h>

#include <internal/platform.h>
#include <internal/win32con.h>
#include <internal/ncurdisp.h>
#include <internal/ncursinp.h>
#include <internal/ansidisp.h>
#include <internal/linuxcon.h>
#include <internal/sighandl.h>
#include <internal/terminal.h>
#include <internal/getenv.h>
#include <iostream.h>
#include <chrono>
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::steady_clock;

TEvent THardwareInfo::eventQ[];
size_t THardwareInfo::eventCount = 0;

static bool alwaysFlush;

THardwareInfo::THardwareInfo() noexcept
{
    pendingEvent = 0;
    alwaysFlush = getEnv<int>("TVISION_MAX_FPS", 0) < 0;
#ifdef _TV_UNIX
    static TSignalHandler h;
#endif
}

// For brevity.
static constexpr PlatformStrategy* &platf = PlatformStrategy::instance;
static constexpr NullPlatform* nullPlatf = &NullPlatform::instance;

void THardwareInfo::setCaretSize( ushort size ) noexcept { platf->setCaretSize(size); }
void THardwareInfo::setCaretPosition( ushort x, ushort y ) noexcept { platf->setCaretPosition(x, y); }
ushort THardwareInfo::getCaretSize() noexcept { return platf->getCaretSize(); }
BOOL THardwareInfo::isCaretVisible() noexcept { return platf->isCaretVisible(); }
ushort THardwareInfo::getScreenRows() noexcept { return platf->getScreenRows(); }
ushort THardwareInfo::getScreenCols() noexcept { return platf->getScreenCols(); }
ushort THardwareInfo::getScreenMode() noexcept { return platf->getScreenMode(); }
void THardwareInfo::setScreenMode( ushort mode ) noexcept {}
void THardwareInfo::clearScreen( ushort w, ushort h ) noexcept { platf->clearScreen(); }
void THardwareInfo::screenWrite( ushort x, ushort y, TScreenCell *buf, DWORD len ) noexcept
{
    platf->screenWrite(x, y, buf, len);
    if (alwaysFlush)
        flushScreen();
}
void THardwareInfo::reloadScreenInfo() noexcept { platf->reloadScreenInfo(); }
DWORD THardwareInfo::getButtonCount() noexcept { return platf->getButtonCount(); }
void THardwareInfo::cursorOn() noexcept { platf->cursorOn(); }
void THardwareInfo::cursorOff() noexcept { platf->cursorOff(); }
void THardwareInfo::flushScreen() noexcept { platf->flushScreen(); }

void THardwareInfo::setUpConsole() noexcept
{
    // Set up input/output control.
    // At least with the ncurses implementation, display must be initialized
    // before input.
    if (platf == nullPlatf)
    {
#ifdef _WIN32
        if (!(platf = Win32ConsoleStrategy::create()))
        {
            cerr << "Error: cannot get a console." << endl;
            ExitProcess(1);
        }
#else
        DisplayStrategy *disp;
        if (getEnv<TStringView>("TVISION_DISPLAY") == "ncurses")
            disp = new NcursesDisplay();
        else
            disp = new AnsiDisplay<NcursesDisplay>();
        if (TermIO::isLinuxConsole())
            platf = new LinuxConsoleStrategy( disp,
                                              new NcursesInput(false) );
        else
            platf = new UnixPlatformStrategy( disp,
                                              new NcursesInput() );
#endif
    }
}

void THardwareInfo::restoreConsole() noexcept
{
    // Tear down input/output control by deleting the platform strategy.
    if (platf != nullPlatf)
    {
        auto *p = platf;
        platf = nullPlatf;
        delete p;
    }
}

BOOL THardwareInfo::getPendingEvent(TEvent &event, ushort mask) noexcept
{
    for (size_t i = 0; i < eventCount; ++i)
        if (eventQ[i].what & mask)
        {
            event = eventQ[i];
            for (; i + 1 < eventCount; ++i)
                eventQ[i] = eventQ[i + 1];
            --eventCount;
            return True;
        }
    return False;
}

BOOL THardwareInfo::getMouseEvent( MouseEventType& event ) noexcept
{
    TEvent ev;
    if (getPendingEvent(ev, evMouse))
    {
        event = ev.mouse;
        return True;
    }
    return False;
}

BOOL THardwareInfo::getKeyEvent( TEvent& event, Boolean blocking ) noexcept
{
    readEvents(blocking);
    if (getPendingEvent(event, ~evMouse))
    {
        if (event.what & evKeyboard)
        {
            // Set/Reset insert flag.
            if( event.keyDown.keyCode == kbIns )
                insertState = !insertState;
            if( insertState )
                event.keyDown.controlKeyState |= kbInsState;
            return True;
        }
        return event.what != evNothing;
    }
    return False;
}

void THardwareInfo::readEvents(Boolean blocking) noexcept
{
    // Do not read any more events until the queue is empty.
    if (!eventCount)
    {
        // Flush the screen once for every time all events have been processed,
        // only for blocking requests.
        if (blocking)
            THardwareInfo::flushScreen();
        TEvent event;
        // Non-blocking read.
        while ( eventCount < eventQSize &&
                platf->waitForEvent(0, event) )
            eventQ[eventCount++] = event;
        // Blocking read.
        if (blocking && !eventCount && platf->waitForEvent(eventTimeoutMs, event))
            eventQ[eventCount++] = event;
    }
}

#ifndef _WIN32

extern "C" DWORD GetTickCount(void) noexcept
{
    // This effectively gives a system time reference in milliseconds.
    // steady_clock is best suited for measuring intervals.
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

#endif

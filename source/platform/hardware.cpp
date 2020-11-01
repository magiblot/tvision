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
#include <internal/stdioctl.h>
#include <internal/getenv.h>
#include <string_view>
#include <chrono>
#ifdef _TV_UNIX
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstdio>
#endif
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::steady_clock;

TEvent THardwareInfo::eventQ[];
size_t THardwareInfo::eventCount = 0;

static bool alwaysFlush;

THardwareInfo::THardwareInfo()
{
    pendingEvent = 0;
    alwaysFlush = getEnv<int>("TVISION_MAX_FPS", 0) < 0;
#ifdef _TV_UNIX
    static TSignalHandler h;
#endif
}

bool THardwareInfo::isLinuxConsole()
{
#ifdef __linux__
    /* This is the same function used to get the Shift/Ctrl/Alt modifiers
     * on the console. It fails if stdin is not a console file descriptor. */
    for (int fd : {StdioCtl::in(), StdioCtl::out()})
    {
        char subcode[] = {6, 0}; // Null-terminate so that valgrind doesn't complain.
        if (ioctl(fd, TIOCLINUX, subcode) != -1)
            return true;
    }
    return false;
#else
    return false;
#endif
}

bool THardwareInfo::isWin32Console()
{
#ifdef _WIN32
    DWORD consoleMode;
    return GetConsoleMode( GetStdHandle(STD_INPUT_HANDLE), &consoleMode ) != 0;
#else
    return false;
#endif
}

void THardwareInfo::consoleWrite(const void *data, size_t bytes)
{
#ifdef _WIN32
    Win32ConsoleStrategy::write(data, bytes);
#else
    fflush(StdioCtl::fout());
    ::write(StdioCtl::out(), data, bytes);
#endif
}

/* We don't include these in hardware.h as done originally to prevent it to
 * depend on platform.h. Otherwise, any change in platform.h would affect
 * hardware.h, causing the whole tvision library to recompile. */

static NullPlatform nullPlatf;
static PlatformStrategy *platf = &nullPlatf;

void THardwareInfo::setCaretSize( ushort size ) { platf->setCaretSize(size); }
void THardwareInfo::setCaretPosition( ushort x, ushort y ) { platf->setCaretPosition(x, y); }
ushort THardwareInfo::getCaretSize() { return platf->getCaretSize(); }
BOOL THardwareInfo::isCaretVisible() { return platf->isCaretVisible(); }
ushort THardwareInfo::getScreenRows() { return platf->getScreenRows(); }
ushort THardwareInfo::getScreenCols() { return platf->getScreenCols(); }
ushort THardwareInfo::getScreenMode() { return platf->getScreenMode(); }
void THardwareInfo::setScreenMode( ushort mode ) {}
void THardwareInfo::clearScreen( ushort w, ushort h ) { platf->clearScreen(); }
void THardwareInfo::screenWrite( ushort x, ushort y, TScreenCell *buf, DWORD len )
{
    platf->screenWrite(x, y, buf, len);
    if (alwaysFlush)
        flushScreen();
}
void THardwareInfo::resizeScreenBuffer( TScreenCell *&buffer )
{
    freeScreenBuffer(buffer);
    buffer = allocateScreenBuffer();
    platf->onScreenResize();
}
DWORD THardwareInfo::getButtonCount() { return platf->getButtonCount(); }
void THardwareInfo::cursorOn() { platf->cursorOn(); }
void THardwareInfo::cursorOff() { platf->cursorOff(); }
void THardwareInfo::flushScreen() { platf->flushScreen(); }

void THardwareInfo::setUpConsole()
{
    // Set up input/output control.
    // At least with the ncurses implementation, display must be initialized
    // before input.
    if (platf == &nullPlatf)
    {
#ifdef _WIN32
        if (isWin32Console())
            platf = new Win32ConsoleStrategy();
        else
        {
            cerr << "Error: standard input is being redirected or is not a "
                    "Win32 console." << endl;
            ExitProcess(1);
        }
#else
        DisplayStrategy *disp;
        if (getEnv<std::string_view>("TVISION_DISPLAY") == "ncurses")
            disp = new NcursesDisplay();
        else
            disp = new AnsiDisplay<NcursesDisplay>();
        if (isLinuxConsole())
            platf = new LinuxConsoleStrategy(disp, new NcursesInput(false));
        else
            platf = new UnixPlatformStrategy(disp, new NcursesInput());
#endif
    }
}

void THardwareInfo::restoreConsole()
{
    // Tear down input/output control by deleting the platform strategy.
    if (platf != &nullPlatf)
    {
        delete platf;
        platf = &nullPlatf;
    }
}

BOOL THardwareInfo::getPendingEvent(TEvent &event, ushort mask)
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

BOOL THardwareInfo::getMouseEvent( MouseEventType& event )
{
    TEvent ev;
    if (getPendingEvent(ev, evMouse))
    {
        event = ev.mouse;
        return True;
    }
    return False;
}

BOOL THardwareInfo::getKeyEvent( TEvent& event, Boolean blocking )
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

void THardwareInfo::readEvents(Boolean blocking)
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

extern "C" DWORD GetTickCount(void)
{
    // This effectively gives a system time reference in milliseconds.
    // steady_clock is best suited for measuring intervals.
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

#endif

#define Uses_TKeys
#define Uses_TEvent
#define Uses_THardwareInfo
#include <tvision/tv.h>

#include <internal/platform.h>
#include <internal/ncurdisp.h>
#include <internal/ncursinp.h>
#include <internal/ansidisp.h>
#include <internal/linuxcon.h>
#include <internal/sighandl.h>
#include <internal/getenv.h>
#include <string>
#include <sys/ioctl.h>
#include <chrono>
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::steady_clock;

TEvent THardwareInfo::pendingMouseEvent;

THardwareInfo::THardwareInfo()
{
    pendingEvent = 0;
    static TSignalHandler h;
}

bool THardwareInfo::isLinuxConsole()
{
    /* This is the same function used to get the Shift/Ctrl/Alt modifiers
     * on the console. It fails if stdin is not a console file descriptor. */
    char subcode = 6;
    return ioctl(0, TIOCLINUX, &subcode) != -1;
}

/* We don't include these in hardware.h as done originally to prevent it to
 * depend on platform.h. Otherwise, any change in platform.h would affect
 * hardware.h, causing the whole tvision library to recompile. */

static std::unique_ptr<PlatformStrategy> platf;

void THardwareInfo::setCaretSize( ushort size ) { platf->setCaretSize(size); }
void THardwareInfo::setCaretPosition( ushort x, ushort y ) { platf->setCaretPosition(x, y); }
ushort THardwareInfo::getCaretSize() { return platf->getCaretSize(); }
BOOL THardwareInfo::isCaretVisible() { return platf->isCaretVisible(); }
ushort THardwareInfo::getScreenRows() { return platf->getScreenRows(); }
ushort THardwareInfo::getScreenCols() { return platf->getScreenCols(); }
ushort THardwareInfo::getScreenMode()
{
    ushort mode = platf->getScreenMode();
    if( getScreenRows() > 25 )
        mode |= TDisplay::smFont8x8;
    return mode;
}
void THardwareInfo::setScreenMode( ushort mode ) {}
void THardwareInfo::clearScreen( ushort w, ushort h ) { platf->clearScreen(); }
void THardwareInfo::screenWrite( ushort x, ushort y, ushort *buf, DWORD len )
{
    platf->screenWrite(x, y, buf, len);
}
void THardwareInfo::resizeScreenBuffer( ushort *&buffer )
{
    freeScreenBuffer(buffer);
    buffer = allocateScreenBuffer();
    platf->onScreenResize();
}
DWORD THardwareInfo::getButtonCount() { return platf->getButtonCount(); }
void THardwareInfo::flushScreen() { platf->flushScreen(); }

void THardwareInfo::setUpConsole()
{
    // Set up input/output control.
    // At least with the ncurses implementation, display must be initialized
    // before input.
    if (!platf)
    {
        DisplayStrategy *disp;
        if (getEnv<std::string>("TVISION_DISPLAY") == "ansi")
            disp = new AnsiDisplay<NcursesDisplay>();
        else
            disp = new NcursesDisplay();
        if (isLinuxConsole())
            platf.reset(new LinuxConsoleStrategy(disp, new NcursesInput(false)));
        else
            platf.reset(new PlatformStrategy(disp, new NcursesInput()));
    }
}

void THardwareInfo::restoreConsole()
{
    // Tear down input/output control by deleting the platform strategy.
    platf.reset();
}


BOOL THardwareInfo::getMouseEvent( MouseEventType& event )
{
    if (pendingEvent) {
        event = pendingMouseEvent.mouse;
        pendingEvent = 0;
        return True;
    }
    return False;
}

BOOL THardwareInfo::getKeyEvent( TEvent& event )
{
    /* This is a good place to refresh the display, since it guarantees a
     * a refresh each time an event is processed or there's a wait timeout,
     * and avoids unnecessary screen refreshes. */
    THardwareInfo::flushScreen();
    if (!pendingEvent)
    {
        if (platf->waitForEvent(eventTimeoutMs, event))
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
            else if (event.what & evMouse)
            {
                /* Like in the original implementation, let mouse events
                 * to be treated on the following polling loop. */
                pendingMouseEvent = event;
                pendingEvent = 1;
                return False;
            }
            return (Boolean) event.what != evNothing;
        }
    }
    return False;
}

ulong THardwareInfo::getTickCount()
{
    // This effectively gives a system time reference in milliseconds.
    // steady_clock is best suited for measuring intervals.
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count() / 55;
}

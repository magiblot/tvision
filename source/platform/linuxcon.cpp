#ifdef __linux__

#include <internal/linuxcon.h>
#include <internal/constmap.h>
#include <internal/stdioctl.h>
#include <internal/gpminput.h>
#include <linux/keyboard.h>
#include <linux/vt.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstring>

/* There are cases, such as the linux console, where it is possible to
 * get the state of keyboard modifiers (Shift/Ctrl/Alt), but captured
 * key events don't include that information. So, an extra translation
 * step must be done to get the actual Turbo Vision key codes. */

static constexpr auto keyCodeWithShift = constexpr_map<ushort, ushort>::from_array({
    { kbTab,        kbShiftTab      },
    { kbDel,        kbShiftDel      },
    { kbIns,        kbShiftIns      }
});

static constexpr auto keyCodeWithCtrl = constexpr_map<ushort, ushort>::from_array({
    { 0x001F,       kbCtrlBack      },
    { kbDel,        kbCtrlDel       },
    { kbEnd,        kbCtrlEnd       },
    { kbHome,       kbCtrlHome      },
    { kbIns,        kbCtrlIns       },
    { kbLeft,       kbCtrlLeft      },
    { kbPgDn,       kbCtrlPgDn      },
    { kbPgUp,       kbCtrlPgUp      },
    { kbRight,      kbCtrlRight     },
    { kbUp,         kbCtrlUp        },
    { kbDown,       kbCtrlDown      }
});

static constexpr auto keyCodeWithAlt = constexpr_map<ushort, ushort>::from_array({
    { kbBack,       kbAltBack       },
    { kbDel,        kbAltDel        },
    { kbEnd,        kbAltEnd        },
    { kbHome,       kbAltHome       },
    { kbIns,        kbAltIns        },
    { kbPgDn,       kbAltPgDn       },
    { kbPgUp,       kbAltPgUp       },
    { kbUp,         kbAltUp         },
    { kbDown,       kbAltDown       },
    { kbLeft,       kbAltLeft       },
    { kbRight,      kbAltRight      },
});

LinuxConsoleStrategy &LinuxConsoleStrategy::create( const StdioCtl &io, ScreenLifetime &scrl,
                                                    DisplayStrategy &display,
                                                    InputStrategy &input ) noexcept
{
    return *new LinuxConsoleStrategy(io, scrl, display, input, GpmInput::create());
}

LinuxConsoleStrategy::~LinuxConsoleStrategy()
{
    // The superclass always deletes 'input'. If we have a different object
    // in 'wrapper.input', we delete it too.
    if (&wrapper.input != &input)
        delete &wrapper.input;
}

void LinuxConsoleStrategy::forEachSource(void *args, void (&action)(void *, EventSource &)) noexcept
{
    action(args, wrapper);
    if (&wrapper.input != &input)
        action(args, input);
    UnixConsoleStrategy::forEachPrivateSource(args, action);
}

bool LinuxConsoleInput::getEvent(TEvent &ev) noexcept
{
    /* The keyboard event getter is usually unaware of key modifiers in the
     * console, so we add them on top of the previous translation. */
    if (input.getEvent(ev))
    {
        applyKeyboardModifiers(io, ev.keyDown);
        ushort keyCode = keyCodeWithModifiers(ev.keyDown.controlKeyState, ev.keyDown.keyCode);
        if (keyCode)
            ev.keyDown.keyCode = keyCode;
        // Do not set the Ctrl flag on these, as that would alter their meaning.
        // Note that there already exist kbCtrlBack and kbCtrlEnter.
        else if (ev.keyDown.keyCode == kbBack || ev.keyDown.keyCode == kbTab || ev.keyDown.keyCode == kbEnter)
            ev.keyDown.controlKeyState &= ~kbCtrlShift;
        return true;
    }
    return false;
}

bool LinuxConsoleInput::hasPendingEvents() noexcept
{
    return input.hasPendingEvents();
}

ushort LinuxConsoleInput::keyCodeWithModifiers(ulong controlKeyState, ushort keyCode) noexcept
{
    ushort result = 0;
    if ((controlKeyState & kbAltShift) && (result = keyCodeWithAlt[keyCode]))
        return result;
    if ((controlKeyState & kbCtrlShift) && (result = keyCodeWithCtrl[keyCode]))
        return result;
    if ((controlKeyState & kbShift) && (result = keyCodeWithShift[keyCode]))
        return result;
    return result;
}

void LinuxConsoleInput::applyKeyboardModifiers(const StdioCtl &io, KeyDownEvent &key) noexcept
{
    char res = 6;
    ulong actualModifiers = 0;
    if (ioctl(io.in(), TIOCLINUX, &res) != -1)
    {
        if ((res & (1 << KG_SHIFT)) && !key.textLength)
            actualModifiers |= kbShift;
        if (res & (1 << KG_CTRL))
            actualModifiers |= kbCtrlShift;
        if (res & (1 << KG_ALT))
            actualModifiers |= kbAltShift;
        /* Modifiers must be replaced and not OR'd. For instance, Shift+Tab is
         * captured as Alt+Tab by ncurses, and discarding the current Alt flag
         * makes it possible for it to be processed properly by Turbo Vision. */
        key.controlKeyState = actualModifiers;
    }
}

#endif // __linux__

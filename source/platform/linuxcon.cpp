#ifdef __linux__

#include <internal/linuxcon.h>
#include <internal/constmap.h>
#include <internal/stdioctl.h>
#include <linux/keyboard.h>
#include <linux/vt.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstring>

/* There are cases, such as the linux console, where it is possible to
 * get the state of keyboard modifiers (Shift/Ctrl/Alt), but captured
 * key events don't include that information. So, an extra translation
 * step must be done to get the actual Turbo Vision key codes. */

#if __cpp_constexpr >= 201304L
constexpr
#endif
static auto keyCodeWithShift = constexpr_map<ushort, ushort>::from_array({
    { kbTab,        kbShiftTab      },
    { kbDel,        kbShiftDel      },
    { kbIns,        kbShiftIns      }
});

#if __cpp_constexpr >= 201304L
constexpr
#endif
static auto keyCodeWithCtrl = constexpr_map<ushort, ushort>::from_array({
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

#if __cpp_constexpr >= 201304L
constexpr
#endif
static auto keyCodeWithAlt = constexpr_map<ushort, ushort>::from_array({
    { kbDel,        kbAltDel        },
    { kbEnd,        kbAltEnd        },
    { kbHome,       kbAltHome       },
    { kbIns,        kbAltIns        },
    { kbPgDn,       kbAltPgDn       },
    { kbPgUp,       kbAltPgUp       },
    { kbUp,         kbAltUp         },
    { kbDown,       kbAltDown       },
});

LinuxConsoleStrategy::LinuxConsoleStrategy( DisplayStrategy *d,
                                            FdInputStrategy *i ) :
    UnixPlatformStrategy(d, i),
    gpm(new GpmInput())
{
    /* The FdInputStrategy instance which reads key events is stored in
     * the 'input' attribute of PlatformStrategy, while the GpmInput instance
     * is stored in the 'gpm' attribute of this class. */
    if (input)
        ((FdInputStrategy &) *input).overrideEventGetter([&] (TEvent &ev) {
            return patchKeyEvent(ev);
        });
}

int LinuxConsoleStrategy::getButtonCount()
{
    return gpm->getButtonCount();
}

bool LinuxConsoleStrategy::patchKeyEvent(TEvent &ev)
{
    /* The keyboard event getter is usually unaware of key modifiers in the
     * console, so we add them on top of the previous translation. */
    if (input->getEvent(ev))
    {
        applyKeyboardModifiers(ev.keyDown);
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

ushort LinuxConsoleStrategy::keyCodeWithModifiers(ulong controlKeyState, ushort keyCode)
{
    switch (controlKeyState)
    {
        case kbShift: return keyCodeWithShift[keyCode];
        case kbCtrlShift: return keyCodeWithCtrl[keyCode];
        case kbAltShift: return keyCodeWithAlt[keyCode];
        default: return kbNoKey;
    }
}

void LinuxConsoleStrategy::applyKeyboardModifiers(KeyDownEvent &key)
{
    char res = 6;
    ulong actualModifiers = 0;
    if (ioctl(StdioCtl::in(), TIOCLINUX, &res) != -1)
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

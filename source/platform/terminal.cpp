#define Uses_TKeys
#include <tvision/tv.h>

#include <internal/terminal.h>
#include <internal/stdioctl.h>
#include <internal/constmap.h>
#include <internal/codepage.h>
#include <internal/getenv.h>
#include <internal/utf8.h>

namespace terminp
{

    // Modifier precedece: Shift < Ctrl < Alt

    static const const_unordered_map<ushort, ushort> ShiftKeyCode =
    {
        { kbF1,        kbShiftF1   }, { kbF2,        kbShiftF2   },
        { kbF3,        kbShiftF3   }, { kbF4,        kbShiftF4   },
        { kbF5,        kbShiftF5   }, { kbF6,        kbShiftF6   },
        { kbF7,        kbShiftF7   }, { kbF8,        kbShiftF8   },
        { kbF9,        kbShiftF9   }, { kbF10,       kbShiftF10  },
        { kbF11,       kbShiftF11  }, { kbF12,       kbShiftF12  },
        { kbIns,       kbShiftIns  }, { kbDel,       kbShiftDel  },
        { kbTab,       kbShiftTab  },
    };

    static const const_unordered_map<ushort, ushort> CtrlKeyCode =
    {
        { 'A', kbCtrlA }, { 'B', kbCtrlB }, { 'C', kbCtrlC }, { 'D', kbCtrlD },
        { 'E', kbCtrlE }, { 'F', kbCtrlF }, { 'G', kbCtrlG }, { 'H', kbCtrlH },
        { 'I', kbCtrlI }, { 'J', kbCtrlJ }, { 'K', kbCtrlK }, { 'L', kbCtrlL },
        { 'M', kbCtrlM }, { 'N', kbCtrlN }, { 'O', kbCtrlO }, { 'P', kbCtrlP },
        { 'Q', kbCtrlQ }, { 'R', kbCtrlR }, { 'S', kbCtrlS }, { 'T', kbCtrlT },
        { 'U', kbCtrlU }, { 'V', kbCtrlV }, { 'W', kbCtrlW }, { 'X', kbCtrlX },
        { 'Y', kbCtrlY }, { 'Z', kbCtrlZ },
        { kbF1,        kbCtrlF1    }, { kbF2,        kbCtrlF2    },
        { kbF3,        kbCtrlF3    }, { kbF4,        kbCtrlF4    },
        { kbF5,        kbCtrlF5    }, { kbF6,        kbCtrlF6    },
        { kbF7,        kbCtrlF7    }, { kbF8,        kbCtrlF8    },
        { kbF9,        kbCtrlF9    }, { kbF10,       kbCtrlF10   },
        { kbF11,       kbCtrlF11   }, { kbF12,       kbCtrlF12   },
        { kbShiftF1,   kbCtrlF1    }, { kbShiftF2,   kbCtrlF2    },
        { kbShiftF3,   kbCtrlF3    }, { kbShiftF4,   kbCtrlF4    },
        { kbShiftF5,   kbCtrlF5    }, { kbShiftF6,   kbCtrlF6    },
        { kbShiftF7,   kbCtrlF7    }, { kbShiftF8,   kbCtrlF8    },
        { kbShiftF9,   kbCtrlF9    }, { kbShiftF10,  kbCtrlF10   },
        { kbShiftF11,  kbCtrlF11   }, { kbShiftF12,  kbCtrlF12   },
        { kbBack,      kbCtrlBack  }, { kbEnter,     kbCtrlEnter },
        { kbDown,      kbCtrlDown  }, { kbUp,        kbCtrlUp    },
        { kbLeft,      kbCtrlLeft  }, { kbRight,     kbCtrlRight },
        { kbIns,       kbCtrlIns   }, { kbDel,       kbCtrlDel   },
        { kbHome,      kbCtrlHome  }, { kbEnd,       kbCtrlEnd   },
        { kbPgUp,      kbCtrlPgUp  }, { kbPgDn,      kbCtrlPgDn  },
        { kbShiftIns,  kbCtrlIns   }, { kbShiftDel,  kbCtrlDel   },
    };

    static const const_unordered_map<ushort, ushort> AltKeyCode =
    {
        { 'A', kbAltA }, { 'B', kbAltB }, { 'C', kbAltC }, { 'D', kbAltD },
        { 'E', kbAltE }, { 'F', kbAltF }, { 'G', kbAltG }, { 'H', kbAltH },
        { 'I', kbAltI }, { 'J', kbAltJ }, { 'K', kbAltK }, { 'L', kbAltL },
        { 'M', kbAltM }, { 'N', kbAltN }, { 'O', kbAltO }, { 'P', kbAltP },
        { 'Q', kbAltQ }, { 'R', kbAltR }, { 'S', kbAltS }, { 'T', kbAltT },
        { 'U', kbAltU }, { 'V', kbAltV }, { 'W', kbAltW }, { 'X', kbAltX },
        { 'Y', kbAltY }, { 'Z', kbAltZ },
        { kbCtrlA, kbAltA }, { kbCtrlB, kbAltB }, { kbCtrlC, kbAltC }, { kbCtrlD, kbAltD },
        { kbCtrlE, kbAltE }, { kbCtrlF, kbAltF }, { kbCtrlG, kbAltG }, { kbCtrlH, kbAltH },
        { kbCtrlI, kbAltI }, { kbCtrlJ, kbAltJ }, { kbCtrlK, kbAltK }, { kbCtrlL, kbAltL },
        { kbCtrlM, kbAltM }, { kbCtrlN, kbAltN }, { kbCtrlO, kbAltO }, { kbCtrlP, kbAltP },
        { kbCtrlQ, kbAltQ }, { kbCtrlR, kbAltR }, { kbCtrlS, kbAltS }, { kbCtrlT, kbAltT },
        { kbCtrlU, kbAltU }, { kbCtrlV, kbAltV }, { kbCtrlW, kbAltW }, { kbCtrlX, kbAltX },
        { '1', kbAlt1 }, { '2', kbAlt2 }, { '3', kbAlt3 }, { '4', kbAlt4 },
        { '5', kbAlt5 }, { '6', kbAlt6 }, { '7', kbAlt7 }, { '8', kbAlt8 },
        { '9', kbAlt9 }, { '0', kbAlt0 },
        { '-',         kbAltMinus  }, { '=',         kbAltEqual  },
        { ' ',         kbAltSpace  },
        { kbF1,        kbAltF1     }, { kbF2,        kbAltF2     },
        { kbF3,        kbAltF3     }, { kbF4,        kbAltF4     },
        { kbF5,        kbAltF5     }, { kbF6,        kbAltF6     },
        { kbF7,        kbAltF7     }, { kbF8,        kbAltF8     },
        { kbF9,        kbAltF9     }, { kbF10,       kbAltF10    },
        { kbF11,       kbAltF11    }, { kbF12,       kbAltF12    },
        { kbShiftF1,   kbAltF1     }, { kbShiftF2,   kbAltF2     },
        { kbShiftF3,   kbAltF3     }, { kbShiftF4,   kbAltF4     },
        { kbShiftF5,   kbAltF5     }, { kbShiftF6,   kbAltF6     },
        { kbShiftF7,   kbAltF7     }, { kbShiftF8,   kbAltF8     },
        { kbShiftF9,   kbAltF9     }, { kbShiftF10,  kbAltF10    },
        { kbShiftF11,  kbAltF11    }, { kbShiftF12,  kbAltF12    },
        { kbCtrlF1,    kbAltF1     }, { kbCtrlF2,    kbAltF2     },
        { kbCtrlF3,    kbAltF3     }, { kbCtrlF4,    kbAltF4     },
        { kbCtrlF5,    kbAltF5     }, { kbCtrlF6,    kbAltF6     },
        { kbCtrlF7,    kbAltF7     }, { kbCtrlF8,    kbAltF8     },
        { kbCtrlF9,    kbAltF9     }, { kbCtrlF10,   kbAltF10    },
        { kbCtrlF11,   kbAltF11    }, { kbCtrlF12,   kbAltF12    },
        { kbBack,      kbAltBack   },
        { kbDown,      kbAltDown   }, { kbUp,        kbAltUp     },
        { kbLeft,      kbAltLeft   }, { kbRight,     kbAltRight  },
        { kbIns,       kbAltIns    }, { kbDel,       kbAltDel    },
        { kbHome,      kbAltHome   }, { kbEnd,       kbAltEnd    },
        { kbPgUp,      kbAltPgUp   }, { kbPgDn,      kbAltPgDn   },
        { kbShiftIns,  kbAltIns    }, { kbShiftDel,  kbAltDel    },
        { kbCtrlBack,  kbAltBack   },
        { kbCtrlDown,  kbAltDown   }, { kbCtrlUp,    kbAltUp     },
        { kbCtrlLeft,  kbAltLeft   }, { kbCtrlRight, kbAltRight  },
        { kbCtrlIns,   kbAltIns    }, { kbCtrlDel,   kbAltDel    },
        { kbCtrlHome,  kbAltHome   }, { kbCtrlEnd,   kbAltEnd    },
        { kbCtrlPgUp,  kbAltPgUp   }, { kbCtrlPgDn,  kbAltPgDn   },
    };

    static inline void setModifier( KeyDownEvent &keyDown, ushort mod,
                                    const const_unordered_map<ushort, ushort> &keyMap )
    {
        keyDown.controlKeyState |= mod;
        keyDown.textLength = 0;
        char c = keyDown.charScan.charCode;
        if (keyDown.charScan.scanCode == 0)
            keyDown.charScan.charCode = ('a' <= c && c <= 'z') ? (c - 'a' + 'A') : c;
        ushort keyCode = keyMap[keyDown.keyCode];
        if (keyCode)
            keyDown.keyCode = keyCode;
    }

    static void setShiftModifier(KeyDownEvent &keyDown)
    {
        setModifier(keyDown, kbShift, ShiftKeyCode);
    }

    static void setCtrlModifier(KeyDownEvent &keyDown)
    {
        setModifier(keyDown, kbCtrlShift, CtrlKeyCode);
    }

    static void setAltModifier(KeyDownEvent &keyDown)
    {
        setModifier(keyDown, kbAltShift, AltKeyCode);
    }

    static KeyDownEvent keyWithModifiers(ushort keyCode, ulong mods)
    {
        KeyDownEvent keyDown {{keyCode}, mods};
        if (mods & kbShift) setShiftModifier(keyDown);
        if (mods & kbCtrlShift) setCtrlModifier(keyDown);
        if (mods & kbAltShift) setAltModifier(keyDown);
        return keyDown;
    }

    const uint XTermModDefault = 1;

    static KeyDownEvent keyWithXTermMods(ushort keyCode, uint mods)
    {
        mods -= XTermModDefault;
        ulong tvmods =
            (kbShift & -(mods & 1))
            | (kbAltShift & -(mods & 2))
            | (kbCtrlShift & -(mods & 4))
            ;
        return keyWithModifiers(keyCode, tvmods);
    }

    static bool isAlpha(uint32_t ascii)
    {
        return ' ' <= ascii && ascii < 127;
    };

    static bool isPrivate(uint32_t codepoint)
    {
        return 57344 <= codepoint && codepoint <= 63743;
    };

    static bool keyFromCodepoint(uint value, uint mods, KeyDownEvent &keyDown)
    {

        ushort keyCode = 0;
        switch (value)
        {
            case     8: keyCode = kbBack;   break;
            case     9: keyCode = kbTab;    break;
            case    13: keyCode = kbEnter;  break;
            case    27: keyCode = kbEsc;    break;
            case   127: keyCode = kbBack;   break;
            // Functional keys as represented in Kitty's keyboard protocol.
            // https://sw.kovidgoyal.net/kitty/keyboard-protocol.html#functional
            // Keypad.
            case 57414: keyCode = kbEnter;  break;
            case 57417: keyCode = kbLeft;   break;
            case 57418: keyCode = kbRight;  break;
            case 57419: keyCode = kbUp;     break;
            case 57420: keyCode = kbDown;   break;
            case 57421: keyCode = kbPgUp;   break;
            case 57422: keyCode = kbPgDn;   break;
            case 57423: keyCode = kbHome;   break;
            case 57424: keyCode = kbEnd;    break;
            case 57425: keyCode = kbIns;    break;
            case 57426: keyCode = kbDel;    break;
            default: if (isAlpha(value)) keyCode = value;
        }
        keyDown = keyWithXTermMods(keyCode, mods);
        // Note that 'keyDown.keyCode' may be different from 'keyCode'
        // if there are modifiers.
        if ( (keyDown.keyCode == 0 || isAlpha(keyDown.keyCode)) &&
            ' ' <= value && !isPrivate(value) )
        {
            keyDown.textLength = utf32To8(value, keyDown.text);
            keyDown.charScan.charCode =
                CpTranslator::printableFromUtf8({keyDown.text, keyDown.textLength});
        }
        return keyDown.keyCode != 0 || keyDown.textLength != 0;
    }

    static bool keyFromLetter(uint letter, uint mod, KeyDownEvent &keyDown)
    {
        ushort keyCode = 0;
        switch (letter)
        {
            case 'A': keyCode = kbUp; break;
            case 'B': keyCode = kbDown; break;
            case 'C': keyCode = kbRight; break;
            case 'D': keyCode = kbLeft; break;
            case 'E': keyCode = kbNoKey; break; // Numpad 5, "KP_Begin".
            case 'F': keyCode = kbEnd; break;
            case 'H': keyCode = kbHome; break;
            case 'P': keyCode = kbF1; break;
            case 'Q': keyCode = kbF2; break;
            case 'R': keyCode = kbF3; break;
            case 'S': keyCode = kbF4; break;
            case 'Z': keyCode = kbTab; break;
            // Keypad in XTerm (SS3).
            case 'j': keyCode = '*'; break;
            case 'k': keyCode = '+'; break;
            case 'm': keyCode = '-'; break;
            case 'M': keyCode = kbEnter; break;
            case 'n': keyCode = kbDel; break;
            case 'o': keyCode = '/'; break;
            case 'p': keyCode = kbIns; break;
            case 'q': keyCode = kbEnd; break;
            case 'r': keyCode = kbDown; break;
            case 's': keyCode = kbPgDn; break;
            case 't': keyCode = kbLeft; break;
            case 'u': keyCode = kbNoKey; break; // Numpad 5, "KP_Begin".
            case 'v': keyCode = kbRight; break;
            case 'w': keyCode = kbHome; break;
            case 'x': keyCode = kbUp; break;
            case 'y': keyCode = kbPgUp; break;
            default: return false;
        }
        keyDown = keyWithXTermMods(keyCode, mod);
        // Note that 'keyDown.keyCode' may be different from 'keyCode'
        // if there are modifiers.
        if (isAlpha(keyDown.keyCode))
        {
            keyDown.text[0] = keyDown.keyCode;
            keyDown.textLength = 1;
        }
        return true;
    }

} // namespace terminp

// The default mouse experience with Ncurses is not always good. To work around
// some issues, we request and parse mouse events manually.

void TermIO::mouseOn()
{
    TStringView seq = "\x1B[?1001s" // Save old highlight mouse reporting.
                      "\x1B[?1000h" // Enable mouse reporting.
                      "\x1B[?1002h" // Enable mouse drag reporting.
                      "\x1B[?1006h" // Enable SGR extended mouse reporting.
                    ;
    consoleWrite(seq.data(), seq.size());
}

void TermIO::mouseOff()
{
    TStringView seq = "\x1B[?1006l" // Disable SGR extended mouse reporting.
                      "\x1B[?1002l" // Disable mouse drag reporting.
                      "\x1B[?1000l" // Disable mouse reporting.
                      "\x1B[?1001r" // Restore old highlight mouse reporting.
                    ;
    consoleWrite(seq.data(), seq.size());
}

void TermIO::kittyKeysOn()
{
    // https://sw.kovidgoyal.net/kitty/keyboard-protocol.html
    TStringView seq = "\x1B[>1u"; // Disambiguate escape codes.
    consoleWrite(seq.data(), seq.size());
}

void TermIO::kittyKeysOff()
{
    TStringView seq = "\x1B[<u";
    consoleWrite(seq.data(), seq.size());
}

bool TermIO::acceptMouseEvent(TEvent &ev, MouseState &oldm, const MouseState &newm)
{
    // Some terminal emulators send a mouse event every pixel the graphical
    // mouse cursor moves over the window. Filter out those unnecessary
    // events.
    if (newm.buttons != oldm.buttons || newm.wheel || newm.where != oldm.where)
    {
        ev.what = evMouse;
        ev.mouse = {};
        ev.mouse.buttons = newm.buttons;
        ev.mouse.where = newm.where;
        ev.mouse.wheel = newm.wheel;
        oldm = newm;
        return true;
    }
    return false;
}

void TermIO::setAltModifier(KeyDownEvent &keyDown)
{
    terminp::setAltModifier(keyDown);
}

ParseResult TermIO::parseEscapeSeq(GetChBuf &buf, TEvent &ev, MouseState &oldm)
// Pre: "\x1B" has just been read.
{
    ParseResult res = Rejected;
    switch (buf.get())
    {
        case '[':
            switch (buf.get())
            {
                // Note: mouse events are usually detected in 'NcursesInput::parseCursesMouse'.
                case 'M':
                    return parseX10Mouse(buf, ev, oldm) == Accepted ? Accepted : Ignored;
                case '<':
                    return parseSGRMouse(buf, ev, oldm) == Accepted ? Accepted : Ignored;
                default:
                {
                    buf.unget();
                    CSIData csi;
                    if (csi.readFrom(buf))
                    {
                        if (csi.terminator() == 'u')
                            res = parseFixTermKey(csi, ev);
                        else
                            res = parseCSIKey(csi, ev);
                    }
                    break;
                }
            }
            break;
        case 'O':
            res = parseSS3Key(buf, ev);
            break;
        case '\x1B':
            res = parseEscapeSeq(buf, ev, oldm);
            if (res == Accepted && ev.what == evKeyDown)
                setAltModifier(ev.keyDown);
            break;
    }
    if (res == Rejected)
        buf.reject();
    return res;
}

ParseResult TermIO::parseX10Mouse(GetChBuf &buf, TEvent &ev, MouseState &oldm)
// Pre: "\x1B[M" has just been read.
// The complete sequence looks like "\x1B[Mabc", where:
// * 'a' is the button number plus 32.
// * 'b' is the column number (one-based) plus 32.
// * 'c' is the row number (one-based) plus 32.
{
    int but, col, row;
    but = buf.get();
    if (but < 32 || 255 < but) return Rejected;
    but -= 32;
    for (int *i : {&col, &row})
    {
        *i = buf.get();
        if (*i < 0 || 255 < *i)
            return Rejected;
        // In theory, this encoding only supports coordinates in the range [0, 222].
        // However, some terminal emulators (e.g. urxvt) keep increasing the
        // counters, causing an overflow. We can take advantage of this to support
        // more coordinates, but we definitely don't want to reject the sequence,
        // as that will cause Ctrl+key events to be generated.
        if (*i > 32)
            *i -= 32;
        else
            *i += (256 - 32);
        // Make it zero-based.
        --*i;
    }

    MouseState newm = {};
    newm.where = {col, row};
    newm.buttons = oldm.buttons;
    switch (but)
    {
        case 0: // Press.
        case 32: // Drag.
            newm.buttons |= mbLeftButton; break;
        case 1:
        case 33:
            newm.buttons |= mbMiddleButton; break;
        case 2:
        case 34:
            newm.buttons |= mbRightButton; break;
        case 3: newm.buttons = 0; break; // Release.
        case 64: newm.wheel = mwUp; break;
        case 65: newm.wheel = mwDown; break;
    }
    return acceptMouseEvent(ev, oldm, newm) ? Accepted : Ignored;
}

ParseResult TermIO::parseSGRMouse(GetChBuf &buf, TEvent &ev, MouseState &oldm)
// https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Extended-coordinates
// Pre: "\x1B[<" has just been read.
// The complete sequence looks like "\x1B[<a;b;cM" or "\x1B[<a;b;cm", where:
// * 'a' is a sequence of digits representing the button number in decimal.
// * 'b' is a sequence of digits representing the column number (one-based) in decimal.
// * 'c' is a sequence of digits representing the row number (one-based) in decimal.
// The sequence ends with 'M' on button press and on 'm' on button release.
{
    uint but, state;
    // IntelliJ may emit negative coordinates.
    int col, row;
    if (!buf.getNum(but)) return Rejected;
    if (!buf.getInt(col) || !buf.getInt(row)) return Rejected;
    // Make the coordinates zero-based.
    row = max(row, 1);
    col = max(col, 1);
    --row, --col;
    // Finally, the press/release state.
    state = (uint) buf.last();
    if (!(state == 'M' || state == 'm')) return Rejected;

    MouseState newm = {};
    newm.where = {col, row};
    newm.buttons = oldm.buttons;
    if (state == 'M') // Press, wheel or drag.
    {
        switch (but)
        {
            case 0:
            case 32:
                newm.buttons |= mbLeftButton; break;
            case 1:
            case 33:
                newm.buttons |= mbMiddleButton; break;
            case 2:
            case 34:
                newm.buttons |= mbRightButton; break;
            case 64: newm.wheel = mwUp; break;
            case 65: newm.wheel = mwDown; break;
        }
    }
    else // Release.
    {
        switch (but)
        {
            case 0: newm.buttons &= ~mbLeftButton; break;
            case 1: newm.buttons &= ~mbMiddleButton; break;
            case 2: newm.buttons &= ~mbRightButton; break;
        }
    }
    return acceptMouseEvent(ev, oldm, newm) ? Accepted : Ignored;
}

// The functions below are meant to parse a few sequences emitted
// by terminals that do not match their terminfo / termcap entries, e.g.
// Shift F1-4 on Konsole and F1-4 on Putty. It's easier than fixing the
// application or updating the terminal database.

ParseResult TermIO::parseCSIKey(const CSIData &csi, TEvent &ev)
// https://invisible-island.net/xterm/xterm-function-keys.html
// https://invisible-island.net/xterm/ctlseqs/ctlseqs.html
{
    using namespace terminp;
    uint terminator = csi.terminator();
    if (csi.length == 1 && terminator == '~')
    {
        switch (csi.val[0])
        {
            case 1: ev.keyDown = {{kbHome}}; break;
            case 2: ev.keyDown = {{kbIns}}; break;
            case 3: ev.keyDown = {{kbDel}}; break;
            case 4: ev.keyDown = {{kbEnd}}; break;
            case 5: ev.keyDown = {{kbPgUp}}; break;
            case 6: ev.keyDown = {{kbPgDn}}; break;
            // Note that these numbers can be interpreted in different ways, i.e.
            // they could be interpreted as F1-F12 instead of F1-F10.
            // But this fallback is triggered by Putty, which uses F1-F10.
            case 11: ev.keyDown = {{kbF1}}; break;
            case 12: ev.keyDown = {{kbF2}}; break;
            case 13: ev.keyDown = {{kbF3}}; break;
            case 14: ev.keyDown = {{kbF4}}; break;
            case 15: ev.keyDown = {{kbF5}}; break;
            case 17: ev.keyDown = {{kbF6}}; break;
            case 18: ev.keyDown = {{kbF7}}; break;
            case 19: ev.keyDown = {{kbF8}}; break;
            case 20: ev.keyDown = {{kbF9}}; break;
            case 21: ev.keyDown = {{kbF10}}; break;
            case 23: ev.keyDown = {{kbShiftF1}, kbShift}; break;
            case 24: ev.keyDown = {{kbShiftF2}, kbShift}; break;
            case 25: ev.keyDown = {{kbShiftF3}, kbShift}; break;
            case 26: ev.keyDown = {{kbShiftF4}, kbShift}; break;
            case 28: ev.keyDown = {{kbShiftF5}, kbShift}; break;
            case 29: ev.keyDown = {{kbShiftF6}, kbShift}; break;
            case 31: ev.keyDown = {{kbShiftF7}, kbShift}; break;
            case 32: ev.keyDown = {{kbShiftF8}, kbShift}; break;
            case 33: ev.keyDown = {{kbShiftF9}, kbShift}; break;
            case 34: ev.keyDown = {{kbShiftF10}, kbShift}; break;
            default: return Rejected;
        }
    }
    else if (csi.length == 1 && csi.val[0] == 1)
    {
        if (!keyFromLetter(terminator, XTermModDefault, ev.keyDown))
            return Rejected;
    }
    else if (csi.length == 2)
    {
        uint mod = csi.val[1];
        if (csi.val[0] == 1)
        {
            if (!keyFromLetter(terminator, mod, ev.keyDown))
                return Rejected;
        }
        else if (terminator == '~')
        {
            ushort keyCode = 0;
            switch (csi.val[0])
            {
                case  2: keyCode = kbIns; break;
                case  3: keyCode = kbDel; break;
                case  5: keyCode = kbPgUp; break;
                case  6: keyCode = kbPgDn; break;
                case 11: keyCode = kbF1; break;
                case 12: keyCode = kbF2; break;
                case 13: keyCode = kbF3; break;
                case 14: keyCode = kbF4; break;
                case 15: keyCode = kbF5; break;
                case 17: keyCode = kbF6; break;
                case 18: keyCode = kbF7; break;
                case 19: keyCode = kbF8; break;
                case 20: keyCode = kbF9; break;
                case 21: keyCode = kbF10; break;
                case 23: keyCode = kbF11; break;
                case 24: keyCode = kbF12; break;
                default: return Rejected;
            }
            ev.keyDown = keyWithXTermMods(keyCode, csi.val[1]);
        }
        else
            return Rejected;
    }
    else if (csi.length == 3 && csi.val[0] == 27 && terminator == '~')
    {
        // XTerm's "modifyOtherKeys" mode.
        uint key = csi.val[2];
        uint mod = csi.val[1];
        if (!keyFromCodepoint(key, mod, ev.keyDown))
            return Ignored;
    }
    else
        return Rejected;
    ev.what = evKeyDown;
    return Accepted;
}

ParseResult TermIO::parseSS3Key(GetChBuf &buf, TEvent &ev)
// https://invisible-island.net/xterm/xterm-function-keys.html
// Pre: "\x1BO" has just been read.
// Konsole, IntelliJ.
{
    using namespace terminp;
    uint mod;
    if (!buf.getNum(mod)) return Rejected;
    uint key = (uint) buf.last();
    if (!keyFromLetter(key, mod, ev.keyDown)) return Rejected;
    ev.what = evKeyDown;
    return Accepted;
}

ParseResult TermIO::parseFixTermKey(const CSIData &csi, TEvent &ev)
// https://sw.kovidgoyal.net/kitty/keyboard-protocol.html
// http://www.leonerd.org.uk/hacks/fixterms/
{
    using namespace terminp;

    if (csi.length < 1 || csi.terminator() != 'u')
        return Rejected;

    uint key = csi.val[0];
    uint mods = (csi.length > 1) ? max(csi.val[1], 1) : 1;
    if (keyFromCodepoint(key, mods, ev.keyDown))
    {
        ev.what = evKeyDown;
        return Accepted;
    }
    return Ignored;
}

#ifdef _TV_UNIX
#include <unistd.h>
#include <sys/ioctl.h>

TPoint TermIO::Unix::getSize()
{
    struct winsize w;
    for (int fd : {StdioCtl::in(), StdioCtl::out()})
    {
        if (ioctl(fd, TIOCGWINSZ, &w) != -1)
        {
            int env_col = getEnv<int>("COLUMNS", INT_MAX);
            int env_row = getEnv<int>("LINES", INT_MAX);
            return {
                min(max(w.ws_col, 0), max(env_col, 0)),
                min(max(w.ws_row, 0), max(env_row, 0)),
            };
        }
    }
    return {0, 0};
}

void TermIO::Unix::consoleWrite(const void *data, size_t bytes)
{
    fflush(StdioCtl::fout());
    int rr = ::write(StdioCtl::out(), data, bytes);
    (void) rr;
}

#elif defined(_WIN32)

void TermIO::Win32::consoleWrite(const void *data, size_t bytes)
{
    WriteConsole(StdioCtl::out(), data, bytes, nullptr, nullptr);
}

#endif // _TV_UNIX

bool TermIO::isLinuxConsole()
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
#endif
    return false;
}

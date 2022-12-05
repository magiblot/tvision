#define Uses_TKeys
#define Uses_THardwareInfo
#include <tvision/tv.h>

#include <internal/terminal.h>
#include <internal/stdioctl.h>
#include <internal/constmap.h>
#include <internal/codepage.h>
#include <internal/getenv.h>
#include <internal/utf8.h>
#include <internal/base64.h>

namespace tvision
{

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
        { kbTab,       kbCtrlTab   },
        { kbShiftIns,  kbCtrlIns   }, { kbShiftDel,  kbCtrlDel   },
        { kbShiftTab,  kbCtrlTab   },
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
        { kbBack,      kbAltBack   }, { kbEnter,     kbAltEnter  },
        { kbDown,      kbAltDown   }, { kbUp,        kbAltUp     },
        { kbLeft,      kbAltLeft   }, { kbRight,     kbAltRight  },
        { kbIns,       kbAltIns    }, { kbDel,       kbAltDel    },
        { kbHome,      kbAltHome   }, { kbEnd,       kbAltEnd    },
        { kbPgUp,      kbAltPgUp   }, { kbPgDn,      kbAltPgDn   },
        { kbTab,       kbAltTab    }, { kbEsc,       kbAltEsc    },
        { kbShiftIns,  kbAltIns    }, { kbShiftDel,  kbAltDel    },
        { kbShiftTab,  kbAltTab    },
        { kbCtrlBack,  kbAltBack   },
        { kbCtrlDown,  kbAltDown   }, { kbCtrlUp,    kbAltUp     },
        { kbCtrlLeft,  kbAltLeft   }, { kbCtrlRight, kbAltRight  },
        { kbCtrlIns,   kbAltIns    }, { kbCtrlDel,   kbAltDel    },
        { kbCtrlHome,  kbAltHome   }, { kbCtrlEnd,   kbAltEnd    },
        { kbCtrlPgUp,  kbAltPgUp   }, { kbCtrlPgDn,  kbAltPgDn   },
        { kbCtrlTab,   kbAltTab    },
    };

    static const const_unordered_map<uchar, ushort> virtualKeyCodeToKeyCode =
    {
        { VK_BACK,      kbBack      }, { VK_TAB,        kbTab       },
        { VK_RETURN,    kbEnter     }, { VK_ESCAPE,     kbEsc       },
        { VK_PRIOR,     kbPgUp      }, { VK_NEXT,       kbPgDn      },
        { VK_END,       kbEnd       }, { VK_HOME,       kbHome      },
        { VK_LEFT,      kbLeft      }, { VK_UP,         kbUp        },
        { VK_RIGHT,     kbRight     }, { VK_DOWN,       kbDown      },
        { VK_INSERT,    kbIns       }, { VK_DELETE,     kbDel       },
        { VK_NUMPAD0,   '0'         }, { VK_NUMPAD1,    '1'         },
        { VK_NUMPAD2,   '2'         }, { VK_NUMPAD3,    '3'         },
        { VK_NUMPAD4,   '4'         }, { VK_NUMPAD5,    '5'         },
        { VK_NUMPAD6,   '6'         }, { VK_NUMPAD7,    '7'         },
        { VK_NUMPAD8,   '8'         }, { VK_NUMPAD9,    '9'         },
        { VK_MULTIPLY,  '*'         }, { VK_ADD,        '+'         },
        { VK_SEPARATOR, '|'         }, { VK_SUBTRACT,   '-'         },
        { VK_DECIMAL,   '.'         }, { VK_DIVIDE,     '/'         },
        { VK_F1,        kbF1        }, { VK_F2,         kbF2        },
        { VK_F3,        kbF3        }, { VK_F4,         kbF4        },
        { VK_F5,        kbF5        }, { VK_F6,         kbF6        },
        { VK_F7,        kbF7        }, { VK_F8,         kbF8        },
        { VK_F9,        kbF9        }, { VK_F10,        kbF10       },
        { VK_F11,       kbF11       }, { VK_F12,        kbF12       },
    };

    static inline void setModifier( KeyDownEvent &keyDown, ushort mod,
                                    const const_unordered_map<ushort, ushort> &keyMap ) noexcept
    {
        keyDown.controlKeyState |= mod;
        uchar c = keyDown.charScan.charCode;
        if (keyDown.charScan.scanCode == 0)
            c = ('a' <= c && c <= 'z') ? (c - 'a' + 'A') : c;
        ushort testKeyCode = keyDown.charScan.scanCode << 8 | c;
        if (ushort keyCode = keyMap[testKeyCode])
        {
            keyDown.keyCode = keyCode;
            if (keyDown.charScan.charCode < ' ')
                keyDown.textLength = 0;
        }
    }

    static void setShiftModifier(KeyDownEvent &keyDown) noexcept
    {
        setModifier(keyDown, kbShift, ShiftKeyCode);
    }

    static void setCtrlModifier(KeyDownEvent &keyDown) noexcept
    {
        setModifier(keyDown, kbCtrlShift, CtrlKeyCode);
    }

    static void setAltModifier(KeyDownEvent &keyDown) noexcept
    {
        setModifier(keyDown, kbAltShift, AltKeyCode);
    }

    static KeyDownEvent keyWithModifiers(ushort keyCode, ushort mods) noexcept
    {
        KeyDownEvent keyDown {{keyCode}, mods};
        if (mods & kbShift) setShiftModifier(keyDown);
        if (mods & kbCtrlShift) setCtrlModifier(keyDown);
        if (mods & kbAltShift) setAltModifier(keyDown);
        return keyDown;
    }

    const uint XTermModDefault = 1;

    static KeyDownEvent keyWithXTermMods(ushort keyCode, uint mods) noexcept
    {
        mods -= XTermModDefault;
        ushort tvmods =
            (kbShift & -(mods & 1))
            | (kbAltShift & -(mods & 2))
            | (kbCtrlShift & -(mods & 4))
            ;
        return keyWithModifiers(keyCode, tvmods);
    }

    static bool isAlpha(uint32_t ascii) noexcept
    {
        return ' ' <= ascii && ascii < 127;
    };

    static bool isPrivate(uint32_t codepoint) noexcept
    {
        return 57344 <= codepoint && codepoint <= 63743;
    };

    static bool keyFromCodepoint(uint value, uint mods, KeyDownEvent &keyDown) noexcept
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

    static bool keyFromLetter(uint letter, uint mod, KeyDownEvent &keyDown) noexcept
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

void TermIO::mouseOn(const StdioCtl &io) noexcept
{
    TStringView seq = "\x1B[?1001s" // Save old highlight mouse reporting.
                      "\x1B[?1000h" // Enable mouse reporting.
                      "\x1B[?1002h" // Enable mouse drag reporting.
                      "\x1B[?1006h" // Enable SGR extended mouse reporting.
                    ;
    io.write(seq.data(), seq.size());
}

void TermIO::mouseOff(const StdioCtl &io) noexcept
{
    TStringView seq = "\x1B[?1006l" // Disable SGR extended mouse reporting.
                      "\x1B[?1002l" // Disable mouse drag reporting.
                      "\x1B[?1000l" // Disable mouse reporting.
                      "\x1B[?1001r" // Restore old highlight mouse reporting.
                    ;
    io.write(seq.data(), seq.size());
}

void TermIO::keyModsOn(const StdioCtl &io) noexcept
{
    // https://invisible-island.net/xterm/ctlseqs/ctlseqs.html
    // https://sw.kovidgoyal.net/kitty/keyboard-protocol.html
    TStringView seq = "\x1B[?1036s" // Save metaSendsEscape (XTerm).
                      "\x1B[?1036h" // Enable metaSendsEscape (XTerm).
                      "\x1B[>4;1m"  // Enable modifyOtherKeys (XTerm).
                      "\x1B[>1u"    // Disambiguate escape codes (Kitty).
                      "\x1B_far2l1\x07" // Enable far2l extended input.
                    ;
    io.write(seq.data(), seq.size());
}

void TermIO::keyModsOff(const StdioCtl &io) noexcept
{
    TStringView seq = "\x1B_far2l0\x07" // Disable far2l extended input.
                      "\x1B[<u"     // Restore previous keyboard mode (Kitty).
                      "\x1B[>4m"    // Reset modifyOtherKeys (XTerm).
                      "\x1B[?1036r" // Restore metaSendsEscape (XTerm).
                    ;
    io.write(seq.data(), seq.size());
}

void TermIO::setAltModifier(KeyDownEvent &keyDown) noexcept
{
    terminp::setAltModifier(keyDown);
}

void TermIO::normalizeKey(KeyDownEvent &keyDown) noexcept
{
    using namespace terminp;
    ushort mods = keyDown.controlKeyState;
    if (keyDown.controlKeyState & kbShift) setShiftModifier(keyDown);
    if (keyDown.controlKeyState & kbCtrlShift) setCtrlModifier(keyDown);
    if (keyDown.controlKeyState & kbAltShift) setAltModifier(keyDown);
    keyDown.controlKeyState = mods;
}

ParseResult TermIO::parseEscapeSeq(GetChBuf &buf, TEvent &ev, InputState &state) noexcept
// Pre: "\x1B" has just been read.
{
    ParseResult res = Rejected;
    switch (buf.get())
    {
        case '_':
            if (buf.get() == 'f' && buf.get() == '2' && buf.get() == 'l')
                return parseFar2lInput(buf, ev, state) == Accepted ? Accepted : Ignored;
        case '[':
            switch (buf.get())
            {
                // Note: mouse events are usually detected in 'NcursesInput::parseCursesMouse'.
                case 'M':
                    return parseX10Mouse(buf, ev, state) == Accepted ? Accepted : Ignored;
                case '<':
                    return parseSGRMouse(buf, ev, state) == Accepted ? Accepted : Ignored;
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
            res = parseEscapeSeq(buf, ev, state);
            if (res == Accepted && ev.what == evKeyDown)
                setAltModifier(ev.keyDown);
            break;
    }
    if (res == Rejected)
        buf.reject();
    return res;
}

const ushort
    mmAlt = 0x08,
    mmCtrl = 0x10;

ParseResult TermIO::parseX10Mouse(GetChBuf &buf, TEvent &ev, InputState &state) noexcept
// Pre: "\x1B[M" has just been read.
// The complete sequence looks like "\x1B[Mabc", where:
// * 'a' is the button number plus 32.
// * 'b' is the column number (one-based) plus 32.
// * 'c' is the row number (one-based) plus 32.
{
    uint butm = (uint) buf.get();
    uint mod = butm & (mmAlt | mmCtrl);
    uint but = (butm & ~(mmAlt | mmCtrl)) - 32;
    if (255 - 32 < but) return Rejected;
    int col, row;
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

    ev.what = evMouse;
    ev.mouse = {};
    ev.mouse.where = {col, row};
    ev.mouse.controlKeyState = (mod & mmAlt ? kbAltShift : 0) | (mod & mmCtrl ? kbCtrlShift : 0);
    switch (but)
    {
        case 0: // Press.
        case 32: // Drag.
            state.buttons |= mbLeftButton; break;
        case 1:
        case 33:
            state.buttons |= mbMiddleButton; break;
        case 2:
        case 34:
            state.buttons |= mbRightButton; break;
        case 3: state.buttons = 0; break; // Release.
        case 64: ev.mouse.wheel = mwUp; break;
        case 65: ev.mouse.wheel = mwDown; break;
    }
    ev.mouse.buttons = state.buttons;
    return Accepted;
}

ParseResult TermIO::parseSGRMouse(GetChBuf &buf, TEvent &ev, InputState &state) noexcept
// https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Extended-coordinates
// Pre: "\x1B[<" has just been read.
// The complete sequence looks like "\x1B[<a;b;cM" or "\x1B[<a;b;cm", where:
// * 'a' is a sequence of digits representing the button number in decimal.
// * 'b' is a sequence of digits representing the column number (one-based) in decimal.
// * 'c' is a sequence of digits representing the row number (one-based) in decimal.
// The sequence ends with 'M' on button press and on 'm' on button release.
{
    uint butm;
    if (!buf.getNum(butm)) return Rejected;
    uint mod = butm & (mmAlt | mmCtrl);
    uint but = butm & ~(mmAlt | mmCtrl);
    // IntelliJ may emit negative coordinates.
    int col, row;
    if (!buf.getInt(col) || !buf.getInt(row)) return Rejected;
    // Make the coordinates zero-based.
    row = max(row, 1);
    col = max(col, 1);
    --row, --col;
    // Finally, the press/release state.
    uint type = (uint) buf.last();
    if (!(type == 'M' || type == 'm')) return Rejected;

    ev.what = evMouse;
    ev.mouse = {};
    ev.mouse.where = {col, row};
    ev.mouse.controlKeyState = (mod & mmAlt ? kbAltShift : 0) | (mod & mmCtrl ? kbCtrlShift : 0);
    if (type == 'M') // Press, wheel or drag.
    {
        switch (but)
        {
            case 0:
            case 32:
                state.buttons |= mbLeftButton; break;
            case 1:
            case 33:
                state.buttons |= mbMiddleButton; break;
            case 2:
            case 34:
                state.buttons |= mbRightButton; break;
            case 64: ev.mouse.wheel = mwUp; break;
            case 65: ev.mouse.wheel = mwDown; break;
        }
    }
    else // Release.
    {
        switch (but)
        {
            case 0: state.buttons &= ~mbLeftButton; break;
            case 1: state.buttons &= ~mbMiddleButton; break;
            case 2: state.buttons &= ~mbRightButton; break;
        }
    }
    ev.mouse.buttons = state.buttons;
    return Accepted;
}

// The functions below are meant to parse a few sequences emitted
// by terminals that do not match their terminfo / termcap entries, e.g.
// Shift F1-4 on Konsole and F1-4 on Putty. It's easier than fixing the
// application or updating the terminal database.

ParseResult TermIO::parseCSIKey(const CSIData &csi, TEvent &ev) noexcept
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
                case 29: keyCode = kbNoKey; break; // Menu key (XTerm).
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

ParseResult TermIO::parseSS3Key(GetChBuf &buf, TEvent &ev) noexcept
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

ParseResult TermIO::parseFixTermKey(const CSIData &csi, TEvent &ev) noexcept
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

ParseResult TermIO::parseFar2lInput(GetChBuf &buf, TEvent &ev, InputState &state) noexcept
// Pre: "\x1B_far2l" has just been read.
{
    using namespace terminp;
    enum { k = 32 };
    char s[4*k];
    size_t len = 0;
    char c;
    while (c = buf.get(), c != -1 && c != '\x07')
        if (len < sizeof(s))
            s[len++] = c;
    TStringView input {s, len};

    uint8_t out[3*k];
    size_t outLen = decodeBase64(input, out);
    if (outLen > 0)
    {
        if (out[outLen - 1] == 'K' && outLen - 1 == 14)
        {
            KEY_EVENT_RECORD kev {};
            kev.bKeyDown = 1;
            memcpy(&kev.wRepeatCount,      &out[0],  2);
            memcpy(&kev.wVirtualKeyCode,   &out[2],  2);
            memcpy(&kev.wVirtualScanCode,  &out[4],  2);
            memcpy(&kev.dwControlKeyState, &out[6],  4);
            memcpy(&kev.uChar.UnicodeChar, &out[10], 4);

            if (uint16_t keyCode = virtualKeyCodeToKeyCode[kev.wVirtualKeyCode])
            {
                kev.wVirtualScanCode = keyCode >> 8;
                kev.uChar.UnicodeChar = keyCode & 0xFF;
            }
            // When running directly in a terminal, far2l does not set the
            // UnicodeChar field in Ctrl+Letter/Number events.
            if ( ((0x30 <= kev.wVirtualKeyCode && kev.wVirtualKeyCode <= 0x39) ||
                  (0x41 <= kev.wVirtualKeyCode && kev.wVirtualKeyCode <= 0x5A)) &&
                 kev.uChar.UnicodeChar == 0 )
            {
                kev.uChar.UnicodeChar = kev.wVirtualKeyCode;
            }

            if (getWin32Key(kev, ev, state))
            {
                normalizeKey(ev.keyDown);
                return Accepted;
            }
        }
        else if (out[outLen - 1] == 'M' && outLen - 1 == 16)
        {
            MOUSE_EVENT_RECORD mev {};
            memcpy(&mev.dwMousePosition.X, &out[0],  2);
            memcpy(&mev.dwMousePosition.Y, &out[2],  2);
            memcpy(&mev.dwButtonState,     &out[4],  4);
            memcpy(&mev.dwControlKeyState, &out[8],  4);
            memcpy(&mev.dwEventFlags,      &out[12], 4);

            getWin32Mouse(mev, ev, state);
            return Accepted;
        }
    }
    return Ignored;
}

static bool getWin32KeyText(const KEY_EVENT_RECORD &KeyEvent, TEvent &ev, InputState &state) noexcept
// Returns true unless the event contains a UTF-16 surrogate (Windows only),
// in which case we need the next event.
{
    uint32_t ch = KeyEvent.uChar.UnicodeChar;
    ev.keyDown.textLength = 0;

    // Do not treat non-printable characters as text.
    if (' ' <= ch && ch != 0x7F)
    {
#ifdef _WIN32
        if (0xD800 <= ch && ch <= 0xDBFF)
        {
            state.surrogate = ch;
            return false;
        }

        wchar_t utf16[2] = {(wchar_t) ch, 0};
        if (state.surrogate)
        {
            if (0xDC00 <= ch && ch <= 0xDFFF)
            {
                utf16[1] = (wchar_t) ch;
                utf16[0] = state.surrogate;
            }
            state.surrogate = 0;
        }

        ev.keyDown.textLength = WideCharToMultiByte(
            CP_UTF8, 0,
            utf16, utf16[1] ? 2 : 1,
            ev.keyDown.text, sizeof(ev.keyDown.text),
            nullptr, nullptr
        );
#else
        (void) state;

        if (ch < 0xD800 || (0xDFFF < ch && ch < 0x10FFFF))
            ev.keyDown.textLength = (uchar) utf32To8(ch, ev.keyDown.text);
#endif // _WIN32
    }
    return true;
}

bool TermIO::getWin32Key(const KEY_EVENT_RECORD &KeyEvent, TEvent &ev, InputState &state) noexcept
{
    if (!getWin32KeyText(KeyEvent, ev, state))
        return false;

    ev.what = evKeyDown;
    ev.keyDown.charScan.scanCode = KeyEvent.wVirtualScanCode;
    ev.keyDown.charScan.charCode = KeyEvent.uChar.AsciiChar;
    ev.keyDown.controlKeyState = KeyEvent.dwControlKeyState;

    if (ev.keyDown.textLength)
    {
        ev.keyDown.charScan.charCode = CpTranslator::fromUtf8(ev.keyDown.getText());
        if (KeyEvent.wVirtualKeyCode == VK_MENU)
            // This is enabled when pasting certain characters, and it confuses
            // applications. Clear it.
            ev.keyDown.charScan.scanCode = 0;
        if (!ev.keyDown.charScan.charCode || ev.keyDown.keyCode <= kbCtrlZ)
            // If the character cannot be represented in the current codepage,
            // or if it would accidentally trigger a Ctrl+Key combination,
            // make the whole keyCode zero to avoid side effects.
            ev.keyDown.keyCode = kbNoKey;
    }

    if ( ev.keyDown.keyCode == 0x2A00 || ev.keyDown.keyCode == 0x1D00 ||
         ev.keyDown.keyCode == 0x3600 || ev.keyDown.keyCode == 0x3800 ||
         ev.keyDown.keyCode == 0x3A00 )
        // Discard standalone Shift, Ctrl, Alt, Caps Lock keys.
        ev.keyDown.keyCode = kbNoKey;
    else if ( (ev.keyDown.controlKeyState & kbCtrlShift) &&
              (ev.keyDown.controlKeyState & kbAltShift) ) // Ctrl+Alt is AltGr.
    {
        // When AltGr+Key does not produce a character, a
        // keyCode with unwanted effects may be read instead.
        if (!ev.keyDown.textLength)
            ev.keyDown.keyCode = kbNoKey;
    }
    else if (KeyEvent.wVirtualScanCode < 89)
    {
        // Convert NT style virtual scan codes to PC BIOS codes.
        uchar index = KeyEvent.wVirtualScanCode;
        ushort keyCode = 0;
        if ((ev.keyDown.controlKeyState & kbAltShift) && THardwareInfo::AltCvt[index])
            keyCode = THardwareInfo::AltCvt[index];
        else if ((ev.keyDown.controlKeyState & kbCtrlShift) && THardwareInfo::CtrlCvt[index])
            keyCode = THardwareInfo::CtrlCvt[index];
        else if ((ev.keyDown.controlKeyState & kbShift) && THardwareInfo::ShiftCvt[index])
            keyCode = THardwareInfo::ShiftCvt[index];
        else if ( !(ev.keyDown.controlKeyState & (kbShift | kbCtrlShift | kbAltShift)) &&
                  THardwareInfo::NormalCvt[index] )
            keyCode = THardwareInfo::NormalCvt[index];

        if (keyCode != 0)
        {
            ev.keyDown.keyCode = keyCode;
            if (ev.keyDown.charScan.charCode < ' ')
                ev.keyDown.textLength = 0;
        }
    }

    return ev.keyDown.keyCode != kbNoKey || ev.keyDown.textLength;
}

void TermIO::getWin32Mouse(const MOUSE_EVENT_RECORD &MouseEvent, TEvent &ev, InputState &state) noexcept
{
    ev.what = evMouse;
    ev.mouse.where.x = MouseEvent.dwMousePosition.X;
    ev.mouse.where.y = MouseEvent.dwMousePosition.Y;
    ev.mouse.buttons = state.buttons = MouseEvent.dwButtonState;
    ev.mouse.eventFlags = MouseEvent.dwEventFlags;
    ev.mouse.controlKeyState = MouseEvent.dwControlKeyState;

    // Rotation sense is represented by the sign of dwButtonState's high word
    Boolean positive = !(MouseEvent.dwButtonState & 0x80000000);
    if( MouseEvent.dwEventFlags & MOUSE_WHEELED )
        ev.mouse.wheel = positive ? mwUp : mwDown;
    else if( MouseEvent.dwEventFlags & MOUSE_HWHEELED )
        ev.mouse.wheel = positive ? mwRight : mwLeft;
    else
        ev.mouse.wheel = 0;
}

} // namespace tvision

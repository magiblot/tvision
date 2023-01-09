#define Uses_TKeys
#include <tvision/tv.h>

#include <internal/terminal.h>
#include <internal/far2l.h>
#include <internal/stdioctl.h>
#include <internal/constmap.h>
#include <internal/constarr.h>
#include <internal/codepage.h>
#include <internal/getenv.h>
#include <internal/base64.h>
#include <internal/utf8.h>

namespace tvision
{

static const const_unordered_map<ushort, constarray<ushort, 3>> moddedKeyCodes =
{
    { 'A', {0, kbCtrlA, kbAltA}                }, { 'B', {0, kbCtrlB, kbAltB}                },
    { 'C', {0, kbCtrlC, kbAltC}                }, { 'D', {0, kbCtrlD, kbAltD}                },
    { 'E', {0, kbCtrlE, kbAltE}                }, { 'F', {0, kbCtrlF, kbAltF}                },
    { 'G', {0, kbCtrlG, kbAltG}                }, { 'H', {0, kbCtrlH, kbAltH}                },
    { 'I', {0, kbCtrlI, kbAltI}                }, { 'J', {0, kbCtrlJ, kbAltJ}                },
    { 'K', {0, kbCtrlK, kbAltK}                }, { 'L', {0, kbCtrlL, kbAltL}                },
    { 'M', {0, kbCtrlM, kbAltM}                }, { 'N', {0, kbCtrlN, kbAltN}                },
    { 'O', {0, kbCtrlO, kbAltO}                }, { 'P', {0, kbCtrlP, kbAltP}                },
    { 'Q', {0, kbCtrlQ, kbAltQ}                }, { 'R', {0, kbCtrlR, kbAltR}                },
    { 'S', {0, kbCtrlS, kbAltS}                }, { 'T', {0, kbCtrlT, kbAltT}                },
    { 'U', {0, kbCtrlU, kbAltU}                }, { 'V', {0, kbCtrlV, kbAltV}                },
    { 'W', {0, kbCtrlW, kbAltW}                }, { 'X', {0, kbCtrlX, kbAltX}                },
    { 'Y', {0, kbCtrlY, kbAltY}                }, { 'Z', {0, kbCtrlZ, kbAltZ}                },
    { '1', {0, 0, kbAlt1}                      }, { '2', {0, 0, kbAlt2}                      },
    { '3', {0, 0, kbAlt3}                      }, { '4', {0, 0, kbAlt4}                      },
    { '5', {0, 0, kbAlt5}                      }, { '6', {0, 0, kbAlt6}                      },
    { '7', {0, 0, kbAlt7}                      }, { '8', {0, 0, kbAlt8}                      },
    { '9', {0, 0, kbAlt9}                      }, { '0', {0, 0, kbAlt0}                      },
    { ' ', {0, 0, kbAltSpace}                  }, { '-', {0, 0, kbAltMinus}                  },
    { '=', {0, 0, kbAltEqual}                  },
    { kbF1, {kbShiftF1, kbCtrlF1, kbAltF1}     }, { kbF2, {kbShiftF2, kbCtrlF2, kbAltF2}     },
    { kbF3, {kbShiftF3, kbCtrlF3, kbAltF3}     }, { kbF4, {kbShiftF4, kbCtrlF4, kbAltF4}     },
    { kbF5, {kbShiftF5, kbCtrlF5, kbAltF5}     }, { kbF6, {kbShiftF6, kbCtrlF6, kbAltF6}     },
    { kbF7, {kbShiftF7, kbCtrlF7, kbAltF7}     }, { kbF8, {kbShiftF8, kbCtrlF8, kbAltF8}     },
    { kbF9, {kbShiftF9, kbCtrlF9, kbAltF9}     }, { kbF10, {kbShiftF10, kbCtrlF10, kbAltF10} },
    { kbF11, {kbShiftF11, kbCtrlF11, kbAltF11} }, { kbF12, {kbShiftF12, kbCtrlF12, kbAltF12} },
    { kbEsc, {0, 0, kbAltEsc}                  }, { kbBack, {0, kbCtrlBack, kbAltBack}       },
    { kbTab, {kbShiftTab, kbCtrlTab, kbAltTab} }, { kbEnter, {0, kbCtrlEnter, kbAltEnter}    },
    { kbHome, {0, kbCtrlHome, kbAltHome}       }, { kbUp, {0, kbCtrlUp, kbAltUp}             },
    { kbPgUp, {0, kbCtrlPgUp, kbAltPgUp}       }, { kbLeft, {0, kbCtrlLeft, kbAltLeft}       },
    { kbRight, {0, kbCtrlRight, kbAltRight}    }, { kbEnd, {0, kbCtrlEnd, kbAltEnd}          },
    { kbDown, {0, kbCtrlDown, kbAltDown}       }, { kbPgDn, {0, kbCtrlPgDn, kbAltPgDn}       },
    { kbIns, {kbShiftIns, kbCtrlIns, kbAltIns} }, { kbDel, {kbShiftDel, kbCtrlDel, kbAltDel} },
};

const uint XTermModDefault = 1;

static KeyDownEvent keyWithXTermMods(ushort keyCode, uint mods) noexcept
{
    mods -= XTermModDefault;
    ushort tvmods =
          (kbShift & -(mods & 1))
        | (kbAltShift & -(mods & 2))
        | (kbCtrlShift & -(mods & 4))
        ;
    KeyDownEvent keyDown {{keyCode}, tvmods};
    TermIO::normalizeKey(keyDown);
    return keyDown;
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
    if (isAlpha(keyDown.keyCode))
    {
        keyDown.text[0] = keyDown.keyCode;
        keyDown.textLength = 1;
    }
    return true;
}

// The default mouse experience with Ncurses is not always good. To work around
// some issues, we request and parse mouse events manually.

void TermIO::mouseOn(StdioCtl &io) noexcept
{
    TStringView seq = "\x1B[?1001s" // Save old highlight mouse reporting.
                      "\x1B[?1000h" // Enable mouse reporting.
                      "\x1B[?1002h" // Enable mouse drag reporting.
                      "\x1B[?1006h" // Enable SGR extended mouse reporting.
                    ;
    io.write(seq.data(), seq.size());
}

void TermIO::mouseOff(StdioCtl &io) noexcept
{
    TStringView seq = "\x1B[?1006l" // Disable SGR extended mouse reporting.
                      "\x1B[?1002l" // Disable mouse drag reporting.
                      "\x1B[?1000l" // Disable mouse reporting.
                      "\x1B[?1001r" // Restore old highlight mouse reporting.
                    ;
    io.write(seq.data(), seq.size());
}

void TermIO::keyModsOn(StdioCtl &io) noexcept
{
    // https://invisible-island.net/xterm/ctlseqs/ctlseqs.html
    // https://sw.kovidgoyal.net/kitty/keyboard-protocol.html
    TStringView seq = "\x1B[?1036s" // Save metaSendsEscape (XTerm).
                      "\x1B[?1036h" // Enable metaSendsEscape (XTerm).
                      "\x1B[?2004s" // Save bracketed paste.
                      "\x1B[?2004h" // Enable bracketed paste.
                      "\x1B[>4;1m"  // Enable modifyOtherKeys (XTerm).
                      "\x1B[>1u"    // Disambiguate escape codes (Kitty).
                      far2lEnableSeq
                    ;
    io.write(seq.data(), seq.size());
    if (char *term = getenv("TERM"))
    {
        // Check for full OSC 52 clipboard support.
        if (strstr(term, "alacritty") || strstr(term, "foot"))
            // Request clipboard contents to see if they are readable. It is
            // not safe to print this blindly so only do it for TERMs which
            // we know should work.
            seq = "\x1B]52;;?\x07";
        else
            seq =
                // Check for the 'kitty-query-clipboard_control' capability (XTGETTCAP).
                "\x1BP+q6b697474792d71756572792d636c6970626f6172645f636f6e74726f6c\x1B\\"
                // Check for 'allowWindowOps' (XTQALLOWED).
                "\x1B]60\x1B\\"
                ;
        io.write(seq.data(), seq.size());
    }
}

void TermIO::keyModsOff(StdioCtl &io, EventSource &source, InputState &state) noexcept
{
    TStringView seq = far2lPingSeq
                      far2lDisableSeq
                      "\x1B[<u"     // Restore previous keyboard mode (Kitty).
                      "\x1B[>4m"    // Reset modifyOtherKeys (XTerm).
                      "\x1B[?2004l" // Disable bracketed paste.
                      "\x1B[?2004r" // Restore bracketed paste.
                      "\x1B[?1036r" // Restore metaSendsEscape (XTerm).
                    ;
    io.write(seq.data(), seq.size());
    // If we are running across a slow connection, it is highly likely that
    // far2l will send us keyUp or mouse events before extensions get disabled.
    // Therefore, discard events until we get a ping response.
    waitFar2lPing(source, state);
}

void TermIO::normalizeKey(KeyDownEvent &keyDown) noexcept
{
    TKey key(keyDown);
    if (key.mods & (kbShift | kbCtrlShift | kbAltShift))
    {
        // Modifier precedece: Shift < Ctrl < Alt.
        int largestMod = (key.mods & kbAltShift) ? 2
                       : (key.mods & kbCtrlShift) ? 1
                       : 0;
        if (ushort keyCode = moddedKeyCodes[key.code][largestMod])
        {
            keyDown.keyCode = keyCode;
            if (keyDown.charScan.charCode < ' ')
                keyDown.textLength = 0;
        }
    }
    // TKey does not distinguish left/right modifiers, so preserve those
    // when available.
    ushort origMods = keyDown.controlKeyState;
    keyDown.controlKeyState =
        ((origMods | key.mods) & ~(kbCtrlShift | kbAltShift))
      | ((origMods & kbCtrlShift ? origMods : key.mods) & kbCtrlShift)
      | ((origMods & kbAltShift ? origMods : key.mods) & kbAltShift)
        ;
}

ParseResult TermIO::parseEvent(GetChBuf &buf, TEvent &ev, InputState &state) noexcept
{
    if (buf.get() == '\x1B')
        return parseEscapeSeq(buf, ev, state);
    return Rejected;
}

ParseResult TermIO::parseEscapeSeq(GetChBuf &buf, TEvent &ev, InputState &state) noexcept
// Pre: "\x1B" has just been read.
{
    ParseResult res = Rejected;
    switch (buf.get())
    {
        case '_':
            if (buf.readStr("f2l"))
                return parseFar2lInput(buf, ev, state);
            if (buf.readStr("far2l"))
                return parseFar2lAnswer(buf, ev, state);
            break;
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
                            return parseFixTermKey(csi, ev);
                        else
                            return parseCSIKey(csi, ev, state);
                    }
                    break;
                }
            }
            break;
        case 'O':
            return parseSS3Key(buf, ev);
        case 'P':
            return parseDCS(buf, state);
        case ']':
            return parseOSC(buf, state);
        case '\x1B':
            res = parseEscapeSeq(buf, ev, state);
            if (res == Accepted && ev.what == evKeyDown)
            {
                ev.keyDown.controlKeyState |= kbAltShift;
                normalizeKey(ev.keyDown);
            }
            break;
    }
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

ParseResult TermIO::parseCSIKey(const CSIData &csi, TEvent &ev, InputState &state) noexcept
// https://invisible-island.net/xterm/xterm-function-keys.html
// https://invisible-island.net/xterm/ctlseqs/ctlseqs.html
{
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
            case 200: state.bracketedPaste = true; return Ignored;
            case 201: state.bracketedPaste = false; return Ignored;
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

ParseResult TermIO::parseDCS(GetChBuf &buf, InputState &state) noexcept
// Pre: '\x1BP' has just been read.
{
    if (char *s = readUntilBelOrSt(buf))
    {
        // We only get a DCS in response to our request for kitty capabilities.
        if (strstr(s, "726561642d636c6970626f617264")) // 'read-clipboard'
            state.hasFullOsc52 = true;
        free(s);
    }
    return Ignored;
}

ParseResult TermIO::parseOSC(GetChBuf &buf, InputState &state) noexcept
// Pre: '\x1B]' has just been read.
{
    if (char *s = readUntilBelOrSt(buf))
    {
        TStringView sv(s);
        if (sv.size() > 3 && sv.substr(0, 3) == "52;") // OSC 52
        {
            if (char *begin = (char *) memchr(&sv[3], ';', sv.size() - 3))
            {
                if (!state.hasFullOsc52)
                    // We got a response to our initial request.
                    state.hasFullOsc52 = true;
                else if (state.putPaste)
                {
                    TStringView encoded = sv.substr(begin + 1 - &sv[0]);
                    if (char *pDecoded = (char *) malloc((encoded.size() * 3)/4 + 3))
                    {
                        TStringView decoded = decodeBase64(encoded, pDecoded);
                        state.putPaste(decoded);
                        free(pDecoded);
                    }
                }
            }
        }
        else if (sv.size() > 3 && sv.substr(0, 3) == "60;") // OSC 60
            if (strstr(&sv[3], "allowWindowOps"))
                state.hasFullOsc52 = true;
        free(s);
    }
    return Ignored;
}

static bool setOsc52Clipboard(StdioCtl &io, TStringView text, InputState &state) noexcept
{
    TStringView prefix = "\x1B]52;;";
    TStringView suffix = "\x07";
    if (char *buf = (char *) malloc(prefix.size() + suffix.size() + (text.size() * 4)/3 + 4))
    {
        memcpy(buf, prefix.data(), prefix.size());
        TStringView b64 = encodeBase64(text, buf + prefix.size());
        memcpy(buf + prefix.size() + b64.size(), suffix.data(), suffix.size());
        io.write(buf, prefix.size() + b64.size() + suffix.size());
        free(buf);
    }
    // Return false when there is no full OSC 52 support, even though we always
    // make the request. This way, we can still use the internal clipboard.
    return state.hasFullOsc52;
}

static bool requestOsc52Clipboard(StdioCtl &io, InputState &state) noexcept
{
    if (state.hasFullOsc52)
    {
        TStringView seq = "\x1B]52;;?\x07";
        io.write(seq.data(), seq.size());
        return true;
    }
    return false;
}

bool TermIO::setClipboardText(StdioCtl &io, TStringView text, InputState &state) noexcept
{
    return setFar2lClipboard(io, text, state)
        || setOsc52Clipboard(io, text, state);
}

bool TermIO::requestClipboardText(StdioCtl &io, void (&accept)(TStringView), InputState &state) noexcept
{
    state.putPaste = &accept;
    return requestFar2lClipboard(io, state)
        || requestOsc52Clipboard(io, state);
}

char *TermIO::readUntilBelOrSt(GetChBuf &buf) noexcept
// Returns a malloc-allocated and null-terminated string, or null.
{
    size_t capacity = 1024;
    size_t len = 0;
    char prev = '\0';
    if (char *s = (char *) malloc(capacity))
    {
        char c;
        while (c = buf.getUnbuffered(), c != -1)
        {
            if (c == '\x07') // BEL
                break;
            if (c == '\\' && prev == '\x1B') // ST
            {
                len -= (len > 0);
                break;
            }
            if (capacity == len + 1)
            {
                if (void *tmp = realloc(s, capacity *= 2))
                    s = (char *) tmp;
                else
                    s = (free(s), nullptr);
            }
            if (s)
                s[len++] = c;
            prev = c;
        }
        if (s)
            s[len] = '\0';
        return s;
    }
    return {};
}

} // namespace tvision

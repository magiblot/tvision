#define Uses_TKeys
#include <tvision/tv.h>

#include <internal/far2l.h>
#include <internal/win32con.h>
#include <internal/constmap.h>
#include <internal/base64.h>

namespace tvision
{

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

ParseResult parseFar2lInput(GetChBuf &buf, TEvent &ev, InputState &state) noexcept
// Pre: "\x1B_f2l" has just been read.
{
    enum { k = 32 };
    char s[4*k];
    size_t len = 0;
    char c;
    while (c = buf.getUnbuffered(), c != -1 && c != '\x07')
        if (len < sizeof(s))
            s[len++] = c;
    char o[3*k];
    TStringView out = decodeBase64({s, len}, o);
    if (!out.empty())
    {
        if (out.back() == 'K' && out.size() - 1 == 14)
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
                TermIO::normalizeKey(ev.keyDown);
                return Accepted;
            }
        }
        else if (out.back() == 'M' && out.size() - 1 == 16)
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

} // namespace tvision

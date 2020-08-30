#define Uses_TKeys
#define Uses_TEvent
#include <tvision/tv.h>

#include <internal/codepage.h>
#include <internal/getenv.h>
#include <internal/utf8.h>
#include <array>
using std::unordered_map;
using std::string_view;
using std::array;

/* The Turbo Vision library has all its characters encoded in code page 437.
 * While Unicode support is not added, it's better to just translate them
 * with a lookup table. The following table allows translating the characters
 * stored by Turbo Vision into the corresponding UTF-8 mulibyte characters.
 * Taken from https://en.wikipedia.org/wiki/Code_page_437 */

static constexpr std::string_view cp437toUtf8[256] = {
    "\0", "☺", "☻", "♥", "♦", "♣", "♠", "•", "◘", "○", "◙", "♂", "♀", "♪", "♫", "☼",
    "►", "◄", "↕", "‼", "¶", "§", "▬", "↨", "↑", "↓", "→", "←", "∟", "↔", "▲", "▼",
    " ", "!", "\"", "#", "$", "%", "&", "'", "(", ")", "*", "+", ",", "-", ".", "/",
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "<", "=", ">", "?",
    "@", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "[", "\\", "]", "^", "_",
    "`", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o",
    "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "{", "|", "}", "~", "⌂",
    "Ç", "ü", "é", "â", "ä", "à", "å", "ç", "ê", "ë", "è", "ï", "î", "ì", "Ä", "Å",
    "É", "æ", "Æ", "ô", "ö", "ò", "û", "ù", "ÿ", "Ö", "Ü", "¢", "£", "¥", "₧", "ƒ",
    "á", "í", "ó", "ú", "ñ", "Ñ", "ª", "º", "¿", "⌐", "¬", "½", "¼", "¡", "«", "»",
    "░", "▒", "▓", "│", "┤", "╡", "╢", "╖", "╕", "╣", "║", "╗", "╝", "╜", "╛", "┐",
    "└", "┴", "┬", "├", "─", "┼", "╞", "╟", "╚", "╔", "╩", "╦", "╠", "═", "╬", "╧",
    "╨", "╤", "╥", "╙", "╘", "╒", "╓", "╫", "╪", "┘", "┌", "█", "▄", "▌", "▐", "▀",
    "α", "ß", "Γ", "π", "Σ", "σ", "µ", "τ", "Φ", "Θ", "Ω", "δ", "∞", "φ", "ε", "∩",
    "≡", "±", "≥", "≤", "⌠", "⌡", "÷", "≈", "°", "∙", "·", "√", "ⁿ", "²", "■", " "
};

static constexpr std::array<uint32_t, 256> cp437toUtf8Int = make_utf8int<256>(cp437toUtf8);

static constexpr std::string_view cp850toUtf8[256] = {
    "\0", "☺", "☻", "♥", "♦", "♣", "♠", "•", "◘", "○", "◙", "♂", "♀", "♪", "♫", "☼",
    "►", "◄", "↕", "‼", "¶", "§", "▬", "↨", "↑", "↓", "→", "←", "∟", "↔", "▲", "▼",
    " ", "!", "\"", "#", "$", "%", "&", "'", "(", ")", "*", "+", ",", "-", ".", "/",
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "<", "=", ">", "?",
    "@", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "[", "\\", "]", "^", "_",
    "`", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o",
    "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "{", "|", "}", "~", "⌂",
    "Ç", "ü", "é", "â", "ä", "à", "å", "ç", "ê", "ë", "è", "ï", "î", "ì", "Ä", "Å",
    "É", "æ", "Æ", "ô", "ö", "ò", "û", "ù", "ÿ", "Ö", "Ü", "ø", "£", "Ø", "×", "ƒ",
    "á", "í", "ó", "ú", "ñ", "Ñ", "ª", "º", "¿", "®", "¬", "½", "¼", "¡", "«", "»",
    "░", "▒", "▓", "│", "┤", "Á", "Â", "À", "©", "╣", "║", "╗", "╝", "¢", "¥", "┐",
    "└", "┴", "┬", "├", "─", "┼", "ã", "Ã", "╚", "╔", "╩", "╦", "╠", "═", "╬", "¤",
    "ð", "Ð", "Ê", "Ë", "È", "ı", "Í", "Î", "Ï", "┘", "┌", "█", "▄", "¦", "Ì", "▀",
    "Ó", "ß", "Ô", "Ò", "õ", "Õ", "µ", "þ", "Þ", "Ú", "Û", "Ù", "ý", "Ý", "¯", "´",
    "-", "±", "‗", "¾", "¶", "§", "÷", "¸", "°", "¨", "·", "¹", "³", "²", "■", " "
    // Note that <last row, first column> should be soft hyphen ("\u00AD"), but
    // it is often represented as a regular hyphen.
};

static constexpr std::array<uint32_t, 256> cp850toUtf8Int = make_utf8int<256>(cp850toUtf8);

const CpTranslator::CpTable CpTranslator::tables[] = {
    { "437", cp437toUtf8, cp437toUtf8Int },
    { "850", cp850toUtf8, cp850toUtf8Int }
};

const CpTranslator::CpTable *CpTranslator::activeTable;
CpTranslator CpTranslator::instance;

CpTranslator::CpTranslator() {
    // Set the active codepage. 437 is the default.
    use(getEnv<std::string_view>("TVISION_CODEPAGE", "437"));
}

#ifdef HAVE_NCURSES

#include <internal/ncursinp.h>
#include <ncurses.h>

/* Turbo Vision is designed to work with BIOS key codes. Mnemonics for some
 * key codes are defined in tkeys.h. Until this is not changed, it is
 * necessary to translate ncurses keys to key codes. */

/* Turbo Vision stores key events in a KeyDownEvent struct, defined in
 * system.h. Its first field is a key code (16 bit), which can be decomposed
 * into the ASCII equivalent (lower byte) and a scan code (higher byte).
 * It has a second field with the state of the modifier keys, which can be
 * retrieved by performing a bit-wise AND with the kbShift, kbCtrlShift and
 * kbAltShift bitmasks. Turbo Vision expects this field to be filled even
 * if the key code is already named Shift/Ctrl/Alt+something. */

/* The support for key combinations is the following:
   - PrintScreen, Break are not likely to be captured by the terminal, but
     Ctrl+C could be used as a replacement of the Ctrl+Break interrupt.
   - Ctrl/Alt+F(n) don't work on the linux console and I strongly advice against
     using them.
   - Ctrl+Letter works, except for ^H, ^I, ^J and ^M, which have a special
     meaning.
   - Alt+Letter/Number seem to work quite well.
   - Ctrl+Backspace/Enter can't be recognized on terminal emulators.
   - Shift/Ctrl+Ins/Del/Home/End/PgDn/PgUp seem to work, too.
   - Arrow keys work, as well as combined with Shift, but Turbo Vision doesn't
     support Ctrl+Up/Down (EDIT: it now does).
   - Tab and Backtab are supported too, although the linux console confuses the
     latter with Alt+Tab.
   - Some other key combinations are supported on terminal but not in Turbo Vision.
 * Still, it's up to your luck that ncurses manages to grab any of these
 * combinations from your terminal application. */

unordered_map<char, KeyDownEvent> fromNonPrintableAscii = {
    { '\x01',       {{kbCtrlA},     kbCtrlShift}},
    { '\x02',       {{kbCtrlB},     kbCtrlShift}},
    { '\x03',       {{kbCtrlC},     kbCtrlShift}},
    { '\x04',       {{kbCtrlD},     kbCtrlShift}},
    { '\x05',       {{kbCtrlE},     kbCtrlShift}},
    { '\x06',       {{kbCtrlF},     kbCtrlShift}},
    { '\x07',       {{kbCtrlG},     kbCtrlShift}},
    { '\x08',       {{kbBack},      0}          }, // ^H, Backspace
    { '\x09',       {{kbTab},       0}          }, // ^I, Tab
    { '\x0A',       {{kbEnter},     0}          }, // ^J, Line Feed
    { '\x0B',       {{kbCtrlK},     kbCtrlShift}},
    { '\x0C',       {{kbCtrlL},     kbCtrlShift}},
    { '\x0D',       {{kbEnter},     0}          }, // ^M, Carriage Return
    { '\x0E',       {{kbCtrlN},     kbCtrlShift}},
    { '\x0F',       {{kbCtrlO},     kbCtrlShift}},
    { '\x10',       {{kbCtrlP},     kbCtrlShift}},
    { '\x11',       {{kbCtrlQ},     kbCtrlShift}},
    { '\x12',       {{kbCtrlR},     kbCtrlShift}},
    { '\x13',       {{kbCtrlS},     kbCtrlShift}},
    { '\x14',       {{kbCtrlT},     kbCtrlShift}},
    { '\x15',       {{kbCtrlU},     kbCtrlShift}},
    { '\x16',       {{kbCtrlV},     kbCtrlShift}},
    { '\x17',       {{kbCtrlW},     kbCtrlShift}},
    { '\x18',       {{kbCtrlX},     kbCtrlShift}},
    { '\x19',       {{kbCtrlY},     kbCtrlShift}},
    { '\x1A',       {{kbCtrlZ},     kbCtrlShift}},
    { '\x1B',       {{kbEsc},       0}          }, // ^[, Escape
    { '\x1C',       {{0x1C},        0}          }, // ^\, ?
    { '\x1D',       {{0x1D},        0}          }, // ^], ?
    { '\x1E',       {{0x1E},        0}          }, // ^^, ?
    { '\x1F',       {{0x1F},        0}          }, // ^_, ?
    { '\x7F',       {{kbBack},      0}          }  // ^?, Delete
};

unordered_map<char, ushort> AltKeyCode = {
    { ' ', kbAltSpace },
    { 'Q', kbAltQ }, { 'W', kbAltW }, { 'E', kbAltE }, { 'R', kbAltR },
    { 'T', kbAltT }, { 'Y', kbAltY }, { 'U', kbAltU }, { 'I', kbAltI },
    { 'O', kbAltO }, { 'P', kbAltP }, { 'A', kbAltA }, { 'S', kbAltS },
    { 'D', kbAltD }, { 'F', kbAltF }, { 'G', kbAltG }, { 'H', kbAltH },
    { 'J', kbAltJ }, { 'K', kbAltK }, { 'L', kbAltL }, { 'Z', kbAltZ },
    { 'X', kbAltX }, { 'C', kbAltC }, { 'V', kbAltV }, { 'B', kbAltB },
    { 'N', kbAltN }, { 'M', kbAltM }, { '1', kbAlt1 }, { '2', kbAlt2 },
    { '3', kbAlt3 }, { '4', kbAlt4 }, { '5', kbAlt5 }, { '6', kbAlt6 },
    { '7', kbAlt7 }, { '8', kbAlt8 }, { '9', kbAlt9 }, { '0', kbAlt0 },
    { '-', kbAltMinus }, { '=', kbAltEqual }, { '\x08', kbAltBack }
};

unordered_map<int, KeyDownEvent> NcursesInput::fromCursesKeyCode = {
    { KEY_DOWN,         {{kbDown},      0}          },
    { KEY_UP,           {{kbUp},        0}          },
    { KEY_LEFT,         {{kbLeft},      0}          },
    { KEY_RIGHT,        {{kbRight},     0}          },
    { KEY_HOME,         {{kbHome},      0}          },
    { KEY_BACKSPACE,    {{kbBack},      0}          },
    { KEY_DC,           {{kbDel},       0}          },
    { KEY_IC,           {{kbIns},       0}          },
    { KEY_SF,           {{kbDown},      kbShift}    },
    { KEY_SR,           {{kbUp},        kbShift}    },
    { KEY_NPAGE,        {{kbPgDn},      0}          },
    { KEY_PPAGE,        {{kbPgUp},      0}          },
    { KEY_ENTER,        {{kbEnter},     0}          },
    { KEY_BTAB,         {{kbShiftTab},  kbShift}    },
    { KEY_END,          {{kbEnd},       0}          },
    { KEY_SDC,          {{kbShiftDel},  kbShift}    },
    { KEY_SEND,         {{kbEnd},       kbShift}    },
    { KEY_SHOME,        {{kbHome},      kbShift}    },
    { KEY_SIC,          {{kbShiftIns},  kbShift}    },
    { KEY_SLEFT,        {{kbLeft},      kbShift}    },
    { KEY_SRIGHT,       {{kbRight},     kbShift}    },
    { KEY_SUSPEND,      {{kbCtrlZ},     kbCtrlShift}},
    // Function keys F1-F12
    { KEY_F0 + 1,       {{kbF1},        0}          },
    { KEY_F0 + 2,       {{kbF2},        0}          },
    { KEY_F0 + 3,       {{kbF3},        0}          },
    { KEY_F0 + 4,       {{kbF4},        0}          },
    { KEY_F0 + 5,       {{kbF5},        0}          },
    { KEY_F0 + 6,       {{kbF6},        0}          },
    { KEY_F0 + 7,       {{kbF7},        0}          },
    { KEY_F0 + 8,       {{kbF8},        0}          },
    { KEY_F0 + 9,       {{kbF9},        0}          },
    { KEY_F0 + 10,      {{kbF10},       0}          },
    { KEY_F0 + 11,      {{kbF11},       0}          },
    { KEY_F0 + 12,      {{kbF12},       0}          },
    // Shift+F1-F12
    { KEY_F0 + 13,      {{kbShiftF1},   kbShift}    },
    { KEY_F0 + 14,      {{kbShiftF2},   kbShift}    },
    { KEY_F0 + 15,      {{kbShiftF3},   kbShift}    },
    { KEY_F0 + 16,      {{kbShiftF4},   kbShift}    },
    { KEY_F0 + 17,      {{kbShiftF5},   kbShift}    },
    { KEY_F0 + 18,      {{kbShiftF6},   kbShift}    },
    { KEY_F0 + 19,      {{kbShiftF7},   kbShift}    },
    { KEY_F0 + 20,      {{kbShiftF8},   kbShift}    },
    { KEY_F0 + 21,      {{kbShiftF9},   kbShift}    },
    { KEY_F0 + 22,      {{kbShiftF10},  kbShift}    },
    { KEY_F0 + 23,      {{kbShiftF11},  kbShift}    },
    { KEY_F0 + 24,      {{kbShiftF12},  kbShift}    },
    /* Linux console support for function keys ends here, so please
     * avoid using any of the following: */
    // Ctrl+F1-F12
    { KEY_F0 + 25,      {{kbCtrlF1},    kbCtrlShift}},
    { KEY_F0 + 26,      {{kbCtrlF2},    kbCtrlShift}},
    { KEY_F0 + 27,      {{kbCtrlF3},    kbCtrlShift}},
    { KEY_F0 + 28,      {{kbCtrlF4},    kbCtrlShift}},
    { KEY_F0 + 29,      {{kbCtrlF5},    kbCtrlShift}},
    { KEY_F0 + 30,      {{kbCtrlF6},    kbCtrlShift}},
    { KEY_F0 + 31,      {{kbCtrlF7},    kbCtrlShift}},
    { KEY_F0 + 32,      {{kbCtrlF8},    kbCtrlShift}},
    { KEY_F0 + 33,      {{kbCtrlF9},    kbCtrlShift}},
    { KEY_F0 + 34,      {{kbCtrlF10},   kbCtrlShift}},
    { KEY_F0 + 35,      {{kbCtrlF11},   kbCtrlShift}},
    { KEY_F0 + 36,      {{kbCtrlF12},   kbCtrlShift}},
    // Ctrl+Shift+F(n) supported by ncurses but not Turbo Vision
    // Alt+F1-F12
    { KEY_F0 + 49,      {{kbAltF1},     kbAltShift} },
    { KEY_F0 + 50,      {{kbAltF2},     kbAltShift} },
    { KEY_F0 + 51,      {{kbAltF3},     kbAltShift} },
    { KEY_F0 + 52,      {{kbAltF4},     kbAltShift} },
    { KEY_F0 + 53,      {{kbAltF5},     kbAltShift} },
    { KEY_F0 + 54,      {{kbAltF6},     kbAltShift} },
    { KEY_F0 + 55,      {{kbAltF7},     kbAltShift} },
    { KEY_F0 + 56,      {{kbAltF8},     kbAltShift} },
    { KEY_F0 + 57,      {{kbAltF9},     kbAltShift} },
    { KEY_F0 + 58,      {{kbAltF10},    kbAltShift} },
    { KEY_F0 + 59,      {{kbAltF11},    kbAltShift} },
    { KEY_F0 + 60,      {{kbAltF12},    kbAltShift} }
};

unordered_map<string_view, KeyDownEvent> NcursesInput::fromCursesHighKey = {
    /* These keys are identified by name. The int value is not known
     * at compilation time. */
    { "kDC3",       {{kbAltDel},        kbAltShift}},
    { "kEND3",      {{kbAltEnd},        kbAltShift}},
    { "kHOM3",      {{kbAltHome},       kbAltShift}},
    { "kIC3",       {{kbAltIns},        kbAltShift}},
    { "kLFT3",      {{kbAltLeft},       kbAltShift}},
    { "kNXT3",      {{kbAltPgDn},       kbAltShift}},
    { "kPRV3",      {{kbAltPgUp},       kbAltShift}},
    { "kRIT3",      {{kbAltRight},      kbAltShift}},
    { "kUP3",       {{kbAltUp},         kbAltShift}},
    { "kDN3",       {{kbAltDown},       kbAltShift}},
    { "kDC4",       {{kbShiftDel},      kbShift | kbAltShift}},
    { "kEND4",      {{kbEnd},           kbShift | kbAltShift}},
    { "kHOM4",      {{kbHome},          kbShift | kbAltShift}},
    { "kIC4",       {{kbShiftIns},      kbShift | kbAltShift}},
    { "kLFT4",      {{kbLeft},          kbShift | kbAltShift}},
    { "kNXT4",      {{kbPgDn},          kbShift | kbAltShift}},
    { "kPRV4",      {{kbPgUp},          kbShift | kbAltShift}},
    { "kRIT4",      {{kbRight},         kbShift | kbAltShift}},
    { "kUP4",       {{kbUp},            kbShift | kbAltShift}},
    { "kDN4",       {{kbDown},          kbShift | kbAltShift}},
    { "kDC5",       {{kbCtrlDel},       kbCtrlShift}},
    { "kEND5",      {{kbCtrlEnd},       kbCtrlShift}},
    { "kHOM5",      {{kbCtrlHome},      kbCtrlShift}},
    { "kIC5",       {{kbCtrlIns},       kbCtrlShift}},
    { "kLFT5",      {{kbCtrlLeft},      kbCtrlShift}},
    { "kNXT5",      {{kbCtrlPgDn},      kbCtrlShift}},
    { "kPRV5",      {{kbCtrlPgUp},      kbCtrlShift}},
    { "kRIT5",      {{kbCtrlRight},     kbCtrlShift}},
    { "kUP5",       {{kbCtrlUp},        kbCtrlShift}},
    { "kDN5",       {{kbCtrlDown},      kbCtrlShift}},
    { "kDC6",       {{kbCtrlDel},       kbCtrlShift | kbShift}},
    { "kEND6",      {{kbCtrlEnd},       kbCtrlShift | kbShift}},
    { "kHOM6",      {{kbCtrlHome},      kbCtrlShift | kbShift}},
    { "kIC6",       {{kbCtrlIns},       kbCtrlShift | kbShift}},
    { "kLFT6",      {{kbCtrlLeft},      kbCtrlShift | kbShift}},
    { "kNXT6",      {{kbCtrlPgDn},      kbCtrlShift | kbShift}},
    { "kPRV6",      {{kbCtrlPgUp},      kbCtrlShift | kbShift}},
    { "kRIT6",      {{kbCtrlRight},     kbCtrlShift | kbShift}},
    { "kUP6",       {{kbCtrlUp},        kbCtrlShift | kbShift}},
    { "kDN6",       {{kbCtrlDown},      kbCtrlShift | kbShift}},
    { "kDC7",       {{kbDel},           kbCtrlShift | kbAltShift}}, // Please do not attempt this one
    { "kEND7",      {{kbEnd},           kbCtrlShift | kbAltShift}},
    { "kHOM7",      {{kbHome},          kbCtrlShift | kbAltShift}},
    { "kIC7",       {{kbIns},           kbCtrlShift | kbAltShift}},
    { "kLFT7",      {{kbLeft},          kbCtrlShift | kbAltShift}},
    { "kNXT7",      {{kbPgDn},          kbCtrlShift | kbAltShift}},
    { "kPRV7",      {{kbPgUp},          kbCtrlShift | kbAltShift}},
    { "kRIT7",      {{kbRight},         kbCtrlShift | kbAltShift}},
    { "kUP7",       {{kbUp},            kbCtrlShift | kbAltShift}},
    { "kDN7",       {{kbDown},          kbCtrlShift | kbAltShift}},
};

unordered_map<ulong, unordered_map<ushort, ushort>> keyCodeWithModifiers = {
    /* There are cases, such as the linux console, where it is possible to
     * get the state of keyboard modifiers (Shift/Ctrl/Alt), but captured
     * key events don't include that information. So, an extra translation
     * step must be done to get the actual Turbo Vision key codes. */
    { kbShift, {
        { kbTab,        kbShiftTab      },
        { kbDel,        kbShiftDel      },
        { kbIns,        kbShiftIns      }
    }},
    { kbCtrlShift, {
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
    }},
    { kbAltShift, {
        { kbDel,        kbAltDel        },
        { kbEnd,        kbAltEnd        },
        { kbHome,       kbAltHome       },
        { kbIns,        kbAltIns        },
        { kbPgDn,       kbAltPgDn       },
        { kbPgUp,       kbAltPgUp       },
        { kbUp,         kbAltUp         },
        { kbDown,       kbAltDown       },
    }},
};

#endif // HAVE_NCURSES

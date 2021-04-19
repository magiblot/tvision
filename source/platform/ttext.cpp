#define Uses_TText
#include <tvision/tv.h>

#include <internal/codepage.h>
#include <internal/platform.h>
#include <internal/linuxcon.h>
#include <internal/win32con.h>
#include <internal/winwidth.h>
#include <wchar.h>

// Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

enum { UTF8_ACCEPT = 0, UTF8_REJECT = 12 };

static const uint8_t utf8d[] =
{
    // The first part of the table maps bytes to character classes that
    // to reduce the size of the transition table and create bitmasks.
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
     8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

    // The second part is a transition table that maps a combination
    // of a state of the automaton and a character class to a state.
     0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
    12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
    12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
    12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
    12,36,12,12,12,12,12,12,12,12,12,12,
};

static inline
uint32_t decode_utf8(uint32_t* state, uint32_t* codep, uint8_t byte)
{
    uint32_t type = utf8d[byte];

    *codep = (*state != UTF8_ACCEPT) ?
        (byte & 0x3F) | (*codep << 6) :
        (0xFF >> type) & (byte);

    *state = utf8d[256 + *state + type];
    return *state;
}

namespace ttext
{

    static int mbtowc(char32_t &wc, TStringView text)
    // Returns n >= 1 if 'text' begins with a UTF-8 multibyte character that's
    // 'n' bytes long, -1 otherwise.
    {
        uint32_t state = 0;
        uint32_t codep = 0;
        for (size_t i = 0; i < text.size(); ++i)
        {
            switch (decode_utf8(&state, &codep, text[i]))
            {
                case UTF8_ACCEPT:
                    return (wc = codep), i + 1;
                case UTF8_REJECT:
                    return -1;
                default:
                    break;
            }
        }
        return -1;
    }

}

int TText::mblen(TStringView text)
{
    using namespace ttext;
    char32_t wc;
    return mbtowc(wc, text);
}

TText::mbstat_r TText::mbstat(TStringView text)
{
    using namespace ttext;
    char32_t wc;
    int length = mbtowc(wc, text);
    int width = -1;
    if (length > 1)
        width = PlatformStrategy::instance->charWidth({&text[0], (size_t) length}, wc);
    return {length, width};
}

#ifdef _TV_UNIX
int UnixPlatformStrategy::charWidth(TStringView, char32_t wc)
{
    return wcwidth(wc);
}
#endif // _TV_UNIX

#ifdef __linux__
int LinuxConsoleStrategy::charWidth(TStringView, char32_t wc)
{
    // The Linux Console does not support zero-width characters. It assumes
    // all characters are either single or double-width. Additionally, the
    // double-width characters are the same as in the wcwidth() implementation by
    // Markus Kuhn from 2007-05-26 (https://www.cl.cam.ac.uk/~mgk25/ucs/wcwidth.c).
    return 1 +
        (wc >= 0x1100 &&
         (wc <= 0x115f ||
          wc == 0x2329 || wc == 0x232a ||
          (wc >= 0x2e80 && wc <= 0xa4cf &&
           wc != 0x303f) ||
          (wc >= 0xac00 && wc <= 0xd7a3) ||
          (wc >= 0xf900 && wc <= 0xfaff) ||
          (wc >= 0xfe10 && wc <= 0xfe19) ||
          (wc >= 0xfe30 && wc <= 0xfe6f) ||
          (wc >= 0xff00 && wc <= 0xff60) ||
          (wc >= 0xffe0 && wc <= 0xffe6) ||
          (wc >= 0x20000 && wc <= 0x2fffd) ||
          (wc >= 0x30000 && wc <= 0x3fffd)));
}
#endif // __linux__

#ifdef _WIN32
int Win32ConsoleStrategy::charWidth(TStringView mbc, char32_t)
{
    return WinWidth::mbcwidth(mbc);
}
#endif // _WIN32

namespace ttext
{

    static inline bool isZWJ(TStringView mbc)
    // We want to avoid printing certain characters which are usually represented
    // differently by different terminal applications or which can combine different
    // characters together, changing the width of a whole string.
    {
        return mbc == "\xE2\x80\x8D"; // U+200D ZERO WIDTH JOINER.
    }

} // namespace ttext

TText::eat_r TText::eat_internal( TSpan<TScreenCell> cells, size_t i,
                                  TStringView text, size_t j )
{
    using namespace ttext;
    if (j < text.size())
    {
        auto mb = TText::mbstat(text.substr(j));
        if (mb.length <= 1)
        {
            if (i < cells.size())
            {
                // This is redudant except if non-UTF-8 text is mixed with
                // combining characters.
                if (mb.length < 0)
                    ::setChar(cells[i], CpTranslator::toUtf8Int(text[j]));
                else if (text[j] == '\0')
                    ::setChar(cells[i], ' ');
                else
                    ::setChar(cells[i], text[j]);
                return {true, 1, 1};
            }
        }
        else
        {
            if (mb.width < 0)
            {
                if (i < cells.size())
                {
                    ::setChar(cells[i], "�");
                    return {true, 1, mb.length};
                }
            }
            else if (mb.width == 0)
            {
                TStringView zwc {&text[j], (size_t) mb.length};
                // Append to the previous cell, if present.
                if (i > 0 && !isZWJ(zwc))
                {
                    size_t k = i;
                    while (cells[--k].ch.isWideCharTrail() && k > 0);
                    cells[k].ch.appendZeroWidth(zwc);
                }
                return {true, 0, mb.length};
            }
            else
            {
                if (i < cells.size())
                {
                    bool wide = mb.width > 1;
                    ::setChar(cells[i], {&text[j], (size_t) mb.length}, wide);
                    // Fill trailing cells.
                    auto attr = ::getAttr(cells[i]);
                    int count = min(wide + 1, cells.size() - i);
                    for (int k = 1; k < count; ++k)
                        ::setCell(cells[i + k], TCellChar::wideCharTrail, attr);
                    return {true, count, mb.length};
                }
            }
        }
    }
    return {false, 0, 0};
}

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   TTEXT.H                                                               */
/*                                                                         */
/*   Defines functions related to multibyte string manipulation.           */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_TText ) && !defined( __TText )
#define __TText

#ifndef __BORLANDC__
#include <string.h>
#endif

class TText {

    // This class is actually a namespace.

public:

    static size_t next(TStringView text);
    static size_t prev(TStringView text, size_t index);
    static size_t wseek(TStringView text, int count, Boolean incRemainder=True);

    static size_t fill(TSpan<TScreenCell> cells, TStringView text);
    static size_t fill(TSpan<TScreenCell> cells, TStringView text, TCellAttribs attr);
#ifndef __BORLANDC__
    static size_t fill(TSpan<TScreenCell> cells, TStringView text, uchar attr);
    template<class Func>
    static size_t fill(TSpan<TScreenCell> cells, TStringView text, Func &&func);
#endif

    static Boolean eat(TSpan<TScreenCell> cells, size_t &i, TStringView text, size_t &j);
    static Boolean next(TStringView text, size_t &bytes, size_t &width);
    static Boolean next(TStringView text, size_t &bytes);
    static void wseek(TStringView text, size_t &index, size_t &remainder, int count);

private:

#ifndef __BORLANDC__
    struct mbstat_r { int length, width; };
    static mbstat_r mbstat(TStringView text);
    static int mblen(TStringView text);
    static bool isBlacklisted(TStringView mbc);
    static uint32_t cp2utf8(uchar c);
#endif // __BORLANDC__

};

#ifdef __BORLANDC__

inline size_t TText::next(TStringView text)
{
    return text.size() ? 1 : 0;
}

inline size_t TText::prev(TStringView, size_t index)
{
    return index ? 1 : 0;
}

inline size_t TText::wseek(TStringView text, int count, Boolean)
{
    return count > 0 ? min(count, text.size()) : 0;
}

inline Boolean TText::eat( TSpan<TScreenCell> cells, size_t &i,
                           TStringView text, size_t &j )
{
    if (i < cells.size() && j < text.size()) {
        ::setChar(cells[i], text[j]);
        ++i;
        ++j;
        return True;
    }
    return False;
}

#pragma warn -inl

inline size_t TText::fill(TSpan<TScreenCell> cells, TStringView text)
{
    size_t count = min(cells.size(), text.size());
    for (size_t i = 0; i < count; ++i)
        ::setChar(cells[i], text[i]);
    return count;
}

inline size_t TText::fill(TSpan<TScreenCell> cells, TStringView text, TCellAttribs attr)
{
    size_t count = min(cells.size(), text.size());
    for (size_t i = 0; i < count; ++i)
        ::setCell(cells[i], text[i], attr);
    return count;
}

#pragma warn .inl

inline Boolean TText::next(TStringView text, size_t &index, size_t &width)
{
    if (index < text.size()) {
        ++index;
        ++width;
        return True;
    }
    return False;
}

inline Boolean TText::next(TStringView text, size_t &index)
{
    if (index < text.size()) {
        ++index;
        return True;
    }
    return False;
}

inline void TText::wseek(TStringView text, size_t &index, size_t &remainder, int count)
{
    if (count > 0)
        index += min(count, text.size());
    remainder = 0;
}

#else

inline size_t TText::next(TStringView text)
// Measures the length in bytes of the first multibyte character in 'text'.
// If the sequence is not valid UTF-8, length is 1.
{
    if (text.size())
    {
        int len = TText::mblen(text);
        return len <= 1 ? 1 : len;
    }
    return 0;
}

inline size_t TText::prev(TStringView text, size_t index)
// Measures the length in bytes of the character in 'text' right before position 'index'.
// If 'index' > 0 and that position is not preceded by a valid UTF-8 sequence, length is 1.
{
    if (index)
    {
        // Try reading backwards character by character, until a valid
        // character is found. This tolerates invalid characters.
        size_t lead = min<size_t>(index, 4);
        for (size_t i = 1; i <= lead; ++i)
        {
            int len = TText::mblen({&text[index - i], i});
            if (len > 0)
                return (size_t) len == i ? i : 1;
        }
        return 1;
    }
    return 0;
}

inline size_t TText::wseek(TStringView text, int count, Boolean incRemainder)
// Seeks a string by an amount of display columns ('count'). If that amount
// partially overlaps a multi-column character, the whole character is included,
// unless 'incRemainder' is False.
// Returns the number of bytes seeked.
{
    size_t index = 0, remainder = 0;
    wseek(text, index, remainder, count);
    if (!incRemainder && remainder)
        index -= TText::prev(text, index);
    return index;
}

inline size_t TText::fill(TSpan<TScreenCell> cells, TStringView text)
// Tries to fill all the cells in the 'cells' span with characters from 'text'.
// Preserves the attributes of filled cells.
// Returns the number of cells filled, which will be smaller than cells.size()
// if not enough characters can be extracted from 'text'.
// Note that one cell is always one column wide.
{
    size_t w = 0, b = 0;
    while (TText::eat(cells, w, text, b));
    return w;
}


inline size_t TText::fill(TSpan<TScreenCell> cells, TStringView text, TCellAttribs attr)
// Same as above, but sets the attributes of filled cells to 'attr'.
{
    size_t w = 0, b = 0;
    do {
        if (w < cells.size())
            ::setAttr(cells[w], attr);
    } while (TText::eat(cells, w, text, b));
    return w;
}

inline size_t TText::fill(TSpan<TScreenCell> cells, TStringView text, uchar attr)
{
    return TText::fill(cells, text, TCellAttribs {attr});
}

template<class Func>
inline size_t TText::fill(TSpan<TScreenCell> cells, TStringView text, Func &&func)
// Similar to the above, but gives total control over every iterated cell through
// the 'func' callback. 'func' takes a TScreenCell& by parameter.
// A possible use case for this is if you need to modify the cell attributes
// in a way other than simply replacing them.
//
//   Examples:
//     TText::fill(cells, text, [] (auto &cell) { cell.Attr.fgSet(0x00); }); // OK.
//     TText::fill(cells, text, [] (void) {}); // Error: callback cannot take a TScreenCell&.
{
    size_t w = 0, b = 0;
    do {
        if (w < cells.size())
            func(cells[w]); // If you get a compilation error here, you are using the wrong overload.
    } while (TText::eat(cells, w, text, b));
    return w;
}

inline uint32_t TText::cp2utf8(uchar c)
{
    extern const uint32_t *tv_cp2utf8;
    return tv_cp2utf8[c];
}

inline bool TText::isBlacklisted(TStringView mbc)
// We want to avoid printing certain characters which are usually represented
// differently by different terminal applications or which can combine different
// characters together, changing the width of a whole string.
{
    return mbc == "\xE2\x80\x8D"; // U+200D ZERO WIDTH JOINER.
}

inline Boolean TText::eat( TSpan<TScreenCell> cells, size_t &i,
                           TStringView text, size_t &j )
// Reads a single character from a multibyte-encoded string, and writes it into
// a screen cell.
//
// * cells: range of TScreenCells to write to. If you want the text to have attributes,
//   you should set them on cells[i] before invoking this function.
// * i (input/output parameter): index into 'cells'. Gets increased by
//   the display width of the text written into 'cells'.
// * text: input text.
// * j (input/output parameter): index into 'text'. Gets increased by
//   the number of bytes read from 'text'.
//
// A screen cell may contain one printable character (of width 1 or more) and several
// combining characters appended to it (of width 0).
//
// So, when a zero-width character is found in 'text', it is combined with the
// previous cell, i.e. cells[i - 1], as long as i > 0.
//
// Returns false when no more text can be written into 'cells'. In other words,
// it is safe to use TText::eat in a loop, like this:
//
//      size_t i = 0, j = 0;
//      while (TText::eat(cells, i, text, j));
{
    if (j < text.size())
    {
        auto mb = TText::mbstat(text.substr(j));
        if (mb.length <= 1)
        {
            if (i < cells.size())
            {
                if (mb.length < 0)
                    ::setChar(cells[i], cp2utf8(text[j]));
                else if (mb.length == 0) // '\0'
                    ::setChar(cells[i], ' ');
                else
                    ::setChar(cells[i], {&text[j], 1});
                i += 1;
                j += 1;
                return true;
            }
        }
        else
        {
            if (mb.width < 0)
            {
                if (i < cells.size())
                {
                    ::setChar(cells[i], "�");
                    i += 1;
                    j += mb.length;
                    return true;
                }
            }
            else if (mb.width == 0)
            {
                TStringView zwc {&text[j], (size_t) mb.length};
                // Append to the previous cell, if present.
                if (i > 0 && !isBlacklisted(zwc))
                {
                    size_t k = i;
                    while (cells[--k].Char == TScreenCell::wideCharTrail && k > 0);
                    cells[k].Char.append(zwc);
                }
                j += mb.length;
                return true;
            }
            else
            {
                if (i < cells.size())
                {
                    uchar extraWidth = min(mb.width - 1, 7);
                    ::setChar(cells[i], {&text[j], (size_t) mb.length}, extraWidth);
                    // Fill trailing cells.
                    auto attr = ::getAttr(cells[i]);
                    size_t count = min(extraWidth + 1, cells.size() - i);
                    for (size_t k = 1; k < count; ++k)
                        ::setCell(cells[i + k], TScreenCell::wideCharTrail, attr);
                    i += count;
                    j += mb.length;
                    return true;
                }
            }
        }
    }
    return false;
}

inline Boolean TText::next(TStringView text, size_t &index, size_t &width)
// Measures the length and width of the character starting at '&text[index]'.
//
// * text: input text.
// * index (input/output parameter): index into 'text'. Gets increased by
//   the length of the character.
// * width (input/output parameter): gets increased by the display width of the character.
//
// Returns false if 'index >= text.size()'.
{
    if (index < text.size())
    {
        auto mb = TText::mbstat(text.substr(index));
        if (mb.length <= 1)
        {
            index += 1;
            width += 1;
        }
        else
        {
            mb.width = min(mb.width, 8);
            if (mb.width != 0)
                width += max(mb.width, 1);
            index += mb.length;
        }
        return true;
    }
    return false;
}

inline Boolean TText::next(TStringView text, size_t &index)
// Measures the length of the character starting at '&text[index]'.
//
// * text: input text.
// * index (input/output parameter): index into 'text'. Gets increased by
//   the length of the character.
//
// Returns false if 'index >= text.size()'.
{
    size_t len = TText::next(text.substr(index));
    if (len)
    {
        index += len;
        return true;
    }
    return false;
}

inline void TText::wseek(TStringView text, size_t &index, size_t &remainder, int count)
// Seeks a string by an amount of display columns. If that amount overlaps a multi-column
// character, 'index' is left pointing to the next character and 'remainder' is set to
// the number of extra seeked columns.
//
// * index (input and output parameter): start position.
// * remainder (output parameter): number of columns in the middle of a wide character.
// * count: number of columns to seek.
{
    if (count > 0) {
        while (index < text.size()) {
            size_t width = 0;
            TText::next(text, index, width);
            count -= width;
            if (count <= 0) {
                // Immediately return when the requested width is exceeded.
                remainder = -count;
                return;
            }
        }
    }
    // No remainder when the end of string was reached.
    remainder = 0;
}

#endif // __BORLANDC__

#endif // Uses_TText

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
#include <tvision/internal/codepage.h>
#include <algorithm>
#include <cstring>
#include <cwchar>
#include <type_traits>
#ifdef _WIN32
#include <tvision/internal/winwidth.h>
#endif
#endif

class TText {

    // This class is actually a namespace.

public:

    static size_t next(TStringView text);
    static size_t prev(TStringView text, size_t index);
    static size_t wseek(TStringView text, int count, Boolean incRemainder=True);

    static size_t fill(TSpan<TScreenCell> cells, TStringView text);
#ifdef __BORLANDC__
    static size_t fill(TSpan<TScreenCell> cells, TStringView text, TCellAttribs attr);
#else
    template<class Attr>
    static size_t fill(TSpan<TScreenCell> cells, TStringView text, Attr &&attr);
#endif

    static Boolean eat(TSpan<TScreenCell> cells, size_t &width, TStringView text, size_t &bytes);
    static void next(TStringView text, size_t &bytes, size_t &width);
    static void wseek(TStringView text, size_t &index, size_t &remainder, int count);

private:

#ifndef __BORLANDC__
#ifdef _WIN32
    static int width(TStringView mbc);
#else
    static int width(wchar_t wc);
#endif
    static bool isBlacklisted(TStringView mbc);
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

inline void TText::next(TStringView text, size_t &index, size_t &width)
{
    if (index < text.size()) {
        ++index;
        ++width;
    }
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
    if (text.size()) {
        std::mbstate_t state {};
        int len = std::mbrtowc(nullptr, text.data(), text.size(), &state);
        return len <= 1 ? 1 : len;
    }
    return 0;
}

inline size_t TText::prev(TStringView text, size_t index)
// Measures the length in bytes of the character in 'text' right before position 'index'.
// If 'index' > 0 and that position is not preceded by a valid UTF-8 sequence, length is 1.
{
    if (index) {
        // Try reading backwards character by character, until a valid
        // character is found. This tolerates invalid characters.
        size_t lead = std::min<size_t>(index, 4);
        for (size_t i = 1; i <= lead; ++i) {
            std::mbstate_t state {};
            int64_t size = std::mbrtowc(nullptr, &text[index - i], i, &state);
            if (size > 0)
                return (size_t) size == i ? i : 1;
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

template<class Attr>
inline size_t TText::fill(TSpan<TScreenCell> cells, TStringView text, Attr &&attr)
// Same as above, but gives total control over every iterated cell:
//
// * If 'Attr' is a callable type, it is invoked with the cell as parameter.
//   The most common use case for this is if you need to modify the cell attributes
//   in a way other than simply replacing them.
//
//   Examples:
//     TText::fill(cells, text, [] (auto &cell) { cell.Attr.fgSet(0x00); }); // OK.
//     TText::fill(cells, text, [] (void) {}); // Error: callback cannot take a TScreenCell&.
//
// * Otherwise, 'attr' is directly assigned to the cell's attributes.
{
    size_t w = 0, b = 0;
    do {
        if (w < cells.size()) {
            if constexpr (std::is_invocable<Attr, TScreenCell&>())
                attr(cells[w]);
            else
                ::setAttr(cells[w], attr);
        }
    } while (TText::eat(cells, w, text, b));
    return w;
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
    if (j < text.size()) {
        wchar_t wc;
        std::mbstate_t state {};
        int len = std::mbrtowc(&wc, &text[j], text.size() - j, &state);
        if (len <= 1) {
            if (i < cells.size()) {
                if (len < 0)
                    ::setChar(cells[i], CpTranslator::toUtf8Int(text[j]));
                else if (len == 0) // '\0'
                    ::setChar(cells[i], ' ');
                else {
                    ::setChar(cells[i], {&text[j], 1});
                }
                i += 1;
                j += 1;
                return true;
            }
        } else {
#ifdef _WIN32
            int cWidth = TText::width({&text[j], (size_t) len});
#else
            int cWidth = TText::width(wc);
#endif
            if (cWidth < 0) {
                if (i < cells.size()) {
                    ::setChar(cells[i], "�");
                    i += 1;
                    j += len;
                    return true;
                }
            } else if (cWidth == 0) {
                TStringView zwc {&text[j], (size_t) len};
                // Append to the previous cell, if present.
                if (i > 0 && !isBlacklisted(zwc)) {
                    size_t k = i;
                    while (cells[--k].Char == TScreenCell::wideCharTrail && k > 0);
                    cells[k].Char.append(zwc);
                }
                j += len;
                return true;
            } else {
                if (i < cells.size()) {
                    uchar extraWidth = std::min<size_t>(cWidth - 1, 7);
                    ::setChar(cells[i], {&text[j], (size_t) len}, extraWidth);
                    // Fill trailing cells.
                    auto attr = ::getAttr(cells[i]);
                    size_t count = std::min<size_t>(extraWidth + 1, cells.size() - i);
                    for (size_t k = 1; k < count; ++k)
                        ::setCell(cells[i + k], TScreenCell::wideCharTrail, attr);
                    i += count;
                    j += len;
                    return true;
                }
            }
        }
    }
    return false;
}

inline void TText::next(TStringView text, size_t &index, size_t &width)
// Measures the length and width of the first character in 'text'.
//
// * text: input text.
// * index (input/output parameter): index into 'text'. Gets increased by
//   the length of the first character in 'text'.
// * width (output parameter): gets increased by the display width of the first character in 'text'.
{
    if (index < text.size()) {
        wchar_t wc;
        std::mbstate_t state {};
        int len = std::mbrtowc(&wc, &text[index], text.size() - index, &state);
        if (len <= 1) {
            index += 1;
            width += 1;
        } else {
#ifdef _WIN32
            int cWidth = std::min<int>(TText::width({&text[index], (size_t) len}), 8);
#else
            int cWidth = std::min<int>(TText::width(wc), 8);
#endif
            if (cWidth != 0)
                width += std::max<int>(cWidth, 1);
            index += len;
        }
    }
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

#ifdef _WIN32
inline int TText::width(TStringView mbc)
{
    return WinWidth::mbcwidth(mbc);
}
#else
inline int TText::width(wchar_t wc)
{
    return wcwidth(wc);
}
#endif // _WIN32

#endif // __BORLANDC__

#endif // Uses_TText

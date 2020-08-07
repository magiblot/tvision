/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   TTEXT.H                                                               */
/*                                                                         */
/*   Defines functions related to multibyte string manipulation.           */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef TVISION_TTEXT_H
#define TVISION_TTEXT_H

#ifdef __BORLANDC__
#define constexpr
#else
#include <internal/codepage.h>
#include <algorithm>
#include <string_view>
#include <cstring>
#include <cwchar>
#endif

class TStringView {

    const char _FAR *str;
    size_t len;

public:

    constexpr TStringView();
    TStringView(const char _FAR *str);
    constexpr TStringView(const char _FAR *str, size_t len);
#ifndef __BORLANDC__
    constexpr TStringView(std::string_view text);
    constexpr operator std::string_view() const;
#endif

    constexpr const char* data() const;
    constexpr size_t size() const;
    constexpr Boolean empty() const;
    constexpr const char& operator[](size_t pos) const;

    constexpr TStringView substr(size_t pos) const;
    constexpr TStringView substr(size_t pos, size_t n) const;

};

inline constexpr TStringView::TStringView() :
    str(0),
    len(0)
{
}

inline TStringView::TStringView(const char _FAR *str) :
    str(str),
    len(strlen(str))
{
}

inline constexpr TStringView::TStringView(const char _FAR *str, size_t len) :
    str(str),
    len(len)
{
}

#ifndef __BORLANDC__
inline constexpr TStringView::TStringView(std::string_view text) :
    str(text.data()),
    len(text.size())
{
}

inline constexpr TStringView::operator std::string_view() const
{
    return {str, len};
}
#endif

inline constexpr const char* TStringView::data() const
{
    return str;
}

inline constexpr size_t TStringView::size() const
{
    return len;
}

inline constexpr Boolean TStringView::empty() const
{
    return Boolean(size() == 0);
}

inline constexpr const char& TStringView::operator[](size_t pos) const
{
    return str[pos];
}

inline constexpr TStringView TStringView::substr(size_t pos) const
{
    return TStringView(str + pos, len - pos);
}

inline constexpr TStringView TStringView::substr(size_t pos, size_t n) const
{
    size_t tail = len - pos;
    if (n > tail)
        n = tail;
    return TStringView(str + pos, n);
}

class TText {

// Note: this class is actually a namespace.

public:

    static size_t next(TStringView text);
    static size_t prev(TStringView text, size_t index);
    static size_t wseek(TStringView text, int count);

#ifndef __BORLANDC__
    static void eat(TScreenCell *cell, size_t n, size_t &width, TStringView src, size_t &bytes);
    static void next(TStringView src, size_t &bytes, size_t &width);
    static void wseek(TStringView text, size_t &index, size_t &remainder, int count);
#endif

};

#ifdef __BORLANDC__

inline size_t TText::next(TStringView text)
{
    return text.size() ? 1 : 0;
}

inline size_t TText::prev(TStringView text, size_t index)
{
    return index ? 1 : 0;
}

inline size_t TText::wseek(TStringView text, int count)
{
    return count > 0 ? count : 0;
}

#else

inline size_t TText::next(TStringView text)
{
    if (text.size()) {
        std::mbstate_t state {};
        int64_t len = std::mbrtowc(nullptr, text.data(), text.size(), &state);
        return len <= 1 ? 1 : len;
    }
    return 0;
}

inline size_t TText::prev(TStringView text, size_t index)
{
    if (index) {
        // Try reading backwards character by character, until a valid
        // character is found. This tolerates invalid characters.
        size_t lead = std::min<size_t>(index, 4);
        for (size_t i = 1; i <= lead; ++i) {
            std::mbstate_t state {};
            int64_t size = std::mbrtowc(nullptr, &text[index - i], i, &state);
            if (size > 0)
                return size == i ? i : 1;
        }
        return 1;
    }
    return 0;
}

inline size_t TText::wseek(TStringView text, int count)
{
    size_t index = 0, remainder = 0;
    wseek(text, index, remainder, count);
    return index;
}

inline void TText::eat( TScreenCell *cell, size_t n, size_t &width,
                        TStringView src, size_t &bytes )
// Reads a single character from a multibyte-encoded string. The display width of
// a character may be 1 or more cells. All such cells (at most 'n') get updated
// accordingly.
//
// * cell: TScreenCell to write to.
// * n: maximum number of cells that can be written to.
// * width (output parameter): gets increased by the display width of the text in cell.
// * src: input text.
// * bytes (output parameter): gets increased by the number of bytes read from 'src'.
{
    if (n) {
        auto &dst = cell->Char;
        auto &attr = cell->Attr;
        cell->extraWidth = 0;
        wchar_t wc;
        std::mbstate_t state {};
        int64_t len = std::mbrtowc(&wc, src.data(), src.size(), &state);
        if (len <= 1) {
            bytes += 1;
            width += 1;
            if (len < 0)
                dst = CpTranslator::toUtf8Int(src[0]);
            else if (len == 0) // '\0'
                dst = ' ';
            else {
                dst = 0;
                memcpy(&dst.bytes, src.data(), 1);
            }
        } else {
            int cWidth = wcwidth(wc);
            bytes += len;
            if (cWidth <= 0) {
                width += 1;
                dst = *(uint32_t *) "�";
            } else {
                width += cWidth;
                dst = 0;
                memcpy(&dst.bytes, src.data(), len);
                // Set extraWidth attribute and fill trailing cells.
                cell->extraWidth = std::min<size_t>(cWidth - 1, 7);
                while (--cWidth && --n) {
                    ++cell;
                    auto trailCell = *cell;
                    trailCell.Char = TScreenCell::wideCharTrail;
                    trailCell.Attr = attr;
                    *cell = trailCell;
                }
            }
        }
    }
}

inline void TText::next(TStringView src, size_t &bytes, size_t &width)
// Measures the length and width of the first character in 'src'.
//
// * src: input text.
// * bytes (output parameter): gets increased by the length of the first character in 'src'.
// * width (output parameter): gets increased by the display width of the first character in 'src'.
{
    if (src.size()) {
        wchar_t wc;
        std::mbstate_t state {};
        int64_t len = std::mbrtowc(&wc, src.data(), src.size(), &state);
        if (len <= 1) {
            bytes += 1;
            width += 1;
        } else {
            bytes += len;
            width += std::clamp(wcwidth(wc), 1, 8);
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
        while (count > 0 && index < text.size()) {
            size_t width = 0;
            TText::next({&text[index], text.size() - index}, index, width);
            count -= width;
        }
        remainder = -count;
    } else
        remainder = 0;
}

#endif // __BORLANDC__
#ifdef __BORLANDC__
#undef constexpr
#endif
#endif

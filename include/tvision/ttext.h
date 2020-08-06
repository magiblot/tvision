/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   TTEXT.H                                                               */
/*                                                                         */
/*   Defines functions related to multibyte string manipulation.           */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef TVISION_TTEXT_H
#define TVISION_TTEXT_H

#ifndef __BORLANDC__

#include <internal/codepage.h>
#include <algorithm>
#include <string_view>
#include <cstring>
#include <cwchar>

class TText {

// Note: this class is actually a namespace.

public:

    static void eat(TScreenCell *cell, size_t n, size_t &width, std::string_view src, size_t &bytes);
    static void next(std::string_view src, size_t &bytes, size_t &width);
    static void wseek(std::string_view text, size_t &index, size_t &remainder, int count);

};

inline void TText::eat( TScreenCell *cell, size_t n, size_t &width,
                        std::string_view src, size_t &bytes )
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

inline void TText::next(std::string_view src, size_t &bytes, size_t &width)
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

inline void TText::wseek(std::string_view text, size_t &index, size_t &remainder, int count)
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
#endif

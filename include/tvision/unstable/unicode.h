/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   UNICODE.H                                                             */
/*                                                                         */
/*   Defines functions related to multibyte string manipulation.           */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef TVISION_UNICODE_H
#define TVISION_UNICODE_H

#include <internal/codepage.h>
#include <algorithm>
#include <string_view>
#include <cstring>
#include <cwchar>

inline void utf8read( TScreenCell *cell, size_t n, size_t &width,
                      std::string_view src, size_t &bytes, std::mbstate_t &state )
// Reads a single character from a multibyte-encoded string. The display width of
// a character may be 1 or more cells. All such cells (at most 'n') get updated
// accordingly.
//
// * cell: TScreenCell to write to.
// * n: maximum number of cells that can be written to.
// * width (output parameter): gets increased by the display width of the text in cell.
// * src: input text.
// * bytes (output parameter): gets increased by the number of bytes read from 'src'.
// * state: check the overload below as you probably don't need this.
{
    if (n) {
        auto &dst = cell->Cell.Char.asInt;
        auto &attr = cell->Cell.Attr;
        wchar_t wc;
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
                memcpy(&dst, src.data(), 1);
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
                memcpy(&dst, src.data(), len);
                // Set extraWidth attribute and fill trailing cells.
                cell->Cell.extraWidth = std::min<size_t>(cWidth - 1, 7);
                while (--cWidth && --n) {
                    ++cell;
                    auto trailCell = cell->Cell;
                    trailCell.Char.asInt = TScreenCell::wideCharTrail;
                    trailCell.Attr = attr;
                    cell->Cell = trailCell;
                }
            }
        }
    }
}

inline void utf8read( TScreenCell *cell, size_t n, size_t &width,
                      std::string_view src, size_t &bytes )
{
    std::mbstate_t state {};
    return utf8read(cell, n, width, src, bytes, state);
}

inline void utf8next(std::string_view src, size_t &bytes, size_t &width, std::mbstate_t &state)
// Measures the length and width of the first character in 'src'.
//
// * src: input text.
// * bytes (output parameter): gets increased by the length of the first character in 'src'.
// * width (output parameter): gets increased by the display width of the first character in 'src'.
// * state: check the overload below as you probably don't need this.
{
    if (src.size()) {
        wchar_t wc;
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

inline void utf8next(std::string_view src, size_t &bytes, size_t &width)
{
    std::mbstate_t state {};
    return utf8next(src, bytes, width, state);
}

inline void utf8wseek(std::string_view text, size_t &index, int count, std::mbstate_t &state)
// Seeks a string by an amount of display columns.
//
// * index (input and output parameter): start position.
// * count: number of columns.
// * state: check the overload below as you probably don't need this.
{
    while (count > 0 && index < text.size()) {
        size_t width = 0;
        utf8next(text.substr(index, text.size() - index), index, width, state);
        count -= width;
    }
}

inline void utf8wseek(std::string_view text, size_t &index, int count)
// Seeks a string by a positive amount of display columns.
//
// * index (input and output parameter): start position.
// * count: number of columns.
// * state: check the overload below as you probably don't need this.
{
    std::mbstate_t state {};
    utf8wseek(text, index, count, state);
}

#endif

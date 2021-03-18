/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   SCRNCELL.H                                                            */
/*                                                                         */
/*   Defines the structs TCellChar and TScreenCell.                        */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef TVISION_SCRNCELL_H
#define TVISION_SCRNCELL_H

#ifdef __BORLANDC__

inline const TColorAttr &getAttr(const TScreenCell &cell)
{
    return ((uchar *) &cell)[1];
}

inline void setAttr(TScreenCell &cell, TColorAttr attr)
{
    ((uchar *) &cell)[1] = attr;
}

inline const TCellChar &getChar(const TScreenCell &cell)
{
    return ((uchar *) &cell)[0];
}

inline void setChar(TScreenCell &cell, TCellChar ch)
{
    ((uchar *) &cell)[0] = ch;
}

inline void setCell(TScreenCell &cell, TCellChar ch, TColorAttr attr)
{
    cell = ushort((attr << 8) | ch);
}

#else

//// TCellChar
//
// Represents text in a screen cell. You should usually not need to interact
// with this manually. In order to write text into a screen cell, just use
// the functions in the TText namespace.
//
// INVARIANT:
// * '_text' contains one of the following:
//     1. A single byte of ASCII or 'extended ASCII' text (1 column wide).
//     2. Up to 12 bytes of UTF-8 text (1 or 2 columns wide in total,
//        meaning that it must not contain just zero-width characters).
//     3. A special value that marks it as wide char trail.

struct TCellChar
{

    uint8_t _text[12];

    enum WideCharTrail : uint32_t { wideCharTrail=(uint32_t)-2 };

    TCellChar() = default;
    inline TCellChar(WideCharTrail);
    inline TCellChar(char ch);
    inline TCellChar(uchar ch);
    inline TCellChar(uint32_t ch);
    inline TCellChar(TStringView text);
    TV_TRIVIALLY_ASSIGNABLE(TCellChar)

    inline bool isWideCharTrail() const;
    inline void appendZeroWidth(TStringView text);
    inline TStringView asText() const;
    inline size_t size() const;

    inline uint8_t& operator[](size_t i);
    inline const uint8_t& operator[](size_t i) const;
    inline bool operator==(const TCellChar &other) const;
    inline bool operator!=(const TCellChar &other) const;

};

inline TCellChar::TCellChar(WideCharTrail)
{
    *this = (uint32_t) wideCharTrail;
}

inline TCellChar::TCellChar(char ch)
{
    *this = (uchar) ch;
}

inline TCellChar::TCellChar(uchar ch)
{
    memset(this, 0, sizeof(*this));
    memcpy(_text, &ch, sizeof(ch));
}

inline TCellChar::TCellChar(uint32_t ch)
{
    memset(this, 0, sizeof(*this));
    memcpy(_text, &ch, sizeof(ch));
}

inline TCellChar::TCellChar(TStringView text)
{
    memset(this, 0, sizeof(*this));
    if (text.size() <= sizeof(_text))
        memcpy(_text, text.data(), text.size());
}

inline bool TCellChar::isWideCharTrail() const
{
    uint32_t ch;
    memcpy(&ch, _text, sizeof(ch));
    return ch == wideCharTrail;
}

inline void TCellChar::appendZeroWidth(TStringView text)
{
    if (!_text[0])
        _text[0] = ' ';
    size_t sz = size();
    if (text.size() <= sizeof(_text) - sz)
        memcpy(&_text[sz], text.data(), text.size());
}

inline TStringView TCellChar::asText() const
{
    return {(const char *) _text, size()};
}

inline size_t TCellChar::size() const
{
    size_t i = 0;
    while (++i < sizeof(_text) && _text[i]);
    return i;
}

inline uint8_t& TCellChar::operator[](size_t i)
{
    return _text[i];
}

inline const uint8_t& TCellChar::operator[](size_t i) const
{
    return _text[i];
}

inline bool TCellChar::operator==(const TCellChar &other) const
{
    return memcmp(this, &other, sizeof(*this)) == 0;
}

inline bool TCellChar::operator!=(const TCellChar &other) const
{
    return !(*this == other);
}

//// TScreenCell
//
// Stores the text and color attributes in a screen cell.
// Please use the functions in the TText namespace in order to fill screen cells
// with text.
//
// Considerations:
// * 'wide' indicates the width of the text in 'ch' minus one. In practice,
//   the only possible character widths are 0, 1 and 2, and the text in a
//   TCellChar is at least one column wide. So 'wide' will be either 0 or 1.
// * In order for a double-width character to be displayed entirely, its cell
//   must be followed by another containing a wide char trail. If it is not,
//   or if a wide char trail is not preceded by a double-width character,
//   we'll understand that a double-width character is being overlapped partially.

struct TScreenCell
{

    TColorAttr attr;
    TCellChar ch;
    uint8_t wide;
    uint8_t _unused[3];

    TScreenCell() = default;
    inline TScreenCell(ushort bios);
    TV_TRIVIALLY_ASSIGNABLE(TScreenCell)

    inline bool operator==(const TScreenCell &other) const;
    inline bool operator!=(const TScreenCell &other) const;

};

inline const TColorAttr &getAttr(const TScreenCell &cell);
inline void setAttr(TScreenCell &cell, const TColorAttr &attr);
inline const TCellChar &getChar(const TScreenCell &cell);
inline void setChar(TScreenCell &cell, const TCellChar &ch, bool wide=0);
inline void setChar(TScreenCell &cell, TStringView text, bool wide=0);
inline void setCell(TScreenCell &cell, const TCellChar &ch, const TColorAttr &attr, bool wide=0);

inline TScreenCell::TScreenCell(ushort bios)
{
    memset(this, 0, sizeof(*this));
    ch = uchar(bios);
    attr = uchar(bios >> 8);
}

inline bool TScreenCell::operator==(const TScreenCell &other) const
{
    return memcmp(this, &other, sizeof(*this)) == 0;
}

inline bool TScreenCell::operator!=(const TScreenCell &other) const
{
    return !(*this == other);
}

inline const TColorAttr &getAttr(const TScreenCell &cell)
{
    return cell.attr;
}

inline void setAttr(TScreenCell &cell, const TColorAttr &attr)
{
    cell.attr = attr;
}

inline const TCellChar &getChar(const TScreenCell &cell)
{
    return cell.ch;
}

inline void setChar(TScreenCell &cell, const TCellChar &ch, bool wide)
{
    cell.ch = ch;
    cell.wide = wide;
}

inline void setChar(TScreenCell &cell, TStringView text, bool wide)
{
    cell.ch = text;
    cell.wide = wide;
}

inline void setCell(TScreenCell &cell, const TCellChar &ch, const TColorAttr &attr, bool wide)
{
    memset(&cell, 0, sizeof(cell));
    ::setChar(cell, ch, wide);
    ::setAttr(cell, attr);
}

#ifdef SCRNCELL_DEBUG
#include <type_traits>

namespace scrncell
{
    template <class T>
    inline void check_trivial()
    {
        static_assert(std::is_trivial<T>(), "");
    }

    template<class C, class T = typename C::trivial_t>
    static void check_convertible()
    {
        scrncell::check_trivial<C>();
        static_assert(sizeof(C) == sizeof(T), "");
        static_assert(alignof(C) == alignof(T), "");
    }

    inline void check_assumptions()
    {
        check_trivial<TColorDesired>();
        check_trivial<TColorAttr>();
        check_trivial<TAttrPair>();
        check_trivial<TCellChar>();
        check_trivial<TScreenCell>();
        check_convertible<TColorBIOS>();
        check_convertible<TColorRGB>();
        check_convertible<TColorXTerm>();
        static_assert(sizeof(TScreenCell) == 24, "");
        static_assert(sizeof(TColorDesired) == 4, "");
        static_assert(sizeof(TColorAttr) == 8, "");
    }
}

#endif // SCRNCELL_DEBUG

#endif // __BORLANDC__

#endif // TVISION_SCRNCELL_H

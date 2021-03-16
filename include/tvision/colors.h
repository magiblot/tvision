/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   COLORS.H                                                              */
/*                                                                         */
/*   Defines the structs TColorBIOS, TColorRGB, TColorXTerm,               */
/*   TColorDesired, TColorAttr and TAttrPair.                              */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef TVISION_COLORS_H
#define TVISION_COLORS_H

#ifdef __BORLANDC__

inline TColorDesired getFore(const TColorAttr &attr)
{
    return uchar(attr & 0xF);
}

inline TColorDesired getBack(const TColorAttr &attr)
{
    return uchar(attr >> 4);
}

inline void setFore(TColorAttr &attr, TColorDesired color)
{
    attr = uchar(attr & 0xF0) | uchar(color & 0xF);
}

inline void setBack(TColorAttr &attr, TColorDesired color)
{
    attr = uchar(attr & 0xF) | uchar(color << 4);
}

inline TColorAttr reverseAttribute(TColorAttr attr)
{
    return uchar(attr << 4) | uchar(attr >> 4);
}

#else // __BORLANDC__

#include <string.h>

// Helper class for trivial types.

namespace colors
{

    template<class T, T mask = static_cast<T>(-1)>
    struct alignas(T) trivially_convertible
    {

        using trivial_t = T;

        // If you want the derived classes to be trivial, make sure you also
        // define a trivial default constructor in them.
        trivially_convertible() = default;

        trivially_convertible(T asT)
        {
            *this = asT;
        }

        T operator=(T asT)
        {
            asT &= mask;
            memcpy(this, &asT, sizeof(T));
            return asT;
        }

        operator T() const
        {
            T asT;
            memcpy(&asT, this, sizeof(T));
            return asT & mask;
        }

    };

} // namespace colors

//// Color Formats

////// TColorRGB
//
// Can be initialized like this:
//     TColorRGB rgb = {127, 0, 187}; // {red, green, blue}.
// Or like this (bit-casting from integer):
//     TColorRGB rgb = 0x7F00BB;      // 0xRRGGBB

struct TColorRGB : colors::trivially_convertible<uint32_t, 0xFFFFFF>
{
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t _unused;

    using trivially_convertible::trivially_convertible;
    TColorRGB() = default;
    TColorRGB(uint8_t r, uint8_t g, uint8_t b)
    {
        *this = b | ((g  | (r << 8)) << 8);
    }
};

////// TColorBIOS
//
// This is the 4-bit color encoding used originally by Turbo Vision on MS-DOS.
//
// Examples:
//     TColorBIOS black = 0x0,
//                blue = 0x1,
//                dark_gray = 0x8;

struct TColorBIOS : colors::trivially_convertible<uint8_t, 0xF>
{
    uint8_t
        b       : 1,
        g       : 1,
        r       : 1,
        bright  : 1,
        _unused : 4;

    using trivially_convertible::trivially_convertible;
    TColorBIOS() = default;
};

////// TColorXTerm
//
// Index into an 256-color palette recognized by the 'xterm-256color' terminal type.
// Some terminal emulators support modifying the palette, but Turbo Vision does
// not make use of this feature. So we assume index values 16 to 255 can be
// unequivocally converted from and into RGB as per the table in:
//
// https://jonasjacek.github.io/colors/
//
// Indices 0 to 15 will be displayed just like BIOS colors.

struct TColorXTerm : colors::trivially_convertible<uint8_t>
{
    uint8_t idx;

    using trivially_convertible::trivially_convertible;
    TColorXTerm() = default;
};

//// Color Conversion Functions
//
// They convert between the color types defined previously and other common
// color formats.
//
// No conversion from XTerm16 to RGB is provided because there is no consensus
// on how these colors should be represented and we don't want to encourage
// developers to assume the opposite. Most terminal emulators allow users to
// configure these colors through color schemes.
//
// That's not the case of XTerm256 indices 16 to 255 (with 0 to 15 being the
// same as XTerm16). So we assume these can be unambiguously mapped to a single
// RGB value as per the tables in:
//
// https://jonasjacek.github.io/colors/

inline uint8_t BIOStoXTerm16(TColorBIOS);
inline TColorBIOS RGBtoBIOS(TColorRGB);
inline uint8_t RGBtoXTerm16(TColorRGB);
inline uint8_t RGBtoXTerm256(TColorRGB);
inline TColorBIOS XTerm16toBIOS(uint8_t);
inline TColorRGB XTerm256toRGB(uint8_t); // Only for indices 16..255.
inline uint8_t XTerm256toXTerm16(uint8_t);

namespace detail
{
    template <class T, size_t N>
    struct constarray;

    uint8_t RGBtoXTerm16(TColorRGB);
    extern const constarray<uint8_t, 256> XTerm256toXTerm16LUT;
    extern const constarray<uint32_t, 256> XTerm256toRGBLUT;
}

inline uint8_t BIOStoXTerm16(TColorBIOS c)
{
    // Swap the Red and Blue bits.
    uchar aux = c.b;
    c.b = c.r;
    c.r = aux;
    return c;
}

inline TColorBIOS RGBtoBIOS(TColorRGB c)
{
    return XTerm16toBIOS(RGBtoXTerm16(c));
}

inline uint8_t RGBtoXTerm16(TColorRGB c)
{
    return detail::RGBtoXTerm16(c);
}

inline uint8_t RGBtoXTerm256(TColorRGB c)
{
    // The xterm-256color palette consists of:
    //
    // * [0..15]: 16 colors as in xterm-16color.
    // * [16..231]: 216 colors in a 6x6x6 cube.
    // * [232..255]: 24 grayscale colors.
    //
    // This function does not return indices in the range [0..15]. For that,
    // use 'RGBtoXTerm16' instead.
    //
    // Dark colors are underrepresented in the 6x6x6 cube. The channel values
    // [0, 1, 2, 3, 4, 5] correspond to the 8-bit values
    // [0, 95, 135, 175, 215, 255]. Thus there is a distance of 40 between
    // values, except for 0. Any 8-bit value smaller than 95 - 40/2 = 75
    // would have to be mapped into 0. To compensate a bit for this, we allow
    // values [55..74] to also be mapped into 1.
    //
    // Additionally, we fall back on the grayscale colors whenever using
    // the 6x6x6 color cube would round the color to pure black. This
    // makes it possible to preserve details that would otherwise be lost.
    auto cnvColor = [] (TColorRGB c)
    {
        auto scale = [] (uchar c)
        {
            c += 20 & -(c < 75);
            return uchar(max<uchar>(c, 35) - 35)/40;
        };
        uchar r = scale(c.r),
              g = scale(c.g),
              b = scale(c.b);
        return 16 + uchar(r*uchar(6) + g)*uchar(6) + b;
    };
    auto cnvGray = [] (uchar l)
    {
        if (l < 8 - 5)
            return 16;
        if (l >= 238 + 5)
            return 231;
        return 232 + uchar(max<uchar>(l, 3) - 3)/uchar(10);
    };

    uchar idx = cnvColor(c);
    if (c != XTerm256toRGB(idx))
    {
        uchar Xmin = min(min(c.r, c.g), c.b),
              Xmax = max(max(c.r, c.g), c.b);
        uchar C = Xmax - Xmin; // Chroma in the HSL/HSV theory.
        if (C < 12 || idx == 16) // Grayscale if Chroma < 12 or rounded to black.
        {
            uchar L = ushort(Xmax + Xmin)/2; // Lightness, as in HSL.
            idx = cnvGray(L);
        }
    }
    return idx;
}

inline TColorBIOS XTerm16toBIOS(uint8_t idx)
{
    return BIOStoXTerm16(idx);
}

inline uint8_t XTerm256toXTerm16(uint8_t idx)
{
    using namespace detail;
    return ((const uint8_t (&) [256]) XTerm256toXTerm16LUT)[idx];
}

inline TColorRGB XTerm256toRGB(uint8_t idx)
{
    using namespace detail;
    return ((const uint32_t (&) [256]) XTerm256toRGBLUT)[idx];
}

//// TColorDesired
//
// This is a union type of the different possible kinds of color: BIOS, RGB,
// XTerm or Default.
// The purpose of this type is to describe the foreground *or* background color
// of a screen cell.
//
// You can initialize as BIOS color with a char literal, as RGB with an integer
// literal and as Default with zero-initialization:
//     TColorDesired bios = '\xF',
//                   rgb  = 0x7F00BB,
//                   def  = {};
//
// In a terminal emulator, the 'default color' is the color of text that has no
// display attributes (bold, color...) enabled.

const uchar
    ctDefault       = 0x0,  // Terminal default.
    ctBIOS          = 0x1,  // TColorBIOS.
    ctRGB           = 0x2,  // TColorRGB.
    ctXTerm         = 0x3;  // TColorXTerm.

struct TColorDesired
{

    uint8_t _data[4];

    TColorDesired() = default;

    // Constructors for list-initialization.

    inline TColorDesired(char bios);   // e.g. {'\xF'}
    inline TColorDesired(uchar bios);
    inline TColorDesired(int rgb);     // e.g. {0x7F00BB}
    // Use zero-initialization for for type Default: {}

    // Constructors with explicit type names.

    inline TColorDesired(TColorBIOS bios);
    inline TColorDesired(TColorRGB rgb);
    inline TColorDesired(TColorXTerm xterm);

    inline uchar type() const;
    inline bool isDefault() const;
    inline bool isBIOS() const;
    inline bool isRGB() const;
    inline bool isXTerm() const;

    // No conversion is performed! Make sure to check the type first.

    inline TColorBIOS asBIOS() const;
    inline TColorRGB asRGB() const;
    inline TColorXTerm asXTerm() const;

    // Quantization to TColorBIOS.

    inline TColorBIOS toBIOS(bool isForeground) const;

    inline bool operator==(TColorDesired other) const;
    inline bool operator!=(TColorDesired other) const;

};

inline TColorDesired::TColorDesired(char bios) :
    TColorDesired(TColorBIOS(bios))
{
}

inline TColorDesired::TColorDesired(uchar bios) :
    TColorDesired(TColorBIOS(bios))
{
}

inline TColorDesired::TColorDesired(int rgb) :
    TColorDesired(TColorRGB(rgb))
{
}

inline TColorDesired::TColorDesired(TColorBIOS bios)
{
    uint32_t val = bios | (ctBIOS << 24);
    memcpy(this, &val, sizeof(*this));
}

inline TColorDesired::TColorDesired(TColorRGB rgb)
{
    uint32_t val = rgb | (ctRGB << 24);
    memcpy(this, &val, sizeof(*this));
}

inline TColorDesired::TColorDesired(TColorXTerm xterm)
{
    uint32_t val = xterm | (ctXTerm << 24);
    memcpy(this, &val, sizeof(*this));
}

inline uchar TColorDesired::type() const
{
    return _data[3];
}

inline bool TColorDesired::isDefault() const
{
    return type() == ctDefault;
}

inline bool TColorDesired::isBIOS() const
{
    return type() == ctBIOS;
}

inline bool TColorDesired::isRGB() const
{
    return type() == ctRGB;
}

inline bool TColorDesired::isXTerm() const
{
    return type() == ctXTerm;
}

inline TColorBIOS TColorDesired::asBIOS() const
{
    return _data[0];
}

inline TColorRGB TColorDesired::asRGB() const
{
    uint32_t val;
    memcpy(&val, this, sizeof(val));
    return val;
}

inline TColorXTerm TColorDesired::asXTerm() const
{
    return _data[0];
}

inline TColorBIOS TColorDesired::toBIOS(bool isForeground) const
{
    switch (type())
    {
        case ctBIOS:
            return asBIOS();
        case ctRGB:
            return RGBtoBIOS(asRGB());
        case ctXTerm:
        {
            uint8_t idx = asXTerm();
            if (idx >= 16)
                idx = XTerm256toXTerm16(idx);
            return XTerm16toBIOS(idx);
        }
        default:
            return isForeground ? 0x7 : 0x0;
    }
}

inline bool TColorDesired::operator==(TColorDesired other) const
{
    return memcmp(this, &other, sizeof(*this)) == 0;
}

inline bool TColorDesired::operator!=(TColorDesired other) const
{
    return !(*this == other);
}

//// TColorAttr
//
// Represents the color attributes of a screen cell.
// Examples:
//
//     /* Foreground: BIOS 0x7.             */
//     /* Background: RGB 0x7F00BB.         */
//     /* Style: Bold, Italic.              */
//     TColorAttr a = {'\x07', 0x7F00BB, slBold | slItalic};
//
//     /* Foreground: Default.              */
//     /* Background: BIOS 0xF.             */
//     /* Style: Normal.                    */
//     TColorAttr b = {{}, '\xF'};
//
// For backward-compatibility, you can also use initialize a TColorAttr
// with a BIOS color attribute:
//
//     /* Foreground: BIOS 0xD.             */
//     /* Background: BIOS 0x3.             */
//     /* Style: Normal.                    */
//     TColorAttr c = 0x3D;
//
// A zero-initialized TColorAttr has both the foreground and background
// colors set to 'default'. Therefore, a zero-initialized TColorAttr produces
// visible text.

const ushort

// TColorAttr Style masks

    slBold          = 0x001,
    slItalic        = 0x002,
    slUnderline     = 0x004,
    slBlink         = 0x008,
    slReverse       = 0x010, // Prefer using 'reverseAttribute()' instead.
    slStrike        = 0x020,

// Private masks

    slNoShadow      = 0x200; // Don't draw window shadows over this cell.

struct TAttrPair;

struct TColorAttr
{
    using Style = ushort;

    uint64_t
        _style      : 10,
        _fg         : 27,
        _bg         : 27;

    TColorAttr() = default;
    inline TColorAttr(uchar bios);
    inline TColorAttr(TColorDesired fg, TColorDesired bg, ushort style=0);
    inline TColorAttr(const TAttrPair &attrs);
    inline TColorAttr& operator=(uchar);
    inline TColorAttr& operator=(const TAttrPair &attrs);

    inline bool isBIOS() const;
    inline uchar asBIOS() const; // Result is meaningful only if it actually is BIOS.
    inline uchar toBIOS() const; // Quantization.

    inline operator uchar() const;
    inline TAttrPair operator<<(int shift) const;

    inline bool operator==(const TColorAttr &other) const;
    inline bool operator!=(const TColorAttr &other) const;

    inline bool operator==(int bios) const;
    inline bool operator!=(int bios) const;

};

inline TColorDesired getFore(const TColorAttr &attr);
inline TColorDesired getBack(const TColorAttr &attr);
inline ushort getStyle(const TColorAttr &attr);
inline void setFore(TColorAttr &attr, TColorDesired fg);
inline void setBack(TColorAttr &attr, TColorDesired bg);
inline void setStyle(TColorAttr &attr, ushort style);
inline TColorAttr reverseAttribute(TColorAttr attr);

inline TColorAttr::TColorAttr(uchar bios)
{
    *this = bios;
}

inline TColorAttr::TColorAttr(TColorDesired fg, TColorDesired bg, ushort style)
{
    memset(this, 0, sizeof(*this));
    ::setFore(*this, fg);
    ::setBack(*this, bg);
    ::setStyle(*this, style);
}

inline TColorAttr& TColorAttr::operator=(uchar bios)
{
    memset(this, 0, sizeof(*this));
    ::setFore(*this, TColorBIOS(bios));
    ::setBack(*this, TColorBIOS(bios >> 4));
    return *this;
}

inline bool TColorAttr::isBIOS() const
{
    return ::getFore(*this).isBIOS() & ::getBack(*this).isBIOS()
            & !::getStyle(*this);
}

inline uchar TColorAttr::asBIOS() const
{
    // 'this' must be a BIOS attribute. If it is not, the result will be
    // bogus but harmless. The important is that the result isn't '\x0'
    // unless this is BIOS attribute '\x0'.
    uchar bios = uchar(_fg) | uchar(_bg << 4);
    return isBIOS() ? bios : 0x5F;
}

inline uchar TColorAttr::toBIOS() const
{
    auto fg = ::getFore(*this),
         bg = ::getBack(*this);
    return fg.toBIOS(true) | (bg.toBIOS(false) << 4);
}

inline TColorAttr::operator uchar() const
{
    return asBIOS();
}

inline bool TColorAttr::operator==(const TColorAttr &other) const
{
    return memcmp(this, &other, sizeof(*this)) == 0;
}

inline bool TColorAttr::operator!=(const TColorAttr &other) const
{
    return !(*this == other);
}

inline bool TColorAttr::operator==(int bios) const
{
    return *this == TColorAttr {(uchar) bios};
}

inline bool TColorAttr::operator!=(int bios) const
{
    return !(*this == bios);
}

inline TColorDesired getFore(const TColorAttr &attr)
{
    uint32_t val = attr._fg;
    TColorDesired color;
    memcpy(&color, &val, sizeof(color));
    return color;
}

inline TColorDesired getBack(const TColorAttr &attr)
{
    uint32_t val = attr._bg;
    TColorDesired color;
    memcpy(&color, &val, sizeof(color));
    return color;
}

inline ushort getStyle(const TColorAttr &attr)
{
    return attr._style;
}

inline void setFore(TColorAttr &attr, TColorDesired color)
{
    uint32_t val;
    memcpy(&val, &color, sizeof(val));
    attr._fg = val;
}

inline void setBack(TColorAttr &attr, TColorDesired color)
{
    uint32_t val;
    memcpy(&val, &color, sizeof(val));
    attr._bg = val;
}

inline void setStyle(TColorAttr &attr, ushort style)
{
    attr._style = style;
}

inline TColorAttr reverseAttribute(TColorAttr attr)
{
    auto fg = ::getFore(attr),
         bg = ::getBack(attr);
    // The 'slReverse' attribute is represented differently by every terminal,
    // so it is better to swap the colors manually unless any of them is default.
    if (fg.isDefault() | bg.isDefault())
        ::setStyle(attr, ::getStyle(attr) ^ slReverse);
    else
    {
        ::setFore(attr, bg);
        ::setBack(attr, fg);
    }
    return attr;
}

//// TAttrPair

struct TAttrPair
{

    TColorAttr _attrs[2];

    TAttrPair() = default;
    inline TAttrPair(ushort bios);
    inline TAttrPair(const TColorAttr &lo, const TColorAttr &hi=uchar(0));
    inline TAttrPair& operator=(ushort bios);

    inline ushort asBIOS() const;

    inline operator ushort() const;
    inline TAttrPair& operator|=(TColorAttr attr);

    inline TColorAttr& operator[](size_t i);
    inline const TColorAttr& operator[](size_t i) const;

};

inline TAttrPair::TAttrPair(ushort bios)
{
    *this = bios;
}

inline TAttrPair::TAttrPair(const TColorAttr &lo, const TColorAttr &hi) :
    _attrs {lo, hi}
{
}

inline TAttrPair& TAttrPair::operator=(ushort bios)
{
    _attrs[0] = uchar(bios & 0xFF);
    _attrs[1] = uchar(bios >> 8);
    return *this;
}

inline ushort TAttrPair::asBIOS() const
{
    return _attrs[0].asBIOS() | ushort(_attrs[1].asBIOS() << 8);
}

inline TAttrPair::operator ushort() const
{
    return asBIOS();
}

inline TAttrPair& TAttrPair::operator|=(TColorAttr attr)
{
    // Legacy code may use '|=' on an attribute pair to set the lower attribute.
    _attrs[0] = attr;
    return *this;
}

inline TColorAttr& TAttrPair::operator[](size_t i)
{
    return _attrs[i];
}

inline const TColorAttr& TAttrPair::operator[](size_t i) const
{
    return _attrs[i];
}

// Pending methods from TColorAttr.

inline TColorAttr::TColorAttr(const TAttrPair &attrs)
{
    *this = attrs;
}

inline TColorAttr& TColorAttr::operator=(const TAttrPair &attrs)
{
    *this = attrs[0];
    return *this;
}

inline TAttrPair TColorAttr::operator<<(int shift) const
{
    // Legacy code may use '<< 8' on an attribute to construct an attribute pair.
    if (shift == 8)
        return {uchar(0), *this};
    return asBIOS() << shift;
}

#endif // __BORLANDC__

#endif // TVISION_COLORS_H

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   COLORS.H                                                              */
/*                                                                         */
/*   Defines the classes TColorBIOS, TColorRGB, TColorXTerm,               */
/*   TColor, TColorAttr and TAttrPair.                                     */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef TVISION_COLORS_H
#define TVISION_COLORS_H

#if defined( __BORLANDC__ )

// A BIOS color attribute byte. The lower four bits represent the foreground
// color and the higher four bits the background color. Depending on the video
// adapter configuration, the highest-order bit may represent either the
// background color intensity or the blink attribute.

typedef uchar TColorAttr;

// A pair of color attributes.

typedef ushort TAttrPair;

#else

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  class TColorRGB                                                        */
/*                                                                         */
/*  Represents a 24-bit RGB color.                                         */
/*                                                                         */
/*  Can be initialized like this:                                          */
/*      TColorRGB rgb = {127, 0, 187}; // {red, green, blue}.              */
/*  Or with an integer:                                                    */
/*      TColorRGB rgb = 0x7F00BB;      // 0xRRGGBB                         */
/*  Can be converted back to integer types:                                */
/*      uint32_t asInt = TColorRGB(127, 0, 187);                           */
/*  When doing so, the unused bits are discarded:                          */
/*      uint32_t(TColorRGB(0xAABBCCDD)) == 0xBBCCDD;                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/

class TColorRGB
{
public:

    constexpr TColorRGB() noexcept;
    constexpr TColorRGB(uint8_t r, uint8_t g, uint8_t b) noexcept;
    constexpr TColorRGB(uint32_t rgb) noexcept;

    constexpr uint8_t getRed() const noexcept;
    constexpr void setRed(uint8_t r) noexcept;
    constexpr uint8_t getGreen() const noexcept;
    constexpr void setGreen(uint8_t g) noexcept;
    constexpr uint8_t getBlue() const noexcept;
    constexpr void setBlue(uint8_t b) noexcept;

    constexpr operator uint32_t() const noexcept;

private:

    uint32_t
        _b      : 8,
        _g      : 8,
        _r      : 8,
        _unused : 8;
};

constexpr TColorRGB::TColorRGB() noexcept :
    TColorRGB(0)
{
    (void) _unused;
}

constexpr TColorRGB::TColorRGB(uint8_t r, uint8_t g, uint8_t b) noexcept :
    _b(b),
    _g(g),
    _r(r),
    _unused(0)
{
}

constexpr TColorRGB::TColorRGB(uint32_t rgb) noexcept :
    _b(uint8_t(rgb)),
    _g(uint8_t(rgb >> 8)),
    _r(uint8_t(rgb >> 16)),
    _unused(0)
{
}

constexpr uint8_t TColorRGB::getRed() const noexcept
{
    return _r;
}

constexpr void TColorRGB::setRed(uint8_t r) noexcept
{
    _r = r;
}

constexpr uint8_t TColorRGB::getGreen() const noexcept
{
    return _g;
}

constexpr void TColorRGB::setGreen(uint8_t g) noexcept
{
    _g = g;
}

constexpr uint8_t TColorRGB::getBlue() const noexcept
{
    return _b;
}

constexpr void TColorRGB::setBlue(uint8_t b) noexcept
{
    _b = b;
}

constexpr TColorRGB::operator uint32_t() const noexcept
{
    return (_r << 8 | _g) << 8 | _b;
}

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  class TColorBIOS                                                       */
/*                                                                         */
/*  This is the 4-bit color encoding used originally by Turbo Vision on    */
/*  DOS.                                                                   */
/*                                                                         */
/*  In terminal emulators, BIOS colors are displayed using the 16 basic    */
/*  ANSI colors.                                                           */
/*                                                                         */
/*  This type can be converted from and into integer types:                */
/*      TColorBIOS black = 0x0,                                            */
/*                 blue = 0x1,                                             */
/*                 dark_gray = 0x8;                                        */
/*      uint8_t asChar = TColorBIOS(0xC);                                  */
/*  When doing so, the unused bits are dropped:                            */
/*      uint8_t(TColorBIOS(0xAB)) == 0xB;                                  */
/*                                                                         */
/*-------------------------------------------------------------------------*/

class TColorBIOS
{
public:

    constexpr TColorBIOS() noexcept;
    constexpr TColorBIOS(uint8_t irgb) noexcept;

    constexpr bool getRed() const noexcept;
    constexpr void setRed(bool r) noexcept;
    constexpr bool getGreen() const noexcept;
    constexpr void setGreen(bool g) noexcept;
    constexpr bool getBlue() const noexcept;
    constexpr void setBlue(bool b) noexcept;
    constexpr bool getIntensity() const noexcept;
    constexpr void setIntensity(bool i) noexcept;

    constexpr operator uint8_t() const noexcept;

private:

    uint8_t
        _b       : 1,
        _g       : 1,
        _r       : 1,
        _i       : 1,
        _unused  : 4;
};

constexpr TColorBIOS::TColorBIOS() noexcept :
    TColorBIOS(0)
{
    (void) _unused;
}

constexpr TColorBIOS::TColorBIOS(uint8_t irgb) noexcept :
    _b(irgb),
    _g(irgb >> 1),
    _r(irgb >> 2),
    _i(irgb >> 3),
    _unused(0)
{
}

constexpr bool TColorBIOS::getRed() const noexcept
{
    return _r;
}

constexpr void TColorBIOS::setRed(bool r) noexcept
{
    _r = r;
}

constexpr bool TColorBIOS::getGreen() const noexcept
{
    return _g;
}

constexpr void TColorBIOS::setGreen(bool g) noexcept
{
    _g = g;
}

constexpr bool TColorBIOS::getBlue() const noexcept
{
    return _b;
}

constexpr void TColorBIOS::setBlue(bool b) noexcept
{
    _b = b;
}

constexpr bool TColorBIOS::getIntensity() const noexcept
{
    return _i;
}

constexpr void TColorBIOS::setIntensity(bool i) noexcept
{
    _i = i;
}

constexpr TColorBIOS::operator uint8_t() const noexcept
{
    return ((_i << 1 | _r) << 1 | _g) << 1 | _b;
}

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  class TColorXTerm                                                      */
/*                                                                         */
/*  Index into an 256-color palette recognized by the 'xterm-256color'     */
/*  terminal type. Some terminal emulators support modifying the palette,  */
/*  but Turbo Vision does not make use of this feature.                    */
/*                                                                         */
/*  - Indices 0-15 are the basic ANSI colors. They will be displayed       */
/*    according to the terminal emulator's color scheme.                   */
/*  - Indices 16-231 are a 6x6x6 RGB cube.                                 */
/*  - Indices 232-255 are a 24-step grayscale ramp.                        */
/*                                                                         */
/*  This type can be converted from and into integer types:                */
/*      TColorXTerm xterm = 0xFE;                                          */
/*      uint8_t asChar = xterm;                                            */
/*                                                                         */
/*-------------------------------------------------------------------------*/

class TColorXTerm
{
public:

    constexpr TColorXTerm() noexcept;
    constexpr TColorXTerm(uint8_t idx) noexcept;
    constexpr operator uint8_t() const noexcept;

private:

    uint8_t _idx;
};

constexpr TColorXTerm::TColorXTerm() noexcept :
    TColorXTerm(0)
{
}

constexpr TColorXTerm::TColorXTerm(uint8_t idx) noexcept :
    _idx(idx)
{
}

constexpr TColorXTerm::operator uint8_t() const noexcept
{
    return _idx;
}

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  class TColorDefault                                                    */
/*                                                                         */
/*  Represents the terminal default color. This is the color used by       */
/*  terminal emulators when no display attributes (bold, color...) are     */
/*  enabled (usually white for foreground and black for background).       */
/*                                                                         */
/*-------------------------------------------------------------------------*/

class TColorDefault
{
public:

    TColorDefault() = default;
};

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  Color Conversion Functions                                             */
/*                                                                         */
/*  They convert between some of the aforementioned color types            */
/*                                                                         */
/*  No conversion from XTerm indices 0-15 to RGB is provided because the   */
/*  way these colors are displayed depends on the terminal emulator,       */
/*  which usually allow users to configure these colors through color      */
/*  schemes.                                                               */
/*                                                                         */
/*  Conversely, a conversion from RGB to XTerm indices 0-15 is provided,   */
/*  but it's an approximation based on color hue.                          */
/*                                                                         */
/*  When it comes to XTerm indices 16-255, we assume that they can be      */
/*  mapped to the same RGB values used by XTerm:                           */
/*  https://invisible-island.net/xterm/xterm.faq.html#what_256colors       */
/*  https://jonasjacek.github.io/colors/                                   */
/*                                                                         */
/*-------------------------------------------------------------------------*/

class TColorConversion
{
public:

    static constexpr TColorXTerm BIOStoXTerm16(TColorBIOS) noexcept;
    static constexpr TColorBIOS XTerm16toBIOS(TColorXTerm) noexcept; // XTerm indices 0-15
    static TColorXTerm XTerm256toXTerm16(TColorXTerm) noexcept; // XTerm indices 16-255 → 0-15
    static TColorRGB XTerm256toRGB(TColorXTerm) noexcept; // XTerm indices 16-255
    static TColorXTerm RGBtoXTerm16(TColorRGB) noexcept; // XTerm indices 0-15
    static TColorXTerm RGBtoXTerm256(TColorRGB) noexcept; // XTerm indices 16-255

private:

    friend class TColorConversionInit;

    template <class T, size_t N>
    struct LUT { T entries[N]; };

    static const LUT<TColorXTerm, 256> XTerm256toXTerm16LUT;
    static const LUT<TColorRGB, 256> XTerm256toRGBLUT;
};

constexpr TColorXTerm TColorConversion::BIOStoXTerm16(TColorBIOS c) noexcept
{
    // Swap the Red and Blue bits.
    auto aux = c.getBlue();
    c.setBlue(c.getRed());
    c.setRed(aux);
    return TColorXTerm(c);
}

constexpr TColorBIOS TColorConversion::XTerm16toBIOS(TColorXTerm idx) noexcept
{
    return (uint8_t) BIOStoXTerm16((uint8_t) idx);
}

inline TColorXTerm TColorConversion::XTerm256toXTerm16(TColorXTerm idx) noexcept
{
    return XTerm256toXTerm16LUT.entries[idx];
}

inline TColorRGB TColorConversion::XTerm256toRGB(TColorXTerm idx) noexcept
{
    return XTerm256toRGBLUT.entries[idx];
}

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  class TColor                                                           */
/*                                                                         */
/*  Represents a color in any of the supported color types. It can be      */
/*  initialized with a TColorBIOS, TColorRGB, TColorXTerm or               */
/*  TColorDefault object.                                                  */
/*                                                                         */
/*  It also can be initialized as a BIOS color with a char literal; as an  */
/*  RGB color with an integer literal; or as the terminal default color    */
/*  through default-initialization or value-initialization:                */
/*      TColor bios = '\xF',                                               */
/*             rgb  = 0x7F00BB,                                            */
/*             def; // or 'def = {}'.                                      */
/*                                                                         */
/*  TColor is trivially copyable: 'memcpy' can be used to copy objects.    */
/*                                                                         */
/*-------------------------------------------------------------------------*/

class TColor
{
public:

    constexpr TColor() noexcept; // Default color.

    // Constructors for use with literals.

    constexpr TColor(char bios) noexcept; // e.g. {'\xF'}
    constexpr TColor(int rgb) noexcept; // e.g. {0x7F00BB}

    // Constructors for specific color types.

    constexpr TColor(TColorBIOS bios) noexcept;
    constexpr TColor(TColorRGB rgb) noexcept;
    constexpr TColor(TColorXTerm xterm) noexcept;
    constexpr TColor(TColorDefault) noexcept;

    // Color type getters.

    constexpr bool isDefault() const noexcept;
    constexpr bool isBIOS() const noexcept;
    constexpr bool isRGB() const noexcept;
    constexpr bool isXTerm() const noexcept;

    // Color value getters. They perform no conversion: make sure to check
    // the color type first.

    constexpr TColorBIOS asBIOS() const noexcept;
    constexpr TColorRGB asRGB() const noexcept;
    constexpr TColorXTerm asXTerm() const noexcept;

    // Quantization to TColorBIOS.

    TColorBIOS toBIOS(bool isForeground) const noexcept;

    constexpr bool operator==(TColor other) const noexcept;
    constexpr bool operator!=(TColor other) const noexcept;

private:

    // Bit  0: Color (24 bits)
    // Bit 24: Type (8 bits, of which only 2 are actually used)
    uint32_t _data;

    friend struct TColorAttr;

    enum : uint8_t
    {
        ctDefault = 0x0,
        ctBIOS    = 0x1,
        ctRGB     = 0x2,
        ctXTerm   = 0x3,
    };

    constexpr uint8_t type() const noexcept;
};

constexpr TColor::TColor() noexcept :
    _data(0)
{
}

constexpr TColor::TColor(char bios) noexcept :
    _data(uchar(bios & 0xF) | (ctBIOS << 24))
{
}

constexpr TColor::TColor(int rgb) noexcept :
    _data((rgb & 0xFFFFFF) | (ctRGB << 24))
{
}

constexpr TColor::TColor(TColorBIOS bios) noexcept :
    TColor((char) bios)
{
}

constexpr TColor::TColor(TColorRGB rgb) noexcept :
    TColor((int) rgb)
{
}

constexpr TColor::TColor(TColorXTerm xterm) noexcept :
    _data(xterm | (ctXTerm << 24))
{
}

constexpr TColor::TColor(TColorDefault) noexcept :
    TColor()
{
}

constexpr bool TColor::isDefault() const noexcept
{
    return type() == ctDefault;
}

constexpr bool TColor::isBIOS() const noexcept
{
    return type() == ctBIOS;
}

constexpr bool TColor::isRGB() const noexcept
{
    return type() == ctRGB;
}

constexpr bool TColor::isXTerm() const noexcept
{
    return type() == ctXTerm;
}

constexpr TColorBIOS TColor::asBIOS() const noexcept
{
    return uint8_t(_data);
}

constexpr TColorRGB TColor::asRGB() const noexcept
{
    return _data;
}

constexpr TColorXTerm TColor::asXTerm() const noexcept
{
    return uint8_t(_data);
}

constexpr bool TColor::operator==(TColor other) const noexcept
{
    return _data == other._data;
}

constexpr bool TColor::operator!=(TColor other) const noexcept
{
    return _data != other._data;
}

constexpr uint8_t TColor::type() const noexcept
{
    return _data >> 24;
}

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  struct TColorAttr                                                      */
/*                                                                         */
/*  Represents the colors and style used when writing text to the screen.  */
/*                                                                         */
/*  TColorAttr is designed to be compatible with 'memset' and 'memcpy',    */
/*  and it is therefore trivially constructible and copyable. Because of   */
/*  this, variables of this type which are not explicitly initialized      */
/*  will contain uninitialized data. Watch out!                            */
/*                                                                         */
/*  A zero-initialized TColorAttr (using 'memset' or value-initialization) */
/*  has both the foreground and background colors set to 'default' and no  */
/*  style. Therefore, a zero-initialized TColorAttr is valid and produces  */
/*  visible text.                                                          */
/*                                                                         */
/*  Examples:                                                              */
/*                                                                         */
/*      // Foreground: BIOS 0x7                                            */
/*      // Background: RGB 0x7F00BB                                        */
/*      // Style: Bold, Italic                                             */
/*      TColorAttr a = {'\x07', 0x7F00BB, slBold | slItalic};              */
/*                                                                         */
/*      // Foreground: Default                                             */
/*      // Background: BIOS 0xF                                            */
/*      // Style: Normal                                                   */
/*      TColorAttr b = {{}, '\xF'};                                        */
/*                                                                         */
/*      // Foreground: Default                                             */
/*      // Background: Default                                             */
/*      // Style: Normal                                                   */
/*      TColorAttr c {};                                                   */
/*                                                                         */
/*      // An uninitialized TColorAttr. Watch out!                         */
/*      TColorAttr d;                                                      */
/*                                                                         */
/*  For backward-compatibility, you can also use initialize a TColorAttr   */
/*  with a BIOS color attribute:                                           */
/*                                                                         */
/*      // Foreground: BIOS 0xD                                            */
/*      // Background: BIOS 0x3                                            */
/*      // Style: Normal                                                   */
/*      TColorAttr e = 0x3D;                                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/

const ushort

// TColorAttr Style masks

    slBold         = 0x001,
    slItalic       = 0x002,
    slUnderline    = 0x004,
    slBlink        = 0x008,
    slReverse      = 0x010, // Prefer using 'reversed()' instead.
    slStrike       = 0x020,

// Private masks (used internally by TV)

    slWindowShadow = 0x200;

class TAttrPair;

struct TColorAttr
{
    TColorAttr() = default; // Watch out! This is a trivial constructor.

    constexpr TColorAttr(int bios) noexcept;
    constexpr TColorAttr(TColor fg, TColor bg, ushort style = 0) noexcept;

    constexpr TColorAttr(const TAttrPair &attrs) noexcept;

    constexpr TColor getForeground() const noexcept;
    constexpr void setForeground(TColor fg) noexcept;
    constexpr TColor getBackground() const noexcept;
    constexpr void setBackground(TColor bg) noexcept;
    constexpr ushort getStyle() const noexcept;
    constexpr void setStyle(ushort aStyle) noexcept;

    constexpr TColorAttr reversed() const noexcept;

    // Convert to a BIOS color attribute, using quantization if necessary.
    // Style flags are ignored.

    uchar toBIOS() const noexcept;

    // Backward-compatibility functions:

    // Return the corresponding BIOS color attribute when both the foreground
    // and background are BIOS colors, and there are no style flags. Otherwise,
    // return a fixed color (white on magenta) indicating that the cast is invalid.
    constexpr operator uchar() const noexcept;
    // Used to compose attribute pairs in legacy code.
    constexpr TAttrPair operator<<(int shift) const noexcept;

    constexpr bool operator==(TColorAttr other) const noexcept;
    constexpr bool operator!=(TColorAttr other) const noexcept;
    // Explicitly overload the comparison against int to avoid ambiguity.
    constexpr bool operator==(int bios) const noexcept;
    constexpr bool operator!=(int bios) const noexcept;

private:

    // Bit field implemented manually so that copy and comparison operations
    // can both get optimized by the compiler and be constexpr:
    // Bit 0: Foreground (27 bits)
    // Bit 27: Background (27 bits)
    // Bit 54: Style (10 bits)
    uint64_t _data;

    enum : uint64_t
    {
        fgMask = ((1ULL << 27) - 1),
        bgMask = ((1ULL << 27) - 1),
        styleMask = ((1ULL << 10) - 1),
    };
};

constexpr TColorAttr::TColorAttr(int bios) noexcept :
    TColorAttr(
        TColor(char(bios)),
        TColor(char(bios >> 4))
    )
{
}

constexpr TColorAttr::TColorAttr(TColor fg, TColor bg, ushort style) noexcept :
    _data(
        (fg._data & fgMask)
        | ((bg._data & bgMask) << 27)
        | (uint64_t(style) << 54)
    )
{
}

constexpr TColor TColorAttr::getForeground() const noexcept
{
    TColor color {};
    color._data = _data & fgMask;
    return color;
}

constexpr void TColorAttr::setForeground(TColor fg) noexcept
{
    _data = (_data & ~fgMask) | (fg._data & fgMask);
}

constexpr TColor TColorAttr::getBackground() const noexcept
{
    TColor color {};
    color._data = (_data >> 27) & bgMask;
    return color;
}

constexpr void TColorAttr::setBackground(TColor bg) noexcept
{
    _data = (_data & ~(bgMask << 27)) | ((bg._data & bgMask) << 27);
}

constexpr ushort TColorAttr::getStyle() const noexcept
{
    return _data >> 54;
}

constexpr void TColorAttr::setStyle(ushort aStyle) noexcept
{
    _data = (_data & ~(styleMask << 54)) | (uint64_t(aStyle) << 54);
}

constexpr TColorAttr TColorAttr::reversed() const noexcept
{
    auto fg = getForeground(),
         bg = getBackground();
    // The 'slReverse' attribute may be represented differently by different
    // terminal emulators, so it is better to swap the colors manually unless
    // either of them is the default color.
    if (fg.isDefault() || bg.isDefault())
        return TColorAttr(fg, bg, getStyle() ^ slReverse);
    else
        return TColorAttr(bg, fg, getStyle());
}

constexpr TColorAttr::operator uchar() const noexcept
{
    // 'this' must be a BIOS attribute, or else a fixed color will be returned
    // instead. The key point is that the result shouldn't be '\x0' unless this
    // is the BIOS attribute '\x0', so that legacy code comparing a TColorAttr
    // against '0' keeps working.
    auto fg = getForeground(),
         bg = getBackground();
    if (fg.isBIOS() && bg.isBIOS() && getStyle() == 0)
        return fg.asBIOS() | (bg.asBIOS() << 4);
    return 0x5F;
}

constexpr bool TColorAttr::operator==(TColorAttr other) const noexcept
{
    return _data == other._data;
}

constexpr bool TColorAttr::operator!=(TColorAttr other) const noexcept
{
    return !(*this == other);
}

constexpr bool TColorAttr::operator==(int bios) const noexcept
{
    return *this == TColorAttr((uchar) bios);
}

constexpr bool TColorAttr::operator!=(int bios) const noexcept
{
    return !(*this == bios);
}

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  class TAttrPair                                                        */
/*                                                                         */
/*  Represents a pair of color attributes.                                 */
/*                                                                         */
/*  Example:                                                               */
/*                                                                         */
/*      TColorAttr cNormal = {0x234983, 0x267232};                         */
/*      TColorAttr cHigh = {0x309283, 0x127844};                           */
/*      TAttrPair attrs = {cNormal, cHigh};                                */
/*      assert(attrs[0] == cNormal);                                       */
/*      assert(attrs[1] == cHigh);                                         */
/*      TDrawBuffer b;                                                     */
/*      b.moveCStr(0, "Normal text, ~Highlighted text~", attrs);           */
/*                                                                         */
/*-------------------------------------------------------------------------*/


class TAttrPair
{
public:

    constexpr TAttrPair(TColorAttr low = 0x00, TColorAttr high = 0x00) noexcept;
    constexpr TAttrPair(int bios) noexcept;

    // TAttrPair can be used as a 2-element array ([0]: low, [1]: high).
    // Array reference conversion operators are used instead of 'operator[]'
    // since they enable compile-time diagnostics based on the array length.
    using ArrayRef = TColorAttr (&)[2];
    constexpr operator ArrayRef() noexcept;
    using ConstArrayRef = const TColorAttr (&)[2];
    constexpr operator ConstArrayRef() const noexcept;

    // Backward-compatibility functions:

    // Return the corresponding pair of BIOS color attributes. Only works
    // properly if both attributes are actually BIOS color attributes:
    // see 'TColorAttr::operator uchar()'.
    constexpr operator ushort() const noexcept;
    // Used to compose attribute pairs in legacy code.
    constexpr TAttrPair operator>>(int shift) const noexcept;
    constexpr TAttrPair& operator|=(TColorAttr attr) noexcept;

private:

    TColorAttr _attrs[2];
};

constexpr TAttrPair::TAttrPair(TColorAttr low, TColorAttr high) noexcept :
    _attrs {low, high}
{
}

constexpr TAttrPair::TAttrPair(int bios) noexcept :
    _attrs {uchar(bios & 0xFF), uchar(bios >> 8)}
{
}

constexpr TAttrPair::operator TAttrPair::ArrayRef() noexcept
{
    return _attrs;
}

constexpr TAttrPair::operator TAttrPair::ConstArrayRef() const noexcept
{
    return _attrs;
}

constexpr TAttrPair::operator ushort() const noexcept
{
    return uchar(_attrs[0]) | (uchar(_attrs[1]) << 8);
}

constexpr TAttrPair TAttrPair::operator>>(int shift) const noexcept
{
    // Legacy code may use '>> 8' on an attribute pair to get the higher attribute.
    if (shift == 8)
        return {_attrs[1]};
    return ushort(*this) >> shift;
}

constexpr TAttrPair& TAttrPair::operator|=(TColorAttr attr) noexcept
{
    // Legacy code may use '|=' on an attribute pair to set the lower attribute.
    // However, that's only if the lower attribute is equivalent to the BIOS
    // color attribute 0x00. Otherwise, this is just an arithmetic operation.
    if (_attrs[0] == 0)
        _attrs[0] = attr;
    else
        _attrs[0] = uchar(_attrs[0]) | uchar(attr);
    return *this;
}

// Pending methods from TColorAttr.

constexpr TColorAttr::TColorAttr(const TAttrPair &attrs) noexcept :
    TColorAttr(attrs[0])
{
}

constexpr TAttrPair TColorAttr::operator<<(int shift) const noexcept
{
    // Legacy code may use '<< 8' on an attribute to construct an attribute pair.
    if (shift == 8)
        return {uchar(0), *this};
    return uchar(*this) << shift;
}

#endif // __BORLANDC__

#endif // TVISION_COLORS_H

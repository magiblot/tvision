/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   SCRNCELL.H                                                            */
/*                                                                         */
/*   Defines the structs TScreenCharacter and TScreenCell.                 */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef TVISION_SCRNCELL_H
#define TVISION_SCRNCELL_H

#if defined( __BORLANDC__ )

// Struct that replicates the memory layout of a DOS text mode screen cell,
// with optimized custom operators for converting to and from unsigned short.

struct TScreenCell
{
    char character;
    TColorAttr attribute;

    TScreenCell& operator=(const TScreenCell &other);
    TScreenCell& operator=(ushort value);
    operator ushort() const;
};

inline TScreenCell& TScreenCell::operator=(const TScreenCell &other)
{
    (ushort &) *this = (const ushort &) other;
    return *this;
}

inline TScreenCell& TScreenCell::operator=(ushort value)
{
    (ushort &) *this = value;
    return *this;
}

inline TScreenCell::operator ushort() const
{
    return (const ushort &) *this;
}

#else

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  struct TScreenCharacter                                                */
/*                                                                         */
/*  Represents text in a screen cell. You should usually not need to       */
/*  interact with this directly. In order to write text into a screen      */
/*  cell, just use the functions in the TText namespace.                   */
/*                                                                         */
/*  A TScreenCharacter stores one of the following:                        */
/*    1. A single byte of ASCII or 'extended ASCII' text (1 column wide).  */
/*    2. Up to 15 bytes of UTF-8 text (1 or 2 columns wide in total,       */
/*       so it cannot contain just zero-width characters).                 */
/*    3. A special value that marks it as wide char trail.                 */
/*                                                                         */
/*  In order for a double-width character to be displayed entirely, its    */
/*  cell must be followed by another containing a wide char trail. If it   */
/*  does not, or if a wide char trail is not preceded by a double-width    */
/*  character, it will be assumed that a double-width character is being   */
/*  partially overlapped.                                                  */
/*                                                                         */
/*  TScreenCharacter is designed to be compatible with 'memset' and        */
/*  'memcpy', and it is therefore trivially constructible and copyable.    */
/*  Because of this, variables of this type which are not explicitly       */
/*  initialized will contain uninitialized data. Watch out!                */
/*                                                                         */
/*  A zero-initialized TScreenCharacter (using 'memset' or                 */
/*  value-initialization) is valid and represents the text of an empty     */
/*  screen cell.                                                           */
/*                                                                         */
/*-------------------------------------------------------------------------*/

struct TScreenCharacter
{
    TScreenCharacter() = default; // Watch out! This is a trivial constructor.

    constexpr TScreenCharacter(char c) noexcept;

    constexpr void initWithChar(char c) noexcept;
    constexpr void initWithMultiByteChar(uint32_t mbc, bool wide = false) noexcept;
    constexpr void initWithMultiByteChar(TStringView mbc, bool wide = false) noexcept;
    constexpr void initAsWideCharTrail() noexcept;

    constexpr bool isWide() const noexcept;
    constexpr bool isWideCharTrail() const noexcept;
    constexpr void appendZeroWidthChar(TStringView mbc) noexcept;
    constexpr TStringView getText() const noexcept;

private:

    enum : uint8_t
    {
        fWide     = 0x1,
        fTrail    = 0x2,
        fOverflow = 0x4,
    };

    char _text[15];
    uint8_t
        // There is always at least one character, even if it is a null byte.
        _textLengthMinusOne : 4,
        _flags : 4;
};

constexpr TScreenCharacter::TScreenCharacter(char c) noexcept :
    _text {c},
    _textLengthMinusOne(0),
    _flags(0)
{
}

constexpr void TScreenCharacter::initWithChar(char c) noexcept
{
    *this = {};
    _text[0] = c;
}

constexpr void TScreenCharacter::initWithMultiByteChar(uint32_t mbc, bool wide) noexcept
// Pre: 'mbc' is a bit-casted multibyte-encoded character.
{
    *this = {};
#if !defined( TV_BIG_ENDIAN )
    _text[0] = char(mbc);
    _text[1] = char(mbc >> 8);
    _text[2] = char(mbc >> 16);
    _text[3] = char(mbc >> 24);

    _textLengthMinusOne =
        (mbc > 0x000000FFU) +
        (mbc > 0x0000FFFFU) +
        (mbc > 0x00FFFFFFU);
#else
    _text[0] = char(mbc >> 24);
    _text[1] = char(mbc >> 16);
    _text[2] = char(mbc >> 8);
    _text[3] = char(mbc);

    _textLengthMinusOne =
        ((mbc & 0x00FFFFFFU) != 0) +
        ((mbc & 0x0000FFFFU) != 0) +
        ((mbc & 0x000000FFU) != 0);
#endif
    _flags = -int(wide) & fWide;
}

constexpr void TScreenCharacter::initWithMultiByteChar(TStringView mbc, bool wide) noexcept
{
    static_assert(sizeof(_text) >= maxCharSize, "");
    *this = {};
    if (0 < mbc.size() && mbc.size() <= maxCharSize)
    {
        _flags |= -int(wide) & fWide;
        _textLengthMinusOne = mbc.size() - 1;
        switch (_textLengthMinusOne)
        {
            case 3: _text[3] = mbc[3];
            case 2: _text[2] = mbc[2];
            case 1: _text[1] = mbc[1];
            case 0: _text[0] = mbc[0];
        }
    }
}

constexpr void TScreenCharacter::initAsWideCharTrail() noexcept
{
    *this = {};
    _flags = fTrail;
}

constexpr bool TScreenCharacter::isWide() const noexcept
{
    return _flags & fWide;
}

constexpr bool TScreenCharacter::isWideCharTrail() const noexcept
{
    return _flags & fTrail;
}

constexpr void TScreenCharacter::appendZeroWidthChar(TStringView mbc) noexcept
// Pre: '_text' contains valid UTF-8 text.
{
    if (_flags & fOverflow)
        return;
    size_t sz = _textLengthMinusOne + 1;
    if (mbc.size() <= sizeof(_text) - sz)
    {
        if (_text[0] == '\0')
            _text[0] = ' ';
        switch (mbc.size())
        {
            case 4: _text[sz + 3] = mbc[3];
            case 3: _text[sz + 2] = mbc[2];
            case 2: _text[sz + 1] = mbc[1];
            case 1: _text[sz] = mbc[0];
        }
        _textLengthMinusOne += mbc.size();
    }
    else
        _flags |= fOverflow;
}

constexpr TStringView TScreenCharacter::getText() const noexcept
// Pre: This is not a wide char trail.
{
    return {_text, size_t(_textLengthMinusOne + 1)};
}

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  struct TScreenCell                                                     */
/*                                                                         */
/*  Stores the text and color attributes in a screen cell. You should      */
/*  usually not need to interact with this directly. In order to write     */
/*  text into a screen cell, just use the functions in the TText           */
/*  namespace.                                                             */
/*                                                                         */
/*  TScreenCell is designed to be compatible with 'memset' and 'memcpy',   */
/*  and it is therefore trivially constructible and copyable. Because of   */
/*  this, variables of this type which are not explicitly initialized      */
/*  will contain uninitialized data. Watch out!                            */
/*                                                                         */
/*  A zero-initialized TScreenCell (using 'memset' or                      */
/*  value-initialization) is valid and represents an empty screen cell     */
/*  with default foreground and background colors and no style.            */
/*                                                                         */
/*-------------------------------------------------------------------------*/

struct TScreenCell
{
    TScreenCharacter character;
    TColorAttr attribute;

    TScreenCell() = default; // Watch out! This is a trivial constructor.
    constexpr TScreenCell(const TScreenCharacter &ch, TColorAttr attr) noexcept;
    constexpr TScreenCell(ushort bios) noexcept;

    bool operator==(const TScreenCell &other) const;
    bool operator!=(const TScreenCell &other) const;
};

constexpr TScreenCell::TScreenCell(const TScreenCharacter &ch, TColorAttr attr) noexcept :
    character(ch),
    attribute(attr)
{
}

constexpr TScreenCell::TScreenCell(ushort bios) noexcept :
    character(char(bios)),
    attribute(uchar(bios >> 8))
{
}

inline bool TScreenCell::operator==(const TScreenCell &other) const
{
    return memcmp(this, &other, sizeof(*this)) == 0;
}

inline bool TScreenCell::operator!=(const TScreenCell &other) const
{
    return !(*this == other);
}

#endif // __BORLANDC__

#endif // TVISION_SCRNCELL_H

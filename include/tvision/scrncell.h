/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   SCRNCELL.H                                                            */
/*                                                                         */
/*   Defines the low level structs used to represent text and attributes   */
/*   on the screen, most notably TCellAttribs and TScreenCell.             */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef TVISION_SCRNCELL_H
#define TVISION_SCRNCELL_H

#ifdef __BORLANDC__

typedef ushort TScreenCell;

#else

#include <cstring>
#include <type_traits>

struct alignas(uint8_t) TCellAttribs
{

    uint8_t
        fgBlue      : 1,
        fgGreen     : 1,
        fgRed       : 1,
        fgBright    : 1,
        bgBlue      : 1,
        bgGreen     : 1,
        bgRed       : 1,
        bgBright    : 1;

    TCellAttribs() = default;

    TCellAttribs(uint8_t asChar)
    {
        *this = asChar;
    }

    uint8_t operator=(uint8_t asChar)
    {
        // Aliasing through uchar pointers is allowed.
        *(uint8_t *) this = asChar;
        return asChar;
    }

    operator uint8_t() const
    {
        return *(uint8_t *) this;
    }

    operator uint8_t&()
    {
        return *(uint8_t *) this;
    }

    uint8_t fgGet() const
    {
        return uint8_t(*this) & 0x0F;
    }

    uint8_t bgGet() const
    {
        return uint8_t(*this) >> 4;
    }

    void fgSet(uint8_t fg)
    {
        *this = (bgGet() << 4) | (fg & 0x0F);
    }

    void bgSet(uint8_t bg)
    {
        *this = (bg << 4) | fgGet();
    }

private:

    static void check_assumptions()
    {
        static_assert(std::is_trivial<TCellAttribs>());
        static_assert(std::is_standard_layout<TCellAttribs>());
        static_assert(sizeof(TCellAttribs) == sizeof(uint8_t));
    }

};

struct alignas(uint16_t) TScreenCellA
{
    // This struct has the same layout as TScreenCell on Borland C++ platforms.
    // However, TScreenCell is defined as ushort in Borland C++ as that is what
    // existing code relies on.

    uint8_t Char;
    TCellAttribs Attr;

    TScreenCellA() = default;

    TScreenCellA(uint16_t asShort)
    {
        *this = asShort;
    }

    uint16_t operator=(uint16_t asShort)
    {
        memcpy(this, &asShort, sizeof(*this));
        return asShort;
    }

    operator uint16_t() const
    {
        uint16_t asShort;
        memcpy(&asShort, this, sizeof(*this));
        return asShort;
    }

private:

    static void check_assumptions()
    {
        static_assert(std::is_trivial<TScreenCellA>());
        static_assert(std::is_standard_layout<TScreenCellA>());
        static_assert(sizeof(TScreenCellA) == sizeof(uint16_t));
    }

};

struct alignas(uint32_t) TCellChar
{

    uint8_t bytes[4];

    TCellChar() = default;

    TCellChar(uint32_t asInt)
    {
        *this = asInt;
    }

    uint32_t operator=(uint32_t asInt)
    {
        memcpy(this, &asInt, sizeof(*this));
        return asInt;
    }

    operator uint32_t() const
    {
        uint32_t asInt {0};
        memcpy(&asInt, this, sizeof(*this));
        return asInt;
    }

private:

    static void check_assumptions()
    {
        static_assert(std::is_trivial<TCellChar>());
        static_assert(std::is_standard_layout<TCellChar>());
        static_assert(sizeof(TCellChar) == sizeof(uint32_t));
    }

};

struct alignas(uint64_t) TScreenCell
{

    TCellChar Char;
    TCellAttribs Attr;
    uint8_t
        extraWidth : 3;

    TScreenCell() = default;

    TScreenCell(uint64_t asLong)
    {
        *this = asLong;
    }

    TScreenCell(TScreenCellA pair)
    {
        *this = 0;
        Char = pair.Char;
        Attr = pair.Attr;
    }

    uint64_t operator=(uint64_t asLong)
    {
        memcpy(this, &asLong, sizeof(*this));
        return asLong;
    }

    operator uint64_t() const
    {
        uint64_t asLong;
        memcpy(&asLong, this, sizeof(*this));
        return asLong;
    }

    static constexpr uint32_t wideCharTrail = -2U;

private:

    static void check_assumptions()
    {
        static_assert(std::is_trivial<TScreenCell>());
        static_assert(std::is_standard_layout<TScreenCell>());
        static_assert(sizeof(TScreenCell) == sizeof(uint64_t));
    }

};

#endif // __BORLANDC__

#endif

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

template<typename T>
struct alignas(T) trivially_convertible {

    trivially_convertible() = default;

    trivially_convertible(T asT)
    {
        *this = asT;
    }

    T operator=(T asT)
    {
        memcpy(this, &asT, sizeof(T));
        return asT;
    }

    operator T() const
    {
        T asT;
        memcpy(&asT, this, sizeof(T));
        return asT;
    }

protected:

    template<class C>
    static constexpr bool check_trivial()
    {
        static_assert(std::is_trivial<C>());
        static_assert(std::is_standard_layout<C>());
        static_assert(sizeof(C) == sizeof(T));
        static_assert(alignof(C) == alignof(T));
        return true;
    }

};

struct TCellAttribs : trivially_convertible<uint16_t>
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
    uint8_t
        fgDefault   : 1,
        bgDefault   : 1,
        bold        : 1,
        italic      : 1,
        underline   : 1;

    using trivially_convertible::trivially_convertible;

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

    static constexpr void check_assumptions()
    {
        check_trivial<TCellAttribs>();
    }

};

struct TScreenCellA : trivially_convertible<uint16_t>
{

    uint8_t Char;
    uint8_t Attr;

    using trivially_convertible::trivially_convertible;

    static constexpr void check_assumptions()
    {
        check_trivial<TScreenCellA>();
    }

};

struct TCellChar : trivially_convertible<uint32_t>
{

    uint8_t bytes[4];

    using trivially_convertible::trivially_convertible;

    static constexpr void check_assumptions()
    {
        check_trivial<TCellChar>();
    }

};

struct TScreenCell : trivially_convertible<uint64_t>
{

    TCellChar Char;
    TCellAttribs Attr;
    uint8_t
        extraWidth : 3;

    using trivially_convertible::trivially_convertible;

    TScreenCell(TScreenCellA pair)
    {
        *this = 0;
        Char = pair.Char;
        Attr = pair.Attr;
    }

    static constexpr uint32_t wideCharTrail = -2U;

    static constexpr void check_assumptions()
    {
        check_trivial<TScreenCell>();
    }

};

#endif // __BORLANDC__

#endif

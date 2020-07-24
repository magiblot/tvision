/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   SCRNCELL.H                                                            */
/*                                                                         */
/*   Defines the TCellAttribs and TScreenCell classes.                     */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef TVISION_SCRNCELL_H
#define TVISION_SCRNCELL_H

struct TCellAttribs
{
    union {
        uchar asChar;
        struct {
            uchar
                fg : 4,
                bg : 4;
        } colors;
        struct {
            uchar
                fgBlue      : 1,
                fgGreen     : 1,
                fgRed       : 1,
                fgBright    : 1,
                bgBlue      : 1,
                bgGreen     : 1,
                bgRed       : 1,
                bgBright    : 1;
        } bits;
    };
    // We should use std::bit_cast instead of unions when upgrading to C++20.
    operator uchar&() { return asChar; }
    operator const uchar&() const { return asChar; }
};

struct TScreenCell
{
    union {
        uint64_t asLong;
        struct {
            union {
                uint8_t bytes[4];
                uint32_t asInt;
            } Char;
            TCellAttribs Attr;
            uchar
                extraWidth : 3;
        } Cell;
    };
    static constexpr uint32_t wideCharTrail = -2U;
    static TScreenCell fromPair(ushort source)
    {
        TScreenCell c {};
        c.Cell.Char.asInt = source & 0x00FF;
        c.Cell.Attr.asChar = (source & 0xFF00) >> 8;
        return c;
    }
};

#endif

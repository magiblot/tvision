#ifndef TEXTATTR_H
#define TEXTATTR_H

#include <tvision/tv.h>

inline void swapRedBlue(TCellAttribs &c) {
    // Swap the Red and Blue bits so that each color can be
    // straightforwardly converted to an SGR color code.
    uchar fgAux = c.bits.fgBlue;
    c.bits.fgBlue = c.bits.fgRed;
    c.bits.fgRed = fgAux;
    uchar bgAux = c.bits.bgBlue;
    c.bits.bgBlue = c.bits.bgRed;
    c.bits.bgRed = bgAux;
}

// SGRAttribs conversion flags.

const uint
    sgrBrightIsBold   = 0x0001,
    sgrBrightIsBlink  = 0x0002;

struct SGRAttribs {

    union {
        uint asInt;
        struct {
            uchar fg;
            uchar bg;
            uchar bold;
            uchar blink;
        } attr;
    };

    SGRAttribs();
    SGRAttribs(TCellAttribs bios, uint flags);
    bool operator!=(SGRAttribs other) const;

};

inline SGRAttribs::SGRAttribs()
{
    attr.fg = 30;       // Black
    attr.bg = 40;       // Black
    attr.bold = 22;     // Bold Off
    attr.blink = 25;    // Blink Off
}

inline SGRAttribs::SGRAttribs(TCellAttribs c, uint flags) :
    SGRAttribs()
{
    swapRedBlue(c);
    attr.fg += (c.colors.fg & 0x07);
    attr.bg += (c.colors.bg & 0x07);
    if (c.bits.fgBright)
    {
        if (flags & sgrBrightIsBold)
            attr.bold = 1; // Bold On
        else
            attr.fg += 60;
    }
    if (c.bits.bgBright)
    {
        if (flags & sgrBrightIsBlink)
            attr.blink = 5; // Blink On
        else
            attr.bg += 60;
    }
}

inline bool SGRAttribs::operator!=(SGRAttribs other) const
{
    return asInt != other.asInt;
}

struct BufferCell {

    union {
        uint64_t asLong;
        struct {
            union {
                uint8_t bytes[4];
                uint32_t asInt;
            } Char;
            TCellAttribs Attr;
            uchar
                extraWidth  : 3,
                dirty       : 1;
        } Cell;
    };

    BufferCell();
    BufferCell(TScreenCell cell);
    bool operator!=(BufferCell other) const;
    void ensurePrintable(bool wideChars);

};

inline BufferCell::BufferCell() :
    asLong(0)
{
}

inline BufferCell::BufferCell(TScreenCell cell) :
    asLong(cell.asLong)
{
    Cell.dirty = 0;
}

inline bool BufferCell::operator!=(BufferCell other) const
{
    // Discard dirty bit and redundant/unused bits.
    return (asLong ^ other.asLong) & 0x0000'00FF'FFFF'FFFF;
}

#endif

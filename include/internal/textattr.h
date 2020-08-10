#ifndef TEXTATTR_H
#define TEXTATTR_H

#include <tvision/tv.h>

inline void swapRedBlue(TCellAttribs &c) {
    // Swap the Red and Blue bits so that each color can be
    // straightforwardly converted to an SGR color code.
    uchar fgAux = c.fgBlue;
    c.fgBlue = c.fgRed;
    c.fgRed = fgAux;
    uchar bgAux = c.bgBlue;
    c.bgBlue = c.bgRed;
    c.bgRed = bgAux;
}

// SGRAttribs conversion flags.

const uint
    sgrBrightIsBold   = 0x0001,
    sgrBrightIsBlink  = 0x0002,
    sgrNoItalic       = 0x0004,
    sgrNoUnderline    = 0x0008;

struct SGRAttribs : trivially_convertible<uint64_t> {

    uchar fg;
    uchar bg;
    uchar bold;
    uchar italic;
    uchar underline;
    uchar blink;
    uchar reverse;

    using trivially_convertible::trivially_convertible;
    SGRAttribs() = default;

    enum {defaultInit};
    SGRAttribs(decltype(defaultInit));
    SGRAttribs(TCellAttribs bios, uint flags);

    static constexpr void check_assumptions()
    {
        check_trivial<SGRAttribs>();
    }

};

inline SGRAttribs::SGRAttribs(decltype(defaultInit))
{
    *this = 0;
    fg = 30;        // Black
    bg = 40;        // Black
    bold = 22;      // Bold Off
    italic = 23;    // Italic Off
    underline = 24; // Underline Off
    blink = 25;     // Blink Off
    reverse = 27;   // Reverse Off
}

inline SGRAttribs::SGRAttribs(TCellAttribs c, uint flags) :
    SGRAttribs(defaultInit)
{
    swapRedBlue(c);
    if (c.fgDefault)
        fg = 39; // Default foreground color
    else
        fg += (c.fgGet() & 0x07);
    if (c.bgDefault)
        bg = 49; // Default background color
    else
        bg += (c.bgGet() & 0x07);
    if (c.fgBright)
    {
        if (flags & sgrBrightIsBold)
            bold = 1; // Bold On
        else
            fg += 60;
    }
    if (c.bgBright)
    {
        if (flags & sgrBrightIsBlink)
            blink = 5; // Blink On
        else
            bg += 60;
    }
    if (c.bold)
        bold = 1; // Bold On
    if (!(flags & sgrNoItalic) && c.italic)
        italic = 3; // Italic On
    if (!(flags & sgrNoUnderline) && c.underline)
        underline = 4; // Underline On
    if (c.reverse)
        reverse = 7; // Reverse On
}

struct BufferCell : trivially_convertible<uint64_t> {

    TCellChar Char;
    TCellAttribs Attr;
    uint8_t
        extraWidth  : 3,
        dirty       : 1;

    using trivially_convertible::trivially_convertible;

    BufferCell(TScreenCell cell);
    bool operator!=(BufferCell other) const;
    void ensurePrintable(bool wideChars);

};

inline BufferCell::BufferCell(TScreenCell cell)
{
    *this = (uint64_t) cell;
    dirty = 0;
}

inline bool BufferCell::operator!=(BufferCell other) const
{
    // Discard dirty bit and unused bits. extraWidth is redundant.
    return (*this ^ other) & 0x0000'FFFF'FFFF'FFFF;
}

#endif

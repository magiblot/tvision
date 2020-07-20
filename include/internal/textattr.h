#ifndef TEXTATTR_H
#define TEXTATTR_H

#include <tvision/ttypes.h>

struct BIOSColor {

    union {
        uchar asChar;
        struct {
            uchar
                fg  : 4,
                bg  : 4;
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

    constexpr BIOSColor(uchar c) : asChar(c) {}
    constexpr operator uchar&() { return asChar; }

    constexpr void swapRedBlue() {
        // Swap the Red and Blue bits so that each color can be
        // straightforwardly converted to an SGR color code.
        uchar fgAux = bits.fgBlue,
              bgAux = bits.bgBlue;
        bits.fgBlue = bits.fgRed;
        bits.fgRed = fgAux;
        bits.bgBlue = bits.bgRed;
        bits.bgRed = bgAux;
    }

};

// SGRAttribs conversion flags.

const uint
    sgrBrightIsBold   = 0x0001,
    sgrBrightIsBlink  = 0x0002;

struct SGRAttribs {

    union {
        struct {
            uchar fg;
            uchar bg;
            uchar bold;
            uchar blink;
        } attr;
        uint asInt;
    };

    SGRAttribs()
    {
        attr.fg = 30;       // Black
        attr.bg = 40;       // Black
        attr.bold = 22;     // Bold Off
        attr.blink = 25;    // Blink Off
    }

    SGRAttribs(BIOSColor bios, uint flags) :
        SGRAttribs()
    {
        bios.swapRedBlue();
        attr.fg += (bios.colors.fg & 0x07);
        attr.bg += (bios.colors.bg & 0x07);
        if (bios.bits.fgBright)
        {
            if (flags & sgrBrightIsBold)
                attr.bold = 1; // Bold On
            else
                attr.fg += 60;
        }
        if (bios.bits.bgBright)
        {
            if (flags & sgrBrightIsBlink)
                attr.blink = 5; // Blink On
            else
                attr.bg += 60;
        }
    }

    bool operator!=(SGRAttribs other)
    {
        return asInt != other.asInt;
    }

};

struct BufferCharInfo {

    union {
        struct {
            union {
                uchar character;
                ushort unicode;
            };
            uchar attr;
            uchar dirty : 1;
        };
        uint asInt;
    };

    BufferCharInfo(uint i=0) :
        asInt(i)
    {
    }

    bool operator!=(BufferCharInfo other)
    {
        return (asInt & 0x00FFFFFF) != (other.asInt & 0x00FFFFFF);
    }

};

#endif

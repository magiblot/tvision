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
        } layers;
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
        // straightforwardly converted to an ANSI color code.
        uchar fgAux = bits.fgBlue,
              bgAux = bits.bgBlue;
        bits.fgBlue = bits.fgRed;
        bits.fgRed = fgAux;
        bits.bgBlue = bits.bgRed;
        bits.bgRed = bgAux;
    }

};

constexpr inline int getANSIColorCode(uchar c, bool bg=false) {
    return ((c & 0x08) ? (bg ? 100 : 90) : (bg ? 40 : 30)) + (c & 0x07);
}

struct WinCharInfo {
    union {
        struct {
            union {
                ushort Unicode;
                uchar AsciiChar;
            } Char;
            ushort Attributes;
        };
        uint asInt;
    };
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

    BufferCharInfo(WinCharInfo info) :
        asInt(info.asInt)
    {
    }

    bool operator!=(WinCharInfo info)
    {
        return (asInt & 0x00FFFFFF) != (info.asInt & 0x00FFFFFF);
    }

};

#endif

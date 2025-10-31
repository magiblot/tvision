#ifndef TVISION_CODEPAGE_H
#define TVISION_CODEPAGE_H

#include <stdint.h>
#include <string.h>

namespace tvision
{

class CpTranslator
{
    static const char (*cpToUtf8)[256][4];

public:

    static void setTranslation(const char (*translation)[256][4]) noexcept;

    static const char (&toUtf8(unsigned char c) noexcept)[4]
    {
        return (*cpToUtf8)[c];
    }

    static uint32_t toPackedUtf8(unsigned char c) noexcept
    {
        uint32_t asInt;
        memcpy(&asInt, (*cpToUtf8)[c], sizeof(asInt));
        return asInt;
    }

    static char fromUtf8(TStringView s) noexcept;

    static char printableFromUtf8(TStringView s) noexcept
    {
        unsigned char c = fromUtf8(s);
        if (c < ' ')
            return '\0';
        return c;
    }
};

} // namespace tvision

#endif // TVISION_CODEPAGE_H

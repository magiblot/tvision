#ifndef TVISION_CODEPAGE_H
#define TVISION_CODEPAGE_H

#ifndef _TV_VERSION
#include <tvision/tv.h>
#endif

#include <tvision/internal/strings.h>

#include <unordered_map>
#include <array>

// tables.cpp

class CpTranslator
{

    CpTranslator();
    static CpTranslator instance;

    struct CpTable {
        TStringView cp;
        const uint32_t *toUtf8Int;
        const std::unordered_map<uint32_t, char> fromUtf8;

        static auto initMap(const TStringView toUtf8[256])
        {
            using namespace detail;
            std::unordered_map<uint32_t, char> map;
            for (size_t i = 0; i < 256; ++i)
                map.emplace(string_as_int<uint32_t>(toUtf8[i]), char(i));
            return map;
        }

        CpTable( TStringView cp,
                 const TStringView toUtf8[256],
                 const std::array<uint32_t, 256> &toUtf8Int ) :
            cp(cp),
            toUtf8Int(toUtf8Int.data()),
            fromUtf8(initMap(toUtf8))
        {
        }
    };
    
    static const CpTable tables[2];
    static const CpTable *activeTable;

public:

    static void use(TStringView cp)
    {
        for (const CpTable &t : tables)
            if (t.cp == cp)
            {
                activeTable = &t;
                return;
            }
        activeTable = &tables[0];
    }

    static TCellChar toUtf8Int(unsigned char c)
    {
        return activeTable->toUtf8Int[c];
    }

    static char fromUtf8(TStringView s)
    {
        using namespace detail;
        auto it = activeTable->fromUtf8.find(string_as_int<uint32_t>(s));
        if (it != activeTable->fromUtf8.end())
            return it->second;
        return 0;
    }

};

#endif // TVISION_CODEPAGE_H

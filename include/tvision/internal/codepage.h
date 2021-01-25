#ifndef TVISION_CODEPAGE_H
#define TVISION_CODEPAGE_H

#ifndef _TV_VERSION
#include <tvision/tv.h>
#endif

#include <unordered_map>
#include <array>

// tables.cpp

extern const uint32_t *tv_cp2utf8;

class CpTranslator
{

    CpTranslator();
    static CpTranslator instance;

    struct CpTable
    {
        TStringView cp;
        const uint32_t *toUtf8Int;
        const std::unordered_map<uint32_t, char> fromUtf8;

        CpTable( TStringView cp,
                 const TStringView toUtf8[256],
                 const std::array<uint32_t, 256> &toUtf8Int );
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
        tv_cp2utf8 = activeTable->toUtf8Int;
    }

    static uint32_t toUtf8Int(unsigned char c)
    {
        return activeTable->toUtf8Int[c];
    }

    static char fromUtf8(TStringView s);

};

#endif // TVISION_CODEPAGE_H

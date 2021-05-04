#ifndef TVISION_CODEPAGE_H
#define TVISION_CODEPAGE_H

#ifndef _TV_VERSION
#include <tvision/tv.h>
#endif

#include <unordered_map>
#include <array>

// tables.cpp

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

    static void useTable(const CpTable *table)
    {
        activeTable = table;
    }

public:

    static void use(TStringView cp)
    {
        for (const CpTable &t : tables)
            if (t.cp == cp)
            {
                useTable(&t);
                return; // Watch out!
            }
        useTable(&tables[0]);
    }

    static uint32_t toUtf8Int(unsigned char c)
    {
        return activeTable->toUtf8Int[c];
    }

    static char fromUtf8(TStringView s);

    static char printableFromUtf8(TStringView s)
    {
        uchar c = fromUtf8(s);
        if (c < ' ')
            return '\0';
        return c;
    }

};

#endif // TVISION_CODEPAGE_H

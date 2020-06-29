#ifndef CODEPAGE_H
#define CODEPAGE_H
#include <string_view>
#include <unordered_map>

// tables.cpp

class CpTranslator {

    CpTranslator();
    static CpTranslator instance;

    struct CpTable {
        std::string_view cp;
        const char* const* toUtf8;
        std::unordered_map<std::string_view, char> toCp;

        CpTable(std::string_view cp, const char* const* toUtf8) :
            cp(cp),
            toUtf8(toUtf8)
        {
            for (int i = 0; i < 256; ++i)
                toCp.emplace(toUtf8[i], char(i));
        };
    } static const tables[2];

    static const CpTable *activeTable;

public:

    static void use(std::string_view cp) {
        for (const CpTable &t : tables)
            if (t.cp == cp) {
                activeTable = &t;
                return;
            }
        activeTable = &tables[0];
    }

    static const char* toUtf8(unsigned char c) {
        return activeTable->toUtf8[c];
    }

    static char toCp(std::string_view s) {
        char c = 0;
        auto it = activeTable->toCp.find(s);
        if (it != activeTable->toCp.end())
            c = it->second;
        return c;
    }

};

#endif

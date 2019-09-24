#ifndef CODEPAGE_H
#define CODEPAGE_H
#include <string>
#include <unordered_map>

// tables.cpp

class CpTranslator {

    CpTranslator();
    static CpTranslator instance;

    struct CpTable {
        const int cp;
        const char** toUtf8;
        std::unordered_map<std::string, char> toCp;
    } static tables[1];

    static const CpTable *activeTable;

public:

    static void use(int cp) {
        bool valid = false;
        for (const CpTable &t : tables) {
            if (t.cp == cp) {
                valid = true;
                activeTable = &t;
            }
            break;
        }
        if (!valid)
            activeTable = &tables[0];
    }

    static const char* toUtf8(unsigned char c) {
        return activeTable->toUtf8[c];
    }

    static char toCp(const std::string &s) {
        char c = 0;
        auto it = activeTable->toCp.find(s);
        if (it != activeTable->toCp.end())
            c = it->second;
        return c;
    }

};

#endif

#ifndef UTF8_H
#define UTF8_H
#include <string>
#include <unordered_map>

// tables.cpp
extern const char* cp437toUtf8[256];
extern std::unordered_map<std::string, char> Utf8toCp437;

static inline int Utf8BytesLeft(char first_byte)
{
    // https://en.wikipedia.org/wiki/UTF-8
    return (first_byte & 0b11100000) == 0b11000000 ? 1 : \
           (first_byte & 0b11110000) == 0b11100000 ? 2 : \
           (first_byte & 0b11111000) == 0b11110000 ? 3 : 0;
}

#endif

#ifndef UTF8_H
#define UTF8_H
#include <string_view>
#include <array>
#include <cstdint>
#include <cstring>

inline constexpr int Utf8BytesLeft(char first_byte)
{
    // https://en.wikipedia.org/wiki/UTF-8
    return (first_byte & 0b11100000) == 0b11000000 ? 1 : \
           (first_byte & 0b11110000) == 0b11100000 ? 2 : \
           (first_byte & 0b11111000) == 0b11110000 ? 3 : 0;
}

template<size_t N>
inline constexpr std::array<uint32_t, N> make_utf8int(const std::string_view utf8[N]) {
    std::array<uint32_t, N> result {};
    for (size_t i = 0; i < N; ++i) {
        uint8_t chars[4] {0};
        for (size_t j = 0; j < utf8[i].size(); ++j)
            chars[j] = (uint8_t) utf8[i][j];
        // This won't work in big endian systems.
        result[i] = (chars[3] << 24) | (chars[2] << 16) | (chars[1] << 8) | chars[0];
    }
    return result;
}

inline constexpr uint32_t utf8To32(std::string_view s) {
    // Precondition: s is a valid UTF-8 sequence.
    switch (s.size()) {
        case 1:
            return s[0];
        case 2:
            return ((s[0] & 0b0001'1111) << 6)  |  (s[1] & 0b0011'1111);
        case 3:
            return ((s[0] & 0b0000'1111) << 12) | ((s[1] & 0b0011'1111) << 6)  |  (s[2] & 0b0011'1111);
        case 4:
            return ((s[0] & 0b0000'1111) << 18) | ((s[1] & 0b0011'1111) << 12) | ((s[2] & 0b0011'1111) << 6) | (s[3] & 0b0011'1111);
    }
    return 0;
}

inline constexpr size_t utf32To8(uint32_t u, char utf8[4]) {
    union {
        uint32_t asInt {0};
        uint8_t asChars[4];
    };
    if (u <= 0x007F) {
        asInt = u;
        memcpy(&utf8[0], asChars, 4);
        return 1;
    } else if (u <= 0x07FF) {
        asChars[1] =  (u        & 0b0011'1111) | 0b1000'0000;
        asChars[0] = ((u >> 6)  & 0b0001'1111) | 0b1100'0000;
        memcpy(&utf8[0], asChars, 4);
        return 2;
    } else if (u <= 0xFFFF) {
        asChars[2] =  (u        & 0b0011'1111) | 0b1000'0000;
        asChars[1] = ((u >> 6)  & 0b0011'1111) | 0b1000'0000;
        asChars[0] = ((u >> 12) & 0b0000'1111) | 0b1110'0000;
        memcpy(&utf8[0], asChars, 4);
        return 3;
    } else {
        asChars[3] =  (u        & 0b0011'1111) | 0b1000'0000;
        asChars[2] = ((u >> 6)  & 0b0011'1111) | 0b1000'0000;
        asChars[1] = ((u >> 12) & 0b0011'1111) | 0b1000'0000;
        asChars[0] = ((u >> 18) & 0b0000'0111) | 0b1111'0000;
        memcpy(&utf8[0], asChars, 4);
        return 4;
    }
}

#endif

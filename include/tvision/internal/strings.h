#ifndef TVISION_STRINGS_H
#define TVISION_STRINGS_H

#ifndef _TV_VERSION
#include <tvision/tv.h>
#endif

#include <algorithm>

namespace detail
{

template<class Int>
inline constexpr Int string_as_int(TStringView s)
{
    Int res = 0;
    for (size_t i = 0; i < std::min(s.size(), sizeof(res)); ++i)
        // CAUTION: Assumes Little Endian.
        res |= uint64_t(uint8_t(s[i])) << 8*i;
    return res;
}

// NOTE: 'buffer' is not left null-terminated.
size_t fast_utoa(uint32_t value, char *buffer);

template <class T, size_t N>
struct constarray;

struct alignas(4) btoa_lut_elem_t
{
    char chars[3];
    uint8_t digits;
};

using btoa_lut_t = constarray<btoa_lut_elem_t, 256>;

// NOTE: 'buffer' is not left null-terminated.
inline size_t fast_btoa(uint8_t value, char *buffer)
{
    extern const btoa_lut_t btoa_lut;
    const auto &lut = (btoa_lut_elem_t (&) [256]) btoa_lut;
    // CAUTION: Assumes Little Endian.
    // We can afford to write more bytes into 'buffer' than digits.
    uint32_t asInt;
    memcpy(&asInt, &lut[value], 4);
    memcpy(buffer, &asInt, 4);
    return asInt >> 24;
    static_assert(sizeof(btoa_lut_elem_t) == 4, "");
}

} // namespace detail

#endif // TVISION_STRINGS_H

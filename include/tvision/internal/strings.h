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
        // This won't work in big endian systems.
        res |= uint64_t(uint8_t(s[i])) << 8*i;
    return res;
}

uint32_t fast_utoa( uint32_t value, char *buffer );

} // namespace detail

#endif // TVISION_STRINGS_H

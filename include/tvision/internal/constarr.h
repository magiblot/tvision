#ifndef TVISION_CONSTARR_H
#define TVISION_CONSTARR_H

#include <stddef.h>

// std::array is not constexpr until C++17. So we make our own, which costs
// nothing.

namespace detail
{

template <class T, size_t N>
struct constarray
{
    T elems[N];

    constexpr T& operator[](size_t i)
    {
        return elems[i];
    }

    constexpr const T& operator[](size_t i) const
    {
        return elems[i];
    }

};

} // namespace detail

#endif // TVISION_CONSTARR_H

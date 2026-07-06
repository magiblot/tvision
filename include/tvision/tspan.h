/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   TSPAN.H                                                               */
/*                                                                         */
/*   Defines the class TSpan and its member functions.                     */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef TVISION_TSPAN_H
#define TVISION_TSPAN_H

#if !defined(TVISION_NO_STL) && __cplusplus >= 202002L
#include <span>
#endif

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  class TSpan                                                            */
/*                                                                         */
/*  Represents a view over a contiguous sequence of elements of type T,    */
/*  similar to std::span. It is used to pass arrays around without         */
/*  copying or owning them.                                                */
/*                                                                         */
/*  If std::span is available to you, you can continue to use it in your   */
/*  own code: TSpan is intercompatible with std::span, converting to and   */
/*  from it implicitly where possible.                                     */
/*                                                                         */
/*  Unlike std::span, TSpan can be constructed from a null pointer, in     */
/*  which case it results in an empty span.                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/

template <class T>
class TSpan
{
public:

    // These are defined inline because otherwise they trigger
    // a bug in Borland C++ when T is const.

    constexpr TSpan() noexcept :
        ptr(0),
        len(0)
    {
    }

    constexpr TSpan(T _FAR *first, size_t n) noexcept :
        ptr(first),
        len(n)
    {
    }

#if !defined(__BORLANDC__)
    constexpr TSpan(decltype(nullptr)) noexcept :
        TSpan()
    {
    }

    template<size_t N>
    constexpr TSpan(T (&array)[N]) noexcept :
        TSpan(array, N)
    {
    }
#endif

#if !defined(TVISION_NO_STL) && __cplusplus >= 202002L
    constexpr TSpan(std::span<T> s) noexcept :
        ptr(s.data()),
        len(s.size())
    {
    }

    constexpr operator std::span<T>() const noexcept
    {
        return {ptr, len};
    }
#endif

    constexpr operator TSpan<const T>() const noexcept
    {
        return TSpan<const T>(ptr, len);
    }

    constexpr T _FAR * data() const noexcept
    {
        return ptr;
    }

    constexpr size_t size() const noexcept
    {
        return len;
    }

    constexpr size_t size_bytes() const noexcept
    {
        return size()*sizeof(T);
    }

    constexpr Boolean empty() const noexcept
    {
        return Boolean( size() == 0 );
    }

    constexpr T _FAR & operator[](size_t pos) const noexcept
    {
        return ptr[pos];
    }

    constexpr T _FAR & front() const noexcept
    {
        return ptr[0];
    }

    constexpr T _FAR & back() const noexcept
    {
        return ptr[len - 1];
    }

    constexpr TSpan subspan(size_t pos) const noexcept
    {
        if (pos >= len)
            return TSpan<T>(ptr + len, 0);
        else
            return TSpan<T>(ptr + pos, len - pos);
    }

    constexpr TSpan subspan(size_t pos, size_t n) const noexcept
    {
        if (pos >= len)
            return TSpan<T>(ptr + len, 0);
        else
            return TSpan<T>(ptr + pos, n <= len - pos ? n : len - pos);
    }

    constexpr T _FAR * begin() const noexcept
    {
        return &ptr[0];
    }

    constexpr const T _FAR * cbegin() const noexcept
    {
        return &ptr[0];
    }

    constexpr T _FAR * end() const noexcept
    {
        return &ptr[len];
    }

    constexpr const T _FAR * cend() const noexcept
    {
        return &ptr[len];
    }

private:

    T _FAR *ptr;
    size_t len;
};

#endif // TVISION_TSPAN_H

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   TSTRVIEW.H                                                            */
/*                                                                         */
/*   Defines the class TStringView and its member functions.               */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef TVISION_TSTRVIEW_H
#define TVISION_TSTRVIEW_H

#include <string.h>
#include <tvision/compat/borland/iosfwd.h>

#if !defined(TVISION_NO_STL) && __cplusplus >= 199711L
#include <string>

#if __cplusplus >= 201703L
#include <string_view>
#endif
#endif // TVISION_NO_STL

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  class TStringView                                                      */
/*                                                                         */
/*  Represents a read-only view over a contiguous sequence of characters,  */
/*  similar to std::string_view. It is used throughout Turbo Vision's API  */
/*  to pass around text without copying or owning it.                      */
/*                                                                         */
/*  If std::string_view is available to you, you can continue to use it    */
/*  in your own code: TStringView is intercompatible with                  */
/*  std::string_view, std::string and TSpan<const char>, converting to     */
/*  and from them implicitly where possible.                               */
/*                                                                         */
/*  Unlike std::string_view, TStringView can be constructed from a null    */
/*  pointer, in which case it results in an empty view. Turbo Vision       */
/*  originally used 'const char *' parameters everywhere instead.          */
/*                                                                         */
/*-------------------------------------------------------------------------*/

class TStringView
{
public:

    constexpr TStringView() noexcept;
#if !defined(TVISION_NO_STL) && __cplusplus >= 201703L
    constexpr TStringView(const char _FAR *str) noexcept;
#else
    TStringView(const char _FAR *str) noexcept;
#endif
    constexpr TStringView(const char _FAR *str, size_t len) noexcept;
    constexpr TStringView(TSpan<char> span) noexcept;
    constexpr TStringView(TSpan<const char> span) noexcept;
#if !defined(TVISION_NO_STL) && __cplusplus >= 199711L
#if __cplusplus >= 201703L
    constexpr TStringView(std::string_view text) noexcept;
    constexpr operator std::string_view() const noexcept;
#endif
    TStringView(const std::string &text) noexcept;
    operator std::string() const;
#endif // TVISION_NO_STL
    constexpr operator TSpan<const char>() const noexcept;

    constexpr const char _FAR * data() const noexcept;
    constexpr size_t size() const noexcept;
    constexpr Boolean empty() const noexcept;
    constexpr const char _FAR & operator[](size_t pos) const noexcept;
    constexpr const char _FAR & front() const noexcept;
    constexpr const char _FAR & back() const noexcept;

    constexpr TStringView substr(size_t pos) const noexcept;
    constexpr TStringView substr(size_t pos, size_t n) const noexcept;

    constexpr const char _FAR * begin() const noexcept;
    constexpr const char _FAR * cbegin() const noexcept;
    constexpr const char _FAR * end() const noexcept;
    constexpr const char _FAR * cend() const noexcept;

#if !defined(TVISION_NO_STL) && __cplusplus >= 201703L
    friend constexpr Boolean operator==(TStringView a, TStringView b) noexcept;
    friend constexpr Boolean operator!=(TStringView a, TStringView b) noexcept;
    friend constexpr Boolean operator<(TStringView a, TStringView b) noexcept;
    friend constexpr Boolean operator>(TStringView a, TStringView b) noexcept;
    friend constexpr Boolean operator<=(TStringView a, TStringView b) noexcept;
    friend constexpr Boolean operator>=(TStringView a, TStringView b) noexcept;
#else
    friend Boolean operator==(TStringView a, TStringView b) noexcept;
    friend Boolean operator!=(TStringView a, TStringView b) noexcept;
    friend Boolean operator<(TStringView a, TStringView b) noexcept;
    friend Boolean operator>(TStringView a, TStringView b) noexcept;
    friend Boolean operator<=(TStringView a, TStringView b) noexcept;
    friend Boolean operator>=(TStringView a, TStringView b) noexcept;
#endif

    friend ostream _FAR & _Cdecl operator<<(ostream _FAR &, TStringView);

private:

    const char _FAR *str;
    size_t len;
};

constexpr TStringView::TStringView() noexcept :
    str(0),
    len(0)
{
}

#if !defined(TVISION_NO_STL) && __cplusplus >= 201703L
constexpr TStringView::TStringView(const char _FAR *str) noexcept :
    str(str),
    len(str ? std::char_traits<char>::length(str) : 0)
{
}
#else
inline TStringView::TStringView(const char _FAR *str) noexcept :
    str(str),
    len(str ? strlen(str) : 0)
{
}
#endif

constexpr TStringView::TStringView(const char _FAR *str, size_t len) noexcept :
    str(str),
    len(len)
{
}

constexpr TStringView::TStringView(TSpan<char> span) noexcept :
    str(span.data()),
    len(span.size())
{
}

constexpr TStringView::TStringView(TSpan<const char> span) noexcept :
    str(span.data()),
    len(span.size())
{
}

#if !defined(TVISION_NO_STL) && __cplusplus >= 199711L
#if __cplusplus >= 201703L
constexpr TStringView::TStringView(std::string_view text) noexcept :
    str(text.data()),
    len(text.size())
{
}

constexpr TStringView::operator std::string_view() const noexcept
{
    return {str, len};
}
#endif

inline TStringView::TStringView(const std::string &text) noexcept :
    str(text.data()),
    len(text.size())
{
}

inline TStringView::operator std::string() const
{
    return {str, len};
}
#endif // TVISION_NO_STL

constexpr TStringView::operator TSpan<const char>() const noexcept
{
    return TSpan<const char>(str, len);
}

constexpr const char _FAR * TStringView::data() const noexcept
{
    return str;
}

constexpr size_t TStringView::size() const noexcept
{
    return len;
}

constexpr Boolean TStringView::empty() const noexcept
{
    return Boolean( size() == 0 );
}

constexpr const char _FAR & TStringView::operator[](size_t pos) const noexcept
{
    return str[pos];
}

constexpr const char _FAR & TStringView::front() const noexcept
{
    return str[0];
}

constexpr const char _FAR & TStringView::back() const noexcept
{
    return str[len - 1];
}

constexpr TStringView TStringView::substr(size_t pos) const noexcept
{
    if (pos >= len)
        return TStringView(str + len, 0);
    else
        return TStringView(str + pos, len - pos);
}

constexpr TStringView TStringView::substr(size_t pos, size_t n) const noexcept
{
    if (pos >= len)
        return TStringView(str + len, 0);
    else
        return TStringView(str + pos, n <= len - pos ? n : len - pos);
}

constexpr const char _FAR * TStringView::begin() const noexcept
{
    return &str[0];
}

constexpr const char _FAR * TStringView::cbegin() const noexcept
{
    return &str[0];
}

constexpr const char _FAR * TStringView::end() const noexcept
{
    return &str[len];
}

constexpr const char _FAR * TStringView::cend() const noexcept
{
    return &str[len];
}

#if !defined(TVISION_NO_STL) && __cplusplus >= 201703L
constexpr Boolean operator==(TStringView a, TStringView b) noexcept
{
    if (a.size() == b.size())
        return Boolean( std::char_traits<char>::compare(a.data(), b.data(), b.size()) == 0 );
    return False;
}
#else
inline Boolean operator==(TStringView a, TStringView b) noexcept
{
    if (a.size() == b.size())
        return Boolean( b.size() == 0 || memcmp(a.data(), b.data(), b.size()) == 0 );
    return False;
}
#endif

#if !defined(TVISION_NO_STL) && __cplusplus >= 201703L
constexpr
#endif
inline Boolean operator!=(TStringView a, TStringView b) noexcept
{
    return Boolean( !(a == b) );
}

#if !defined(TVISION_NO_STL) && __cplusplus >= 201703L
constexpr Boolean operator<(TStringView a, TStringView b) noexcept
{
    size_t n = a.size() < b.size() ? a.size() : b.size();
    int c = n ? std::char_traits<char>::compare(a.data(), b.data(), n) : 0;
    return Boolean( c < 0 || (c == 0 && a.size() < b.size()) );
}
#else
inline Boolean operator<(TStringView a, TStringView b) noexcept
{
    size_t n = a.size() < b.size() ? a.size() : b.size();
    int c = n ? memcmp(a.data(), b.data(), n) : 0;
    return Boolean( c < 0 || (c == 0 && a.size() < b.size()) );
}
#endif

#if !defined(TVISION_NO_STL) && __cplusplus >= 201703L
constexpr
#endif
inline Boolean operator>(TStringView a, TStringView b) noexcept
{
    return Boolean( b < a );
}

#if !defined(TVISION_NO_STL) && __cplusplus >= 201703L
constexpr
#endif
inline Boolean operator<=(TStringView a, TStringView b) noexcept
{
    return Boolean( !(b < a) );
}

#if !defined(TVISION_NO_STL) && __cplusplus >= 201703L
constexpr
#endif
inline Boolean operator>=(TStringView a, TStringView b) noexcept
{
    return Boolean( !(a < b) );
}

#if !defined(TVISION_NO_STL) && __cplusplus >= 201103L

#include <typeindex>

namespace std {
#if __cplusplus >= 201703L
    template<>
    struct hash<TStringView> : public hash<std::string_view> {};
#else
    template<>
    struct hash<TStringView> : public hash<std::string> {};
#endif
} // namespace std

#endif // TVISION_NO_STL && __cplusplus >= 201103L

#endif // TVISION_TSTRVIEW_H

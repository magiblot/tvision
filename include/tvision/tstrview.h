/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   TSTRVIEW.H                                                            */
/*                                                                         */
/*   Defines the class TStringView and its member functions.               */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef TVISION_TSTRVIEW_H
#define TVISION_TSTRVIEW_H

#if !defined( __STRING_H )
#include <string.h>
#endif  // __STRING_H

#ifndef __BORLANDC__

#if __cplusplus >= 201703L
#include <string_view>
#endif

#include <string>

#endif // __BORLANDC__

class TStringView {

    // This class exists only to compensate for the lack of std::string_view
    // in Borland C++. Unless you are programming for that compiler, you should
    // always use std::string_view.
    // Unlike std::string_view, TStringView can be constructed from a null pointer,
    // for backward compatibility.
    // TStringView is intercompatible with std::string_view, std::string and
    // TSpan<const char>.

    const char _FAR *str;
    size_t len;

public:

    constexpr TStringView();
    constexpr TStringView(const char _FAR *str);
    constexpr TStringView(const char _FAR *str, size_t len);
    constexpr TStringView(TSpan<char> span);
    constexpr TStringView(TSpan<const char> span);
#if __cplusplus >= 201703L
    constexpr TStringView(std::string_view text);
    constexpr operator std::string_view() const;
#endif
#ifndef __BORLANDC__
    TStringView(const std::string &text);
#endif
    constexpr operator TSpan<const char>() const;

    constexpr const char _FAR * data() const;
    constexpr size_t size() const;
    constexpr Boolean empty() const;
    constexpr const char _FAR & operator[](size_t pos) const;
    constexpr const char _FAR & front() const;
    constexpr const char _FAR & back() const;

    constexpr TStringView substr(size_t pos) const;
    constexpr TStringView substr(size_t pos, size_t n) const;

    friend constexpr Boolean operator==(TStringView a, TStringView b);
    friend constexpr Boolean operator!=(TStringView a, TStringView b);

};

inline constexpr TStringView::TStringView() :
    str(0),
    len(0)
{
}

#pragma warn -inl

#if __cplusplus >= 201703L
inline constexpr TStringView::TStringView(const char _FAR *str) :
    TStringView()
{
    if (str)
        *this = std::string_view {str};
}
#else
inline constexpr TStringView::TStringView(const char _FAR *str) :
    str(str),
    len(0)
{
    if (str)
        while (str[len])
            ++len;
}
#endif

#pragma warn .inl

inline constexpr TStringView::TStringView(const char _FAR *str, size_t len) :
    str(str),
    len(len)
{
}

inline constexpr TStringView::TStringView(TSpan<char> span) :
    str(span.data()),
    len(span.size())
{
}

inline constexpr TStringView::TStringView(TSpan<const char> span) :
    str(span.data()),
    len(span.size())
{
}

#if __cplusplus >= 201703L
inline constexpr TStringView::TStringView(std::string_view text) :
    str(text.data()),
    len(text.size())
{
}

inline constexpr TStringView::operator std::string_view() const
{
    return {str, len};
}
#endif

#ifndef __BORLANDC__
inline TStringView::TStringView(const std::string &text) :
    str(text.data()),
    len(text.size())
{
}
#endif

inline constexpr TStringView::operator TSpan<const char>() const
{
    return TSpan<const char>(str, len);
}

inline constexpr const char _FAR * TStringView::data() const
{
    return str;
}

inline constexpr size_t TStringView::size() const
{
    return len;
}

inline constexpr Boolean TStringView::empty() const
{
    return size() == 0;
}

inline constexpr const char _FAR & TStringView::operator[](size_t pos) const
{
    return str[pos];
}

inline constexpr const char _FAR & TStringView::front() const
{
    return str[0];
}

inline constexpr const char _FAR & TStringView::back() const
{
    return str[len - 1];
}

inline constexpr TStringView TStringView::substr(size_t pos) const
{
    return TStringView(str + pos, len - pos);
}

inline constexpr TStringView TStringView::substr(size_t pos, size_t n) const
{
    size_t tail = len - pos;
    if (n > tail)
        n = tail;
    return TStringView(str + pos, n);
}

inline constexpr Boolean operator==(TStringView a, TStringView b)
{
#if __cplusplus >= 201703L
    return std::string_view {a} == std::string_view {b};
#else
    if (a.size() == b.size())
        return memcmp(a.data(), b.data(), b.size()) == 0;
    return False;
#endif
}

inline constexpr Boolean operator!=(TStringView a, TStringView b)
{
    return !(a == b);
}


#endif // TVISION_TSTRVIEW_H

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   TSPAN.H                                                               */
/*                                                                         */
/*   Defines the class TSpan and its member functions.                     */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef TVISION_TSPAN_H
#define TVISION_TSPAN_H

template <class T>
class TSpan {

    // This is actually a generalization of TStringView for any kind of element
    // type (and without enforcing the 'const' qualifier).
    // It exists for compatibility with Borland C++ and because std::span (C++ 20)
    // may not be widely available yet.
    // Unlike std::span, TSpan<T> can cast implicitly to T* (pointer to T).

    T _FAR *ptr;
    size_t len;

public:

    constexpr TSpan();
    constexpr TSpan(T _FAR *first, size_t n);

    constexpr T _FAR * data() const;
    constexpr size_t size() const;
    constexpr size_t size_bytes() const;
    constexpr Boolean empty() const;
    constexpr T _FAR & operator[](size_t pos) const;
    constexpr operator T _FAR *() const;
    constexpr T _FAR & front() const;
    constexpr T _FAR & back() const;

    constexpr TSpan subspan(size_t pos) const;
    constexpr TSpan subspan(size_t pos, size_t n) const;

};

template <class T>
inline constexpr TSpan<T>::TSpan() :
    ptr(0),
    len(0)
{
}

template <class T>
inline constexpr TSpan<T>::TSpan(T _FAR *first, size_t n) :
    ptr(first),
    len(n)
{
}

template <class T>
inline constexpr T _FAR * TSpan<T>::data() const
{
    return ptr;
}

template <class T>
inline constexpr size_t TSpan<T>::size() const
{
    return len;
}

template <class T>
inline constexpr size_t TSpan<T>::size_bytes() const
{
    return size()*sizeof(T);
}

template <class T>
inline constexpr Boolean TSpan<T>::empty() const
{
    return size() == 0;
}

template <class T>
inline constexpr T _FAR & TSpan<T>::operator[](size_t pos) const
{
    return ptr[pos];
}

template <class T>
inline constexpr TSpan<T>::operator T _FAR *() const
{
    return ptr;
}

template <class T>
inline constexpr T _FAR & TSpan<T>::front() const
{
    return ptr[0];
}

template <class T>
inline constexpr T _FAR & TSpan<T>::back() const
{
    return ptr[len - 1];
}

template <class T>
inline constexpr TSpan<T> TSpan<T>::subspan(size_t pos) const
{
    return TSpan<T>(ptr + pos, len - pos);
}

template <class T>
inline constexpr TSpan<T> TSpan<T>::subspan(size_t pos, size_t n) const
{
    size_t tail = len - pos;
    if (n > tail)
        n = tail;
    return TSpan<T>(ptr + pos, n);
}

#endif // TVISION_TSPAN_H

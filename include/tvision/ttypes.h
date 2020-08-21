/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   TTYPES.H                                                              */
/*                                                                         */
/*   provides miscellaneous types used throughout Turbo Vision             */
/*                                                                         */
/* ------------------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#if !defined( __TTYPES_H )
#define __TTYPES_H

#if !defined(_NEAR)
#define _NEAR near
#endif

#include <_defs.h>

#ifdef __BORLANDC__
#define I   asm
#endif

enum { False, True };

#ifdef __BORLANDC__
typedef int Boolean;
#else
typedef bool Boolean;
#endif

typedef unsigned short ushort;
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

#ifndef __BORLANDC__
#include <cstddef>
#include <cstdint>
#else
#include <stddef.h>
typedef char int8_t;
typedef short int16_t;
typedef long int32_t;
typedef uchar uint8_t;
typedef ushort uint16_t;
typedef ulong uint32_t;
#endif

const char EOS = '\0';

enum StreamableInit { streamableInit };

class _FAR ipstream;
class _FAR opstream;
class _FAR TStreamable;
class _FAR TStreamableTypes;

typedef int ccIndex;
typedef Boolean (*ccTestFunc)( void *, void * );
typedef void (*ccAppFunc)( void *, void * );

const int ccNotFound = -1;

extern const uchar specialChars[];

#if !defined ( __FLAT__ )
#define _genInt(i) __int__(i)
#endif

#ifdef __BORLANDC__
#define constexpr
#endif

template <class T>
class TSpan {

    // This is actually a generalization of TStringView for any kind of element
    // type (and with no 'const' by default).
    // It exists for compatibility with Borland C++ and because std::span (C++ 20)
    // may not be widely available yet.

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

#ifdef __BORLANDC__
#undef constexpr
#endif

#endif  // __TTYPES_H

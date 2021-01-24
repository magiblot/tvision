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

#include <tvision/compat/_defs.h>

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
#include <stdint.h>
#else
#include <stddef.h>
typedef char int8_t;
typedef short int16_t;
typedef long int32_t;
typedef uchar uint8_t;
typedef ushort uint16_t;
typedef ulong uint32_t;
#endif

#ifdef __BORLANDC__
typedef ushort TScreenCell;
typedef uchar TCellAttribs;
typedef uchar TCellChar;
#else
struct TScreenCell;
struct TCellAttribs;
struct TCellChar;
#endif

const char EOS = '\0';

enum StreamableInit { streamableInit };

class _FAR ipstream;
class _FAR opstream;
class _FAR TStreamable;
class _FAR TStreamableTypes;

ipstream& _Cdecl operator >> ( ipstream&, char& );
ipstream& _Cdecl operator >> ( ipstream&, signed char& );
ipstream& _Cdecl operator >> ( ipstream&, unsigned char& );
ipstream& _Cdecl operator >> ( ipstream&, signed short& );
ipstream& _Cdecl operator >> ( ipstream&, unsigned short& );
ipstream& _Cdecl operator >> ( ipstream&, signed int& );
ipstream& _Cdecl operator >> ( ipstream&, unsigned int& );
ipstream& _Cdecl operator >> ( ipstream&, signed long& );
ipstream& _Cdecl operator >> ( ipstream&, unsigned long& );
ipstream& _Cdecl operator >> ( ipstream&, float& );
ipstream& _Cdecl operator >> ( ipstream&, double& );
ipstream& _Cdecl operator >> ( ipstream&, long double& );
ipstream& _Cdecl operator >> ( ipstream&, TStreamable& );
ipstream& _Cdecl operator >> ( ipstream&, void _FAR *& );

opstream& _Cdecl operator << ( opstream&, char );
opstream& _Cdecl operator << ( opstream&, signed char );
opstream& _Cdecl operator << ( opstream&, unsigned char );
opstream& _Cdecl operator << ( opstream&, signed short );
opstream& _Cdecl operator << ( opstream&, unsigned short );
opstream& _Cdecl operator << ( opstream&, signed int );
opstream& _Cdecl operator << ( opstream&, unsigned int );
opstream& _Cdecl operator << ( opstream&, signed long );
opstream& _Cdecl operator << ( opstream&, unsigned long );
opstream& _Cdecl operator << ( opstream&, float );
opstream& _Cdecl operator << ( opstream&, double );
opstream& _Cdecl operator << ( opstream&, long double );
opstream& _Cdecl operator << ( opstream&, TStreamable& );
opstream& _Cdecl operator << ( opstream&, TStreamable _FAR * );

typedef int ccIndex;
typedef Boolean (*ccTestFunc)( void *, void * );
typedef void (*ccAppFunc)( void *, void * );

const int ccNotFound = -1;

extern const uchar specialChars[];

#if !defined ( __FLAT__ )
#define _genInt(i) __int__(i)
#endif

// Reserve future keywords
#if __cplusplus < 201103L
#define constexpr
#define noexcept
#endif

#endif  // __TTYPES_H

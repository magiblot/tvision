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

#define I   asm

enum Boolean { False, True };

typedef unsigned short ushort;
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

const char EOS = '\0';

enum StreamableInit { streamableInit };

class _FAR ipstream;
class _FAR opstream;
class _FAR TStreamable;
class _FAR TStreamableTypes;

typedef int ccIndex;
typedef Boolean (*ccTestFunc)( void *, void * );
typedef void (*ccAppFunc)( void *, void * );

const ccNotFound = -1;

extern const uchar specialChars[];

#if !defined ( __FLAT__ )
#define _genInt(i) __int__(i)
#endif

#endif  // __TTYPES_H

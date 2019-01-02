/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   TTYPES.H                                                              */
/*                                                                         */
/*   Copyright (c) Borland International 1991                              */
/*   All Rights Reserved.                                                  */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if !defined( __TTYPES_H )
#define __TTYPES_H

enum Boolean { False, True };

typedef unsigned short ushort;
typedef unsigned char uchar;

const char EOS = '\0';

enum StreamableInit { streamableInit };

class far ipstream;
class far opstream;
class far TStreamable;
class far TStreamableTypes;

typedef int ccIndex;
typedef Boolean (*ccTestFunc)( void *, void * );
typedef void (*ccAppFunc)( void *, void * );

const ccNotFound = -1;

extern const uchar specialChars[];


#endif	// __TTYPES_H

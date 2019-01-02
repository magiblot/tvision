/*****************************************************************************\
*                                                                             *
* lzexpand.h    Public interfaces for LZEXPAND.DLL.                           *
*                                                                             *
*               Version 3.10                                                  *
*                                                                             *
*               NOTE: windows.h must be included first if LIB is NOT #defined *
*                                                                             *
*******************************************************************************
*
* #define LIB   - To be used with LZEXP?.LIB (default is for LZEXPAND.DLL)
*                 NOTE: Not compatible with windows.h if LIB is #defined
*
\*****************************************************************************/

/*
 *      C/C++ Run Time Library - Version 6.0
 *
 *      Copyright (c) 1987, 1993 by Borland International
 *      All Rights Reserved.
 *
 */

#ifndef __LZEXPAND_H    /* prevent multiple includes */
#define __LZEXPAND_H

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

#if !defined(__WINDOWS_H) && !defined(LIB)
#include <windows.h>    /* <windows.h> must be included */
#endif  /* __WINDOWS_H */

#if !defined(__FLAT__)

/*
 * If .lib version is being used, declare types used in this file.
 */
#ifdef LIB

#define LZAPI           _pascal

#ifndef WINAPI          /* don't declare if they're already declared */
#define WINAPI          _far _pascal
#define NEAR            _near
#define FAR             _far
#define PASCAL          _pascal
typedef int             BOOL;
#define TRUE            1
#define FALSE           0
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned int    UINT;
typedef signed long     LONG;
typedef unsigned long   DWORD;
typedef char far*       LPSTR;
typedef const char far* LPCSTR;
typedef int             HFILE;
#define OFSTRUCT        void            /* Not used by the .lib version */
#endif  /* WINAPI */

#else   /* LIB */

#define LZAPI           _far _pascal

/* If included with the 3.0 windows.h, define compatible aliases */
#if !defined(WINVER) || (WINVER < 0x030a)
#define UINT        WORD
#define LPCSTR      LPSTR
#define HFILE       int
#endif  /* WIN3.0 */

#endif  /* !LIB */

/****** Error return codes ***************************************************/

#define LZERROR_BADINHANDLE   (-1)  /* invalid input handle */
#define LZERROR_BADOUTHANDLE  (-2)  /* invalid output handle */
#define LZERROR_READ          (-3)  /* corrupt compressed file format */
#define LZERROR_WRITE         (-4)  /* out of space for output file */
#define LZERROR_GLOBALLOC     (-5)  /* insufficient memory for LZFile struct */
#define LZERROR_GLOBLOCK      (-6)  /* bad global handle */
#define LZERROR_BADVALUE      (-7)  /* input parameter out of range */
#define LZERROR_UNKNOWNALG    (-8)  /* compression algorithm not recognized */

/****** Public functions *****************************************************/

int     LZAPI LZStart(void);
void    LZAPI LZDone(void);
LONG    LZAPI CopyLZFile(HFILE, HFILE);
LONG    LZAPI LZCopy(HFILE, HFILE);
HFILE   LZAPI LZInit(HFILE);
int     LZAPI GetExpandedName(LPCSTR, LPSTR);
HFILE   LZAPI LZOpenFile(LPCSTR, OFSTRUCT FAR*, UINT);
LONG    LZAPI LZSeek(HFILE, LONG, int);
int     LZAPI LZRead(HFILE, void FAR*, int);
void    LZAPI LZClose(HFILE);

#else    /* defined __FLAT__ */

/*
** Error Return Codes
*/

#define LZERROR_BADINHANDLE   (-1)  /* invalid input handle */
#define LZERROR_BADOUTHANDLE  (-2)  /* invalid output handle */
#define LZERROR_READ          (-3)  /* corrupt compressed file format */
#define LZERROR_WRITE         (-4)  /* out of space for output file */
#define LZERROR_GLOBALLOC     (-5)  /* insufficient memory for LZFile struct */
#define LZERROR_GLOBLOCK      (-6)  /* bad global handle */
#define LZERROR_BADVALUE      (-7)  /* input parameter out of acceptable range*/
#define LZERROR_UNKNOWNALG    (-8)  /* compression algorithm not recognized */


/*
** Prototypes
*/

INT
APIENTRY
LZStart(
        VOID
        );

VOID
APIENTRY
LZDone(
        VOID
        );

LONG
APIENTRY
CopyLZFile(
        INT,
        INT
        );

LONG
APIENTRY
LZCopy(
        INT,
        INT
        );


INT
APIENTRY
LZInit(
        INT
        );

INT
APIENTRY
GetExpandedNameA(
        LPSTR,
        LPSTR
        );
INT
APIENTRY
GetExpandedNameW(
        LPWSTR,
        LPWSTR
        );
#ifdef UNICODE
#define GetExpandedName GetExpandedNameW
#else
#define GetExpandedName GetExpandedNameA
#endif // !UNICODE

INT
APIENTRY
LZOpenFileA(
        LPSTR,
        LPOFSTRUCT,
        WORD
        );
INT
APIENTRY
LZOpenFileW(
        LPWSTR,
        LPOFSTRUCT,
        WORD
        );
#ifdef UNICODE
#define LZOpenFile LZOpenFileW
#else
#define LZOpenFile LZOpenFileA
#endif // !UNICODE

LONG
APIENTRY
LZSeek(
        INT,
        LONG,
        INT
        );

INT
APIENTRY
LZRead(
        INT,
        LPSTR,
        INT
        );

VOID
APIENTRY
LZClose(
        INT
        );

#endif  /* __FLAT__ */

#ifdef __cplusplus
}       /* End of extern "C" { */
#endif  /* __cplusplus */


#endif // _LZEXPAND_INCLUDED_

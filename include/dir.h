/*  dir.h

    Defines structures, macros, and functions for dealing with
    directories and pathnames.

*/

/*
 *      C/C++ Run Time Library - Version 6.0
 *
 *      Copyright (c) 1987, 1993 by Borland International
 *      All Rights Reserved.
 *
 */

#if !defined(__DIR_H)
#define __DIR_H

#if !defined(___DEFS_H)
#include <_defs.h>
#endif

#define WILDCARDS 0x01
#define EXTENSION 0x02
#define FILENAME  0x04
#define DIRECTORY 0x08
#define DRIVE     0x10

#define MAXDRIVE  3

#if !defined(__FLAT__)

#ifndef _FFBLK_DEF
#define _FFBLK_DEF
struct  ffblk   {
    char        ff_reserved[21];
    char        ff_attrib;
    unsigned    ff_ftime;
    unsigned    ff_fdate;
    long        ff_fsize;
    char        ff_name[13];
};
#endif

#define MAXPATH   80
#define MAXDIR    66
#define MAXFILE   9
#define MAXEXT    5

#else  /* defined __FLAT__ */

#pragma option -a-

#ifndef _FFBLK_DEF
#define _FFBLK_DEF
struct  ffblk   {
    long            ff_reserved;
    long            ff_fsize;
    unsigned long   ff_attrib;
    unsigned short  ff_ftime;
    unsigned short  ff_fdate;
    char            ff_name[256];
};
#endif

#define MAXPATH   260
#define MAXDIR    256
#define MAXFILE   256
#define MAXEXT    256

#pragma option -a.  /* restore default packing */

#endif  /* __FLAT__  */

#ifdef __cplusplus
extern "C" {
#endif

int         _RTLENTRYF _EXPFUNC32   chdir( const char _FAR *__path );
int         _RTLENTRYF _EXPFUNC     findfirst( const char _FAR *__path,
                                    struct ffblk _FAR *__ffblk,
                                    int __attrib );
int         _RTLENTRYF _EXPFUNC     findnext( struct ffblk _FAR *__ffblk );
void        _RTLENTRYF _EXPFUNC     fnmerge( char _FAR *__path,
                                    const char _FAR *__drive,
                                    const char _FAR *__dir,
                                    const char _FAR *__name,
                                    const char _FAR *__ext );
int         _RTLENTRYF _EXPFUNC     fnsplit(const char _FAR *__path,
                                    char _FAR *__drive,
                                    char _FAR *__dir,
                                    char _FAR *__name,
                                    char _FAR *__ext );
int         _RTLENTRY  _EXPFUNC32   getcurdir( int __drive, char _FAR *__directory );
char _FAR * _RTLENTRY  _EXPFUNC     getcwd( char _FAR *__buf, int __buflen );
int         _RTLENTRY  _EXPFUNC32   getdisk( void );
int         _RTLENTRYF _EXPFUNC32   mkdir( const char _FAR *__path );
char _FAR * _RTLENTRYF _EXPFUNC     mktemp( char _FAR *__template );
int         _RTLENTRY  _EXPFUNC32   rmdir( const char _FAR *__path );
char _FAR * _RTLENTRYF _EXPFUNC32   searchpath( const char _FAR *__file );
int         _RTLENTRY  _EXPFUNC32   setdisk( int __drive );

#ifdef __cplusplus
}
#endif

#endif  /* __DIR_H */



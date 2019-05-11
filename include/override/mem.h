/*  mem.h

    Memory manipulation functions

    Copyright (c) 1987, 1992 by Borland International
    All Rights Reserved.
*/

#if !defined(__MEM_H)
#define __MEM_H

#if !defined(___DEFS_H)
#include <_defs.h>
#endif

#ifndef NULL
#include <_null.h>
#endif

#include <cstddef>

#ifdef __cplusplus
extern "C" {
#endif
// void _FAR * _FARFUNC _Cdecl memccpy(void _FAR *__dest, const void _FAR *__src,
//                            int __c, size_t __n);
// void _FAR * _FARFUNC _Cdecl memchr(const void _FAR *__s, int __c, size_t __n);
// int         _FARFUNC _Cdecl memcmp(const void _FAR *__s1, const void _FAR *__s2,
//                           size_t __n);
// void _FAR * _FARFUNC _Cdecl memcpy(void _FAR *__dest, const void _FAR *__src,
//                           size_t __n);
int         _FARFUNC _Cdecl memicmp(const void _FAR *__s1, const void _FAR *__s2,
                          size_t __n);
// void _FAR * _CType _FARFUNC memmove(void _FAR *__dest, const void _FAR *__src,
//                           size_t __n);
// void _FAR * _CType _FARFUNC memset(void _FAR *__s, int __c, size_t __n);
void        _FARFUNC _Cdecl movedata(unsigned __srcseg, unsigned __srcoff,
                            unsigned __dstseg,unsigned __dstoff, size_t __n);
#include <string.h>
static void        _FARFUNC _Cdecl movmem(const void _FAR *__src, void _FAR *__dest,
                                   unsigned __length) {
  memmove(__dest, __src, __length);
}
void        _FARFUNC _Cdecl setmem(void _FAR *__dest,unsigned __length, char __value);

#if !__STDC__
void  far * _FARCALL cdecl _fmemccpy(void far *__dest, const void far *__src,
                       int __c, size_t __n);
void  far * _FARCALL cdecl _fmemchr(const void far *__s, int __c, size_t __n);
int         _FARCALL cdecl _fmemcmp(const void far *__s1, const void far *__s2,
                       size_t __n);
void  far * _FARCALL cdecl _fmemcpy(void far *__dest, const void far *__src,
                       size_t __n);
int         _FARCALL cdecl _fmemicmp(const void far *__s1, const void far *__s2,
                       size_t __n);
void  far * _FARCALL cdecl _fmemmove(void far *__dest, const void far *__src,
                       size_t __n);
void  far * _FARCALL cdecl _fmemset(void far *__s, int __c, size_t __n);
void        _FARCALL cdecl _fmovmem(const void far *__src, void far *__dest,
                       unsigned __length);
void        _FARCALL cdecl _fsetmem(void far *__dest, unsigned __length,
                       char __value);
#endif

#if !defined(__STRING_H)
/* Intrinsic functions */

void _FAR * _Cdecl _FARFUNC __memchr__  (const void _FAR *__s, int __c, size_t __n);
int         _Cdecl _FARFUNC __memcmp__(const void _FAR *__s1,
                          const void _FAR *__s2, size_t __n);
void _FAR * _Cdecl _FARFUNC __memcpy__(void _FAR *__dest, const void _FAR *__src,
                          size_t __n);
void _FAR * _CType _FARFUNC __memset__(void _FAR *__s, int __c, size_t __n);
#endif
#ifdef __cplusplus
}
#endif

#endif  /* __MEM_H */


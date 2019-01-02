/*  _defs.h

    Common definitions for pointer size and calling conventions.

    Calling conventions:
    _RTLENTRY       Specifies the calling convention used by the RTL

    _USERENTRY      Specifies the calling convention the RTL expects user
                    compiled functions to use (for callbacks)

    Export (and size for DOS) information:
    _EXPCLASS       Exports class if building DLL version of library
                    For DOS16 also provides size information

    _EXPDATA        Exports data if building DLL version of library

    _EXPFUNC        Exports function if building DLL version of library
                    For DOS16 also provides size information

    _FAR            Promotes data pointers to far in DLLs (DOS16 only)

    Obsolete versions:
    _Cdecl          Use _RTLENTRY
    _CLASSTYPE      Use _EXPCLASS
    _FARFUNC        Use _EXPFUNC
    _FARCALL        Use _EXPFUNC and declare function explicity __far

    Copyright (c) 1991, 1992 by Borland International
    All Rights Reserved.
*/

#if !defined(___DEFS_H)
#define ___DEFS_H

#if defined(__MT__)
#    define _MT
#endif

#if defined(__OS2__)
#    define _RTLENTRY  __stdcall
#    define _USERENTRY __stdcall
#elif defined(__WIN32__)
#    define _RTLENTRY  __cdecl
#    define _USERENTRY __cdecl
#else
#    define _RTLENTRY  __cdecl
#    define _USERENTRY __cdecl
#endif

#if defined(__PAS__)
#    define _RTLENTRYF __pascal
#else
#    define _RTLENTRYF _RTLENTRY
#endif

#if defined(__FLAT__)
#   define _FAR
#   if defined(_BUILDRTLDLL)
#       define _EXPCLASS  __export
#       define _EXPDATA   __export
#       define _EXPFUNC   __export
#   elif defined(_RTLDLL)
#       define _EXPCLASS  __import
#       define _EXPDATA   __import
#       define _EXPFUNC   __import
#   else
#       define _EXPCLASS
#       define _EXPDATA
#       define _EXPFUNC
#   endif
#else
#   if defined(__DLL__)
#       if defined(_BUILDRTLDLL)
#           define _EXPCLASS __export
#       elif defined(_RTLDLL) || defined(_CLASSDLL)
#           define _EXPCLASS __export
#       else
#           define _EXPCLASS __far
#       endif
#       define _FAR __far
#   elif defined(_RTLDLL) || defined(_CLASSDLL)
#       define _EXPCLASS __export
#       define _FAR __far
#   else
#       if   defined(__TINY__) || defined(__SMALL__) || defined(__MEDIUM__)
#           if defined(_RTLFARVTABLE)
#               define _EXPCLASS  __huge
#           else
#               define _EXPCLASS  __near
#           endif
#       elif defined(__COMPACT__) || defined(__LARGE__)
#           if defined(_RTLFARVTABLE)
#               define _EXPCLASS  __huge
#           else
#               define _EXPCLASS  __far
#           endif
#       else
#           define _EXPCLASS  __huge
#       endif
#       define _FAR
#   endif
#   if defined(_BUILDRTLDLL)
#       define _EXPFUNC __export
#   else
#       if defined(_RTLDLL) || defined(_CLASSDLL)
#           define _EXPFUNC __far
#       else
#           define _EXPFUNC
#       endif
#   endif
#   define _EXPDATA
#endif


#if defined(__FLAT__)
#define _EXPFUNC32  _EXPFUNC
#define _EXPFUNC16
#else
#define _EXPFUNC32
#define _EXPFUNC16  _EXPFUNC
#endif


#define _Cdecl      _RTLENTRY
#define _CType      _RTLENTRYF
#define _CLASSTYPE  _EXPCLASS
#define _FARFUNC    _EXPFUNC
#define _FARCALL    _EXPFUNC __far

#if defined(__FLAT__)
#  define _M_IX86 300
#endif

#if !defined(__FLAT__) && defined(__MSC)
#   define __emit db
#   ifdef __SMALL__
#       define _M_I86SM
#   endif
#   ifdef __COMPACT__
#       define _M_I86CM
#   endif
#   ifdef __MEDIUM__
#       define _M_I86MM
#   endif
#   ifdef __LARGE__
#       define _M_I86LM
#   endif
#   ifndef _Windows
#       define _DOS
#   else
#       define _WINDOWS
#   endif
#endif

#if defined(__cplusplus)
#   define _PTRDEF(name) typedef name _FAR * P##name;
#   define _REFDEF(name) typedef name _FAR & R##name;
#   define _REFPTRDEF(name) typedef name _FAR * _FAR & RP##name;
#   define _PTRCONSTDEF(name) typedef const name _FAR * PC##name;
#   define _REFCONSTDEF(name) typedef const name _FAR & RC##name;

/*********** Obsolete definitions for OWL 1.0 *************/
#   define _CLASSDEF(name) class _EXPCLASS name; \
           _PTRDEF(name) \
           _REFDEF(name) \
           _REFPTRDEF(name) \
           _PTRCONSTDEF(name) \
           _REFCONSTDEF(name)
#endif

#endif  /* ___DEFS_H */

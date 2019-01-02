/*****************************************************************************\
*                                                                             *
* shellapi.h -  SHELL.DLL functions, types, and definitions                   *
*                                                                             *
\*****************************************************************************/

/*
 *      C/C++ Run Time Library - Version 6.0
 *
 *      Copyright (c) 1987, 1993 by Borland International
 *      All Rights Reserved.
 *
 */

#ifndef __SHELLAPI_H    /* prevent multiple includes */
#define __SHELLAPI_H

#ifndef __WINDOWS_H
#include <windows.h>    /* <windows.h> must be included */
#endif  /* __WINDOWS_H */

#ifndef RC_INVOKED
#pragma option -a-      /* Assume byte packing throughout */
#endif  /* RC_INVOKED */

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

#if !defined(__FLAT__)

/* If included with the 3.0 windows.h, define compatible aliases */
#if !defined(WINVER) || (WINVER < 0x030a)
#define HDROP       HANDLE
#define HINSTANCE   HANDLE
#define WINAPI      FAR PASCAL
#define LPCSTR      LPSTR
#define UINT        WORD
#else
DECLARE_HANDLE(HDROP);
#endif  /* WIN3.0 */

/* return codes from Registration functions */
#define ERROR_SUCCESS           0L
#define ERROR_BADDB             1L
#define ERROR_BADKEY            2L
#define ERROR_CANTOPEN          3L
#define ERROR_CANTREAD          4L
#define ERROR_CANTWRITE         5L
#define ERROR_OUTOFMEMORY       6L
#define ERROR_INVALID_PARAMETER 7L
#define ERROR_ACCESS_DENIED     8L

#define REG_SZ                  1           /* string type */

#define HKEY_CLASSES_ROOT       1

typedef DWORD HKEY;
typedef HKEY FAR* PHKEY;

LONG WINAPI RegOpenKey(HKEY, LPCSTR, HKEY FAR*);
LONG WINAPI RegCreateKey(HKEY, LPCSTR, HKEY FAR*);
LONG WINAPI RegCloseKey(HKEY);
LONG WINAPI RegDeleteKey(HKEY, LPCSTR);
LONG WINAPI RegSetValue(HKEY, LPCSTR, DWORD, LPCSTR, DWORD);
LONG WINAPI RegQueryValue(HKEY, LPCSTR, LPSTR, LONG FAR*);
LONG WINAPI RegEnumKey(HKEY, DWORD, LPSTR, DWORD);

UINT WINAPI DragQueryFile(HDROP, UINT, LPSTR, UINT);
BOOL WINAPI DragQueryPoint(HDROP, POINT FAR*);
void WINAPI DragFinish(HDROP);
void WINAPI DragAcceptFiles(HWND, BOOL);

HICON WINAPI ExtractIcon(HINSTANCE hInst, LPCSTR lpszExeFileName, UINT nIconIndex);

/* error values for ShellExecute() beyond the regular WinExec() codes */
#define SE_ERR_SHARE            26
#define SE_ERR_ASSOCINCOMPLETE  27
#define SE_ERR_DDETIMEOUT       28
#define SE_ERR_DDEFAIL          29
#define SE_ERR_DDEBUSY          30
#define SE_ERR_NOASSOC          31

HINSTANCE WINAPI ShellExecute(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, int iShowCmd);
HINSTANCE WINAPI FindExecutable(LPCSTR lpFile, LPCSTR lpDirectory, LPSTR lpResult);

#else    /* defined __FLAT__ */


/* API exports from the library
 */

DECLARE_HANDLE(HDROP);

UINT APIENTRY DragQueryFileW(HDROP,UINT,LPWSTR,UINT);
UINT APIENTRY DragQueryFileA(HDROP,UINT,LPSTR,UINT);

#ifdef UNICODE
#define DragQueryFile DragQueryFileW
#else
#define DragQueryFile DragQueryFileA
#endif

BOOL APIENTRY DragQueryPoint(HDROP,LPPOINT);
VOID APIENTRY DragFinish(HDROP);

VOID APIENTRY DragAcceptFiles(HWND,BOOL);

HICON  APIENTRY ExtractIconW(HINSTANCE hInst, LPCWSTR lpszExeFileName, UINT nIconIndex);
HICON  APIENTRY ExtractIconA(HINSTANCE hInst, LPCSTR lpszExeFileName, UINT nIconIndex);

#ifdef UNICODE
#define ExtractIcon ExtractIconW
#else
#define ExtractIcon ExtractIconA
#endif

/* error values for ShellExecute() beyond the regular WinExec() codes */
#define SE_ERR_SHARE                    26
#define SE_ERR_ASSOCINCOMPLETE  27
#define SE_ERR_DDETIMEOUT               28
#define SE_ERR_DDEFAIL                  29
#define SE_ERR_DDEBUSY                  30
#define SE_ERR_NOASSOC                  31

HINSTANCE APIENTRY ShellExecuteA(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd);
HINSTANCE APIENTRY ShellExecuteW(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd);

HINSTANCE APIENTRY FindExecutableA(LPCSTR lpFile, LPCSTR lpDirectory, LPSTR lpResult);
HINSTANCE APIENTRY FindExecutableW(LPCWSTR lpFile, LPCWSTR lpDirectory, LPWSTR lpResult);

INT   APIENTRY ShellAboutA(HWND hWnd, LPCSTR szApp, LPCSTR szOtherStuff, HICON hIcon);
INT   APIENTRY ShellAboutW(HWND hWnd, LPCWSTR szApp, LPCWSTR szOtherStuff, HICON hIcon);

#ifndef UNICODE
#define ShellExecute ShellExecuteA
#define FindExecutable FindExecutableA
#define ShellAbout ShellAboutA
#else
#define ShellExecute ShellExecuteW
#define FindExecutable FindExecutableW
#define ShellAbout ShellAboutW
#endif

HICON APIENTRY DuplicateIcon(HINSTANCE hInst, HICON hIcon);                                /* ;Internal */
HICON APIENTRY ExtractAssociatedIconA(HINSTANCE hInst, LPSTR lpIconPath, LPWORD lpiIcon);   /* ;Internal */
HICON APIENTRY ExtractAssociatedIconW(HINSTANCE hInst, LPWSTR lpIconPath, LPWORD lpiIcon);   /* ;Internal */

#ifndef UNICODE
#define ExtractAssociatedIcon ExtractAssociatedIconA
#else
#define ExtractAssociatedIcon ExtractAssociatedIconW
#endif


#endif  /* __FLAT__ */

#ifdef __cplusplus
}
#endif

#ifndef RC_INVOKED
#pragma option -a.      /* Revert to default packing */
#endif  /* RC_INVOKED */

#endif  /* __SHELLAPI_H */

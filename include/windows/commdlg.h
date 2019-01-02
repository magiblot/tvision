/*****************************************************************************\
*                                                                             *
* commdlg.h -   Common dialog functions, types, and definitions               *
*                                                                             *
*                                                                             *
\*****************************************************************************/

/*
 *      C/C++ Run Time Library - Version 6.0
 *
 *      Copyright (c) 1987, 1993 by Borland International
 *      All Rights Reserved.
 *
 */

#ifndef __COMMDLG_H     /* prevent multiple includes */
#define __COMMDLG_H

#pragma warn -nak   /* Ignore non-ansi keywords */

#ifndef __WINDOWS_H
#include <windows.h>    /* <windows.h> must be included */
#endif  /* __WINDOWS_H */

#ifndef RC_INVOKED
#pragma option -a-      /* Assume byte packing throughout */
#endif /* !RC_INVOKED */

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

#if !defined(__FLAT__)

/* If included with the 3.0 windows.h, define compatible aliases */
#if !defined(WINVER) || (WINVER < 0x030a)
#define WINAPI      FAR PASCAL
#define CALLBACK    FAR PASCAL
#define LPCSTR      LPSTR
#define UINT        WORD
#define LPARAM      LONG
#define WPARAM      WORD
#define LRESULT     LONG
#define HMODULE     HANDLE
#define HINSTANCE   HANDLE
#define HLOCAL      HANDLE
#define HGLOBAL     HANDLE
#endif  /* WIN3.0 */

typedef struct tagOFN
{
    DWORD   lStructSize;
    HWND    hwndOwner;
    HINSTANCE hInstance;
    LPCSTR  lpstrFilter;
    LPSTR   lpstrCustomFilter;
    DWORD   nMaxCustFilter;
    DWORD   nFilterIndex;
    LPSTR   lpstrFile;
    DWORD   nMaxFile;
    LPSTR   lpstrFileTitle;
    DWORD   nMaxFileTitle;
    LPCSTR  lpstrInitialDir;
    LPCSTR  lpstrTitle;
    DWORD   Flags;
    UINT    nFileOffset;
    UINT    nFileExtension;
    LPCSTR  lpstrDefExt;
    LPARAM  lCustData;
    UINT    (CALLBACK *lpfnHook)(HWND, UINT, WPARAM, LPARAM);
    LPCSTR  lpTemplateName;
}   OPENFILENAME;
typedef OPENFILENAME FAR* LPOPENFILENAME;

BOOL    WINAPI GetOpenFileName(OPENFILENAME FAR*);
BOOL    WINAPI GetSaveFileName(OPENFILENAME FAR*);
int     WINAPI GetFileTitle(LPCSTR, LPSTR, UINT);

#define OFN_READONLY                 0x00000001L
#define OFN_OVERWRITEPROMPT          0x00000002L
#define OFN_HIDEREADONLY             0x00000004L
#define OFN_NOCHANGEDIR              0x00000008L
#define OFN_SHOWHELP                 0x00000010L
#define OFN_ENABLEHOOK               0x00000020L
#define OFN_ENABLETEMPLATE           0x00000040L
#define OFN_ENABLETEMPLATEHANDLE     0x00000080L
#define OFN_NOVALIDATE               0x00000100L
#define OFN_ALLOWMULTISELECT         0x00000200L
#define OFN_EXTENSIONDIFFERENT       0x00000400L
#define OFN_PATHMUSTEXIST            0x00000800L
#define OFN_FILEMUSTEXIST            0x00001000L
#define OFN_CREATEPROMPT             0x00002000L
#define OFN_SHAREAWARE               0x00004000L
#define OFN_NOREADONLYRETURN         0x00008000L
#define OFN_NOTESTFILECREATE         0x00010000L

/* Return values for the registered message sent to the hook function
 * when a sharing violation occurs.  OFN_SHAREFALLTHROUGH allows the
 * filename to be accepted, OFN_SHARENOWARN rejects the name but puts
 * up no warning (returned when the app has already put up a warning
 * message), and OFN_SHAREWARN puts up the default warning message
 * for sharing violations.
 *
 * Note:  Undefined return values map to OFN_SHAREWARN, but are
 *        reserved for future use.
 */

#define OFN_SHAREFALLTHROUGH     2
#define OFN_SHARENOWARN          1
#define OFN_SHAREWARN            0

typedef struct tagCHOOSECOLOR
{
    DWORD   lStructSize;
    HWND    hwndOwner;
    HWND    hInstance;
    COLORREF rgbResult;
    COLORREF FAR* lpCustColors;
    DWORD   Flags;
    LPARAM  lCustData;
    UINT    (CALLBACK* lpfnHook)(HWND, UINT, WPARAM, LPARAM);
    LPCSTR  lpTemplateName;
} CHOOSECOLOR;
typedef CHOOSECOLOR FAR *LPCHOOSECOLOR;

BOOL    WINAPI ChooseColor(CHOOSECOLOR FAR*);

#define CC_RGBINIT               0x00000001L
#define CC_FULLOPEN              0x00000002L
#define CC_PREVENTFULLOPEN       0x00000004L
#define CC_SHOWHELP              0x00000008L
#define CC_ENABLEHOOK            0x00000010L
#define CC_ENABLETEMPLATE        0x00000020L
#define CC_ENABLETEMPLATEHANDLE  0x00000040L

typedef struct tagFINDREPLACE
{
    DWORD    lStructSize;            /* size of this struct 0x20 */
    HWND     hwndOwner;              /* handle to owner's window */
    HINSTANCE hInstance;             /* instance handle of.EXE that
                                      * contains cust. dlg. template
                                      */
    DWORD    Flags;                  /* one or more of the FR_?? */
    LPSTR    lpstrFindWhat;          /* ptr. to search string    */
    LPSTR    lpstrReplaceWith;       /* ptr. to replace string   */
    UINT     wFindWhatLen;           /* size of find buffer      */
    UINT     wReplaceWithLen;        /* size of replace buffer   */
    LPARAM   lCustData;              /* data passed to hook fn.  */
    UINT    (CALLBACK* lpfnHook)(HWND, UINT, WPARAM, LPARAM);
                                     /* ptr. to hook fn. or NULL */
    LPCSTR   lpTemplateName;         /* custom template name     */
} FINDREPLACE;
typedef FINDREPLACE FAR *LPFINDREPLACE;

#define FR_DOWN                         0x00000001L
#define FR_WHOLEWORD                    0x00000002L
#define FR_MATCHCASE                    0x00000004L
#define FR_FINDNEXT                     0x00000008L
#define FR_REPLACE                      0x00000010L
#define FR_REPLACEALL                   0x00000020L
#define FR_DIALOGTERM                   0x00000040L
#define FR_SHOWHELP                     0x00000080L
#define FR_ENABLEHOOK                   0x00000100L
#define FR_ENABLETEMPLATE               0x00000200L
#define FR_NOUPDOWN                     0x00000400L
#define FR_NOMATCHCASE                  0x00000800L
#define FR_NOWHOLEWORD                  0x00001000L
#define FR_ENABLETEMPLATEHANDLE         0x00002000L
#define FR_HIDEUPDOWN                   0x00004000L
#define FR_HIDEMATCHCASE                0x00008000L
#define FR_HIDEWHOLEWORD                0x00010000L

HWND    WINAPI FindText(FINDREPLACE FAR*);
HWND    WINAPI ReplaceText(FINDREPLACE FAR*);

typedef struct tagCHOOSEFONT
{
    DWORD           lStructSize;        /* */
    HWND            hwndOwner;          /* caller's window handle   */
    HDC             hDC;                /* printer DC/IC or NULL    */
    LOGFONT FAR*    lpLogFont;          /* ptr. to a LOGFONT struct */
    int             iPointSize;         /* 10 * size in points of selected font */
    DWORD           Flags;              /* enum. type flags         */
    COLORREF        rgbColors;          /* returned text color      */
    LPARAM          lCustData;          /* data passed to hook fn.  */
    UINT (CALLBACK* lpfnHook)(HWND, UINT, WPARAM, LPARAM);
                                        /* ptr. to hook function    */
    LPCSTR          lpTemplateName;     /* custom template name     */
    HINSTANCE       hInstance;          /* instance handle of.EXE that
                                         * contains cust. dlg. template
                                         */
    LPSTR           lpszStyle;          /* return the style field here
                                         * must be LF_FACESIZE or bigger */
    UINT            nFontType;          /* same value reported to the EnumFonts
                                         * call back with the extra FONTTYPE_
                                         * bits added */
    int             nSizeMin;           /* minimum pt size allowed & */
    int             nSizeMax;           /* max pt size allowed if    */
                                        /* CF_LIMITSIZE is used      */
} CHOOSEFONT;
typedef CHOOSEFONT FAR *LPCHOOSEFONT;

BOOL WINAPI ChooseFont(CHOOSEFONT FAR*);

#define CF_SCREENFONTS               0x00000001L
#define CF_PRINTERFONTS              0x00000002L
#define CF_BOTH                      (CF_SCREENFONTS | CF_PRINTERFONTS)
#define CF_SHOWHELP                  0x00000004L
#define CF_ENABLEHOOK                0x00000008L
#define CF_ENABLETEMPLATE            0x00000010L
#define CF_ENABLETEMPLATEHANDLE      0x00000020L
#define CF_INITTOLOGFONTSTRUCT       0x00000040L
#define CF_USESTYLE                  0x00000080L
#define CF_EFFECTS                   0x00000100L
#define CF_APPLY                     0x00000200L
#define CF_ANSIONLY                  0x00000400L
#define CF_NOVECTORFONTS             0x00000800L
#define CF_NOOEMFONTS                CF_NOVECTORFONTS
#define CF_NOSIMULATIONS             0x00001000L
#define CF_LIMITSIZE                 0x00002000L
#define CF_FIXEDPITCHONLY            0x00004000L
#define CF_WYSIWYG                   0x00008000L /* must also have CF_SCREENFONTS & CF_PRINTERFONTS */
#define CF_FORCEFONTEXIST            0x00010000L
#define CF_SCALABLEONLY              0x00020000L
#define CF_TTONLY                    0x00040000L
#define CF_NOFACESEL                 0x00080000L
#define CF_NOSTYLESEL                0x00100000L
#define CF_NOSIZESEL                 0x00200000L

/* these are extra nFontType bits that are added to what is returned to the
 * EnumFonts callback routine */

#define SIMULATED_FONTTYPE      0x8000
#define PRINTER_FONTTYPE        0x4000
#define SCREEN_FONTTYPE         0x2000
#define BOLD_FONTTYPE           0x0100
#define ITALIC_FONTTYPE         0x0200
#define REGULAR_FONTTYPE        0x0400

#define WM_CHOOSEFONT_GETLOGFONT        (WM_USER + 1)


/* strings used to obtain unique window message for communication
 * between dialog and caller
 */
#define LBSELCHSTRING  "commdlg_LBSelChangedNotify"
#define SHAREVISTRING  "commdlg_ShareViolation"
#define FILEOKSTRING   "commdlg_FileNameOK"
#define COLOROKSTRING  "commdlg_ColorOK"
#define SETRGBSTRING   "commdlg_SetRGBColor"
#define FINDMSGSTRING  "commdlg_FindReplace"
#define HELPMSGSTRING  "commdlg_help"

/* HIWORD values for lParam of commdlg_LBSelChangeNotify message */
#define CD_LBSELNOITEMS -1
#define CD_LBSELCHANGE   0
#define CD_LBSELSUB      1
#define CD_LBSELADD      2

typedef struct tagPD
{
    DWORD   lStructSize;
    HWND    hwndOwner;
    HGLOBAL hDevMode;
    HGLOBAL hDevNames;
    HDC     hDC;
    DWORD   Flags;
    UINT    nFromPage;
    UINT    nToPage;
    UINT    nMinPage;
    UINT    nMaxPage;
    UINT    nCopies;
    HINSTANCE hInstance;
    LPARAM  lCustData;
    UINT    (CALLBACK* lpfnPrintHook)(HWND, UINT, WPARAM, LPARAM);
    UINT    (CALLBACK* lpfnSetupHook)(HWND, UINT, WPARAM, LPARAM);
    LPCSTR  lpPrintTemplateName;
    LPCSTR  lpSetupTemplateName;
    HGLOBAL hPrintTemplate;
    HGLOBAL hSetupTemplate;
} PRINTDLG;
typedef PRINTDLG  FAR* LPPRINTDLG;

BOOL    WINAPI PrintDlg(PRINTDLG FAR*);

#define PD_ALLPAGES                  0x00000000L
#define PD_SELECTION                 0x00000001L
#define PD_PAGENUMS                  0x00000002L
#define PD_NOSELECTION               0x00000004L
#define PD_NOPAGENUMS                0x00000008L
#define PD_COLLATE                   0x00000010L
#define PD_PRINTTOFILE               0x00000020L
#define PD_PRINTSETUP                0x00000040L
#define PD_NOWARNING                 0x00000080L
#define PD_RETURNDC                  0x00000100L
#define PD_RETURNIC                  0x00000200L
#define PD_RETURNDEFAULT             0x00000400L
#define PD_SHOWHELP                  0x00000800L
#define PD_ENABLEPRINTHOOK           0x00001000L
#define PD_ENABLESETUPHOOK           0x00002000L
#define PD_ENABLEPRINTTEMPLATE       0x00004000L
#define PD_ENABLESETUPTEMPLATE       0x00008000L
#define PD_ENABLEPRINTTEMPLATEHANDLE 0x00010000L
#define PD_ENABLESETUPTEMPLATEHANDLE 0x00020000L
#define PD_USEDEVMODECOPIES          0x00040000L
#define PD_DISABLEPRINTTOFILE        0x00080000L
#define PD_HIDEPRINTTOFILE           0x00100000L

typedef struct tagDEVNAMES
{
    UINT wDriverOffset;
    UINT wDeviceOffset;
    UINT wOutputOffset;
    UINT wDefault;
} DEVNAMES;
typedef DEVNAMES FAR* LPDEVNAMES;

#define DN_DEFAULTPRN      0x0001

DWORD   WINAPI CommDlgExtendedError(void);

#else    /* defined __FLAT__  */

typedef UINT (APIENTRY *LPOFNHOOKPROC) (HWND, UINT, WPARAM, LPARAM);

typedef struct tagOFNA {
   DWORD   lStructSize;
   HWND    hwndOwner;
   HINSTANCE  hInstance;
   LPCSTR   lpstrFilter;
   LPSTR   lpstrCustomFilter;
   DWORD   nMaxCustFilter;
   DWORD   nFilterIndex;
   LPSTR   lpstrFile;
   DWORD   nMaxFile;
   LPSTR   lpstrFileTitle;
   DWORD   nMaxFileTitle;
   LPCSTR   lpstrInitialDir;
   LPCSTR   lpstrTitle;
   DWORD   Flags;
   WORD    nFileOffset;
   WORD    nFileExtension;
   LPCSTR   lpstrDefExt;
   LPARAM   lCustData;
   LPOFNHOOKPROC lpfnHook;
   LPCSTR   lpTemplateName;
} OPENFILENAMEA;

typedef struct tagOFNW {
   DWORD   lStructSize;
   HWND    hwndOwner;
   HINSTANCE  hInstance;
   LPCWSTR  lpstrFilter;
   LPWSTR  lpstrCustomFilter;
   DWORD   nMaxCustFilter;
   DWORD   nFilterIndex;
   LPWSTR  lpstrFile;
   DWORD   nMaxFile;
   LPWSTR  lpstrFileTitle;
   DWORD   nMaxFileTitle;
   LPCWSTR  lpstrInitialDir;
   LPCWSTR  lpstrTitle;
   DWORD   Flags;
   WORD    nFileOffset;
   WORD    nFileExtension;
   LPCWSTR  lpstrDefExt;
   LPARAM   lCustData;
   LPOFNHOOKPROC lpfnHook;
   LPCWSTR   lpTemplateName;
} OPENFILENAMEW;

#ifdef UNICODE
#define OPENFILENAME OPENFILENAMEW
#else
#define OPENFILENAME OPENFILENAMEA
#endif // ! UNICODE

typedef OPENFILENAMEA * LPOPENFILENAMEA;
typedef OPENFILENAMEW * LPOPENFILENAMEW;
typedef OPENFILENAME * LPOPENFILENAME;

BOOL  APIENTRY     GetOpenFileNameA(LPOPENFILENAMEA);
BOOL  APIENTRY     GetOpenFileNameW(LPOPENFILENAMEW);

#ifdef UNICODE
#define GetOpenFileName GetOpenFileNameW
#else
#define GetOpenFileName GetOpenFileNameA
#endif // ! UNICODE

BOOL  APIENTRY     GetSaveFileNameA(LPOPENFILENAMEA);
BOOL  APIENTRY     GetSaveFileNameW(LPOPENFILENAMEW);
#ifdef UNICODE
#define GetSaveFileName GetSaveFileNameW
#else
#define GetSaveFileName GetSaveFileNameA
#endif // ! UNICODE

short APIENTRY     GetFileTitleA(LPCSTR, LPSTR, WORD);
short APIENTRY     GetFileTitleW(LPCWSTR, LPWSTR, WORD);

#ifdef UNICODE
#define GetFileTitle GetFileTitleW
#else
#define GetFileTitle GetFileTitleA
#endif // ! UNICODE

#define OFN_READONLY                 0x00000001
#define OFN_OVERWRITEPROMPT          0x00000002
#define OFN_HIDEREADONLY             0x00000004
#define OFN_NOCHANGEDIR              0x00000008
#define OFN_SHOWHELP                 0x00000010
#define OFN_ENABLEHOOK               0x00000020
#define OFN_ENABLETEMPLATE           0x00000040
#define OFN_ENABLETEMPLATEHANDLE     0x00000080
#define OFN_NOVALIDATE               0x00000100
#define OFN_ALLOWMULTISELECT         0x00000200
#define OFN_EXTENSIONDIFFERENT       0x00000400
#define OFN_PATHMUSTEXIST            0x00000800
#define OFN_FILEMUSTEXIST            0x00001000
#define OFN_CREATEPROMPT             0x00002000
#define OFN_SHAREAWARE               0x00004000
#define OFN_NOREADONLYRETURN         0x00008000
#define OFN_NOTESTFILECREATE         0x00010000
#define OFN_NONETWORKBUTTON          0x00020000
#define OFN_NOLONGNAMES              0x00040000

// Return values for the registered message sent to the hook function
// when a sharing violation occurs.  OFN_SHAREFALLTHROUGH allows the
// filename to be accepted, OFN_SHARENOWARN rejects the name but puts
// up no warning (returned when the app has already put up a warning
// message), and OFN_SHAREWARN puts up the default warning message
// for sharing violations.
//
// Note:  Undefined return values map to OFN_SHAREWARN, but are
//        reserved for future use.

#define OFN_SHAREFALLTHROUGH     2
#define OFN_SHARENOWARN          1
#define OFN_SHAREWARN            0

typedef UINT (APIENTRY *LPCCHOOKPROC) (HWND, UINT, WPARAM, LPARAM);

typedef struct tagCHOOSECOLORA {
   DWORD   lStructSize;
   HWND    hwndOwner;
   HWND    hInstance;
   COLORREF  rgbResult;
   COLORREF* lpCustColors;
   DWORD   Flags;
   LPARAM  lCustData;
   LPCCHOOKPROC lpfnHook;
   LPCSTR   lpTemplateName;
} CHOOSECOLORA;

typedef struct tagCHOOSECOLORW {
   DWORD   lStructSize;
   HWND    hwndOwner;
   HWND    hInstance;
   COLORREF  rgbResult;
   COLORREF* lpCustColors;
   DWORD   Flags;
   LPARAM  lCustData;
   LPCCHOOKPROC lpfnHook;
   LPCWSTR   lpTemplateName;
} CHOOSECOLORW;

#ifdef UNICODE
#define CHOOSECOLOR CHOOSECOLORW
#else
#define CHOOSECOLOR CHOOSECOLORA
#endif // ! UNICODE

typedef CHOOSECOLORA *LPCHOOSECOLORA;
typedef CHOOSECOLORW *LPCHOOSECOLORW;
typedef CHOOSECOLOR *LPCHOOSECOLOR;

BOOL  APIENTRY ChooseColorA(LPCHOOSECOLORA);
BOOL  APIENTRY ChooseColorW(LPCHOOSECOLORW);

#ifdef UNICODE
#define ChooseColor ChooseColorW
#else
#define ChooseColor ChooseColorA
#endif // ! UNICODE

#define CC_RGBINIT               0x00000001
#define CC_FULLOPEN              0x00000002
#define CC_PREVENTFULLOPEN       0x00000004
#define CC_SHOWHELP              0x00000008
#define CC_ENABLEHOOK            0x00000010
#define CC_ENABLETEMPLATE        0x00000020
#define CC_ENABLETEMPLATEHANDLE  0x00000040

typedef UINT (APIENTRY *LPFRHOOKPROC) (HWND, UINT, WPARAM, LPARAM);

typedef struct tagFINDREPLACEA {
   DWORD    lStructSize;        // size of this struct 0x20
   HWND     hwndOwner;          // handle to owner's window
   HINSTANCE hInstance;          // instance handle of.EXE that
                                //   contains cust. dlg. template
   DWORD    Flags;              // one or more of the FR_??
   LPSTR    lpstrFindWhat;      // ptr. to search string
   LPSTR    lpstrReplaceWith;   // ptr. to replace string
   WORD     wFindWhatLen;       // size of find buffer
   WORD     wReplaceWithLen;    // size of replace buffer
   LPARAM   lCustData;          // data passed to hook fn.
   LPFRHOOKPROC lpfnHook;       // ptr. to hook fn. or NULL
   LPCSTR    lpTemplateName;     // custom template name
} FINDREPLACEA;

typedef struct tagFINDREPLACEW {
   DWORD    lStructSize;        // size of this struct 0x20
   HWND     hwndOwner;          // handle to owner's window
   HINSTANCE hInstance;          // instance handle of.EXE that
                                //   contains cust. dlg. template
   DWORD    Flags;              // one or more of the FR_??
   LPWSTR   lpstrFindWhat;      // ptr. to search string
   LPWSTR   lpstrReplaceWith;   // ptr. to replace string
   WORD     wFindWhatLen;       // size of find buffer
   WORD     wReplaceWithLen;    // size of replace buffer
   LPARAM   lCustData;          // data passed to hook fn.
   LPFRHOOKPROC lpfnHook;       // ptr. to hook fn. or NULL
   LPCWSTR   lpTemplateName;     // custom template name
} FINDREPLACEW;

#ifdef UNICODE
#define FINDREPLACE FINDREPLACEW
#else
#define FINDREPLACE FINDREPLACEA
#endif // ! UNICODE

typedef FINDREPLACEA  *LPFINDREPLACEA;
typedef FINDREPLACEW *LPFINDREPLACEW;
typedef FINDREPLACE *LPFINDREPLACE;

#define FR_DOWN                         0x00000001
#define FR_WHOLEWORD                    0x00000002
#define FR_MATCHCASE                    0x00000004
#define FR_FINDNEXT                     0x00000008
#define FR_REPLACE                      0x00000010
#define FR_REPLACEALL                   0x00000020
#define FR_DIALOGTERM                   0x00000040
#define FR_SHOWHELP                     0x00000080
#define FR_ENABLEHOOK                   0x00000100
#define FR_ENABLETEMPLATE               0x00000200
#define FR_NOUPDOWN                     0x00000400
#define FR_NOMATCHCASE                  0x00000800
#define FR_NOWHOLEWORD                  0x00001000
#define FR_ENABLETEMPLATEHANDLE         0x00002000
#define FR_HIDEUPDOWN                   0x00004000
#define FR_HIDEMATCHCASE                0x00008000
#define FR_HIDEWHOLEWORD                0x00010000

HWND  APIENTRY    FindTextA(LPFINDREPLACEA);
HWND  APIENTRY    FindTextW(LPFINDREPLACEW);

#ifdef UNICODE
#define FindText FindTextW
#else
#define FindText FindTextA
#endif // ! UNICODE

HWND  APIENTRY    ReplaceTextA(LPFINDREPLACEA);
HWND  APIENTRY    ReplaceTextW(LPFINDREPLACEW);

#ifdef UNICODE
#define ReplaceText ReplaceTextW
#else
#define ReplaceText ReplaceTextA
#endif // ! UNICODE

typedef UINT (APIENTRY *LPCFHOOKPROC) (HWND, UINT, WPARAM, LPARAM);

typedef struct tagCHOOSEFONTA {
   DWORD           lStructSize;
   HWND            hwndOwner;          // caller's window handle
   HDC             hDC;                // printer DC/IC or NULL
   LPLOGFONTA      lpLogFont;          // ptr. to a LOGFONT struct
   INT             iPointSize;         // 10 * size in points of selected font
   DWORD           Flags;              // enum. type flags
   COLORREF        rgbColors;          // returned text color
   LPARAM          lCustData;          // data passed to hook fn.
   LPCFHOOKPROC    lpfnHook;           // ptr. to hook function
   LPCSTR           lpTemplateName;     // custom template name
   HINSTANCE       hInstance;          // instance handle of.EXE that
                                       //   contains cust. dlg. template
   LPSTR           lpszStyle;          // return the style field here
                                       // must be LF_FACESIZE or bigger
   WORD            nFontType;          // same value reported to the EnumFonts
                                       //   call back with the extra FONTTYPE_
                                       //   bits added
   WORD            ___MISSING_ALIGNMENT__;
   INT             nSizeMin;           // minimum pt size allowed &
   INT             nSizeMax;           // max pt size allowed if
                                       //   CF_LIMITSIZE is used
} CHOOSEFONTA;

typedef struct tagCHOOSEFONTW {
   DWORD           lStructSize;
   HWND            hwndOwner;          // caller's window handle
   HDC             hDC;                // printer DC/IC or NULL
   LPLOGFONTW      lpLogFont;          // ptr. to a LOGFONT struct
   INT             iPointSize;         // 10 * size in points of selected font
   DWORD           Flags;              // enum. type flags
   COLORREF        rgbColors;          // returned text color
   LPARAM          lCustData;          // data passed to hook fn.
   LPCFHOOKPROC lpfnHook;              // ptr. to hook function
   LPCWSTR          lpTemplateName;     // custom template name
   HINSTANCE       hInstance;          // instance handle of.EXE that
                                       // contains cust. dlg. template
   LPWSTR          lpszStyle;          // return the style field here
                                       // must be LF_FACESIZE or bigger
   WORD            nFontType;          // same value reported to the EnumFonts
                                       //   call back with the extra FONTTYPE_
                                       //   bits added
   WORD            ___MISSING_ALIGNMENT__;
   INT             nSizeMin;           // minimum pt size allowed &
   INT             nSizeMax;           // max pt size allowed if
                                       //   CF_LIMITSIZE is used
} CHOOSEFONTW;

#ifdef UNICODE
#define CHOOSEFONT CHOOSEFONTW
#else
#define CHOOSEFONT CHOOSEFONTA
#endif // ! UNICODE

typedef CHOOSEFONTA *LPCHOOSEFONTA;
typedef CHOOSEFONTW *LPCHOOSEFONTW;
typedef CHOOSEFONT *LPCHOOSEFONT;

BOOL APIENTRY ChooseFontA(LPCHOOSEFONTA);
BOOL APIENTRY ChooseFontW(LPCHOOSEFONTW);

#ifdef UNICODE
#define ChooseFont ChooseFontW
#else
#define ChooseFont ChooseFontA
#endif // !UNICODE

#define CF_SCREENFONTS             0x00000001
#define CF_PRINTERFONTS            0x00000002
#define CF_BOTH                    (CF_SCREENFONTS | CF_PRINTERFONTS)
#define CF_SHOWHELP                0x00000004L
#define CF_ENABLEHOOK              0x00000008L
#define CF_ENABLETEMPLATE          0x00000010L
#define CF_ENABLETEMPLATEHANDLE    0x00000020L
#define CF_INITTOLOGFONTSTRUCT     0x00000040L
#define CF_USESTYLE                0x00000080L
#define CF_EFFECTS                 0x00000100L
#define CF_APPLY                   0x00000200L
#define CF_ANSIONLY                0x00000400L
#define CF_NOVECTORFONTS           0x00000800L
#define CF_NOOEMFONTS              CF_NOVECTORFONTS
#define CF_NOSIMULATIONS           0x00001000L
#define CF_LIMITSIZE               0x00002000L
#define CF_FIXEDPITCHONLY          0x00004000L
#define CF_WYSIWYG                 0x00008000L // must also have CF_SCREENFONTS & CF_PRINTERFONTS
#define CF_FORCEFONTEXIST          0x00010000L
#define CF_SCALABLEONLY            0x00020000L
#define CF_TTONLY                  0x00040000L
#define CF_NOFACESEL               0x00080000L
#define CF_NOSTYLESEL              0x00100000L
#define CF_NOSIZESEL               0x00200000L

// these are extra nFontType bits that are added to what is returned to the
// EnumFonts callback routine

#define SIMULATED_FONTTYPE    0x8000
#define PRINTER_FONTTYPE      0x4000
#define SCREEN_FONTTYPE       0x2000
#define BOLD_FONTTYPE         0x0100
#define ITALIC_FONTTYPE       0x0200
#define REGULAR_FONTTYPE      0x0400

#define WM_CHOOSEFONT_GETLOGFONT      (WM_USER + 1)

// strings used to obtain unique window message for communication
// between dialog and caller

#define LBSELCHSTRINGA  "commdlg_LBSelChangedNotify"
#define SHAREVISTRINGA  "commdlg_ShareViolation"
#define FILEOKSTRINGA   "commdlg_FileNameOK"
#define COLOROKSTRINGA  "commdlg_ColorOK"
#define SETRGBSTRINGA   "commdlg_SetRGBColor"
#define HELPMSGSTRINGA  "commdlg_help"
#define FINDMSGSTRINGA  "commdlg_FindReplace"

#define LBSELCHSTRINGW  L"commdlg_LBSelChangedNotify"
#define SHAREVISTRINGW  L"commdlg_ShareViolation"
#define FILEOKSTRINGW   L"commdlg_FileNameOK"
#define COLOROKSTRINGW  L"commdlg_ColorOK"
#define SETRGBSTRINGW   L"commdlg_SetRGBColor"
#define HELPMSGSTRINGW  L"commdlg_help"
#define FINDMSGSTRINGW  L"commdlg_FindReplace"

#ifdef UNICODE
#define LBSELCHSTRING  LBSELCHSTRINGW
#define SHAREVISTRING  SHAREVISTRINGW
#define FILEOKSTRING   FILEOKSTRINGW
#define COLOROKSTRING  COLOROKSTRINGW
#define SETRGBSTRING   SETRGBSTRINGW
#define HELPMSGSTRING  HELPMSGSTRINGW
#define FINDMSGSTRING  FINDMSGSTRINGW
#else
#define LBSELCHSTRING  LBSELCHSTRINGA
#define SHAREVISTRING  SHAREVISTRINGA
#define FILEOKSTRING   FILEOKSTRINGA
#define COLOROKSTRING  COLOROKSTRINGA
#define SETRGBSTRING   SETRGBSTRINGA
#define HELPMSGSTRING  HELPMSGSTRINGA
#define FINDMSGSTRING  FINDMSGSTRINGA
#endif

// HIWORD values for lParam of commdlg_LBSelChangeNotify message
#define CD_LBSELNOITEMS -1
#define CD_LBSELCHANGE   0
#define CD_LBSELSUB      1
#define CD_LBSELADD      2

typedef UINT (APIENTRY *LPPRINTHOOKPROC) (HWND, UINT, WPARAM, LPARAM);
typedef UINT (APIENTRY *LPSETUPHOOKPROC) (HWND, UINT, WPARAM, LPARAM);

typedef struct tagPDA {
   DWORD   lStructSize;
   HWND    hwndOwner;
   HGLOBAL hDevMode;
   HGLOBAL hDevNames;
   HDC     hDC;
   DWORD   Flags;
   WORD    nFromPage;
   WORD    nToPage;
   WORD    nMinPage;
   WORD    nMaxPage;
   WORD    nCopies;
   HINSTANCE hInstance;
   LPARAM  lCustData;
   LPPRINTHOOKPROC lpfnPrintHook;
   LPSETUPHOOKPROC lpfnSetupHook;
   LPCSTR   lpPrintTemplateName;
   LPCSTR   lpSetupTemplateName;
   HGLOBAL  hPrintTemplate;
   HGLOBAL  hSetupTemplate;
} PRINTDLGA;

typedef struct tagPDW {
   DWORD   lStructSize;
   HWND    hwndOwner;
   HGLOBAL hDevMode;
   HGLOBAL  hDevNames;
   HDC     hDC;
   DWORD   Flags;
   WORD    nFromPage;
   WORD    nToPage;
   WORD    nMinPage;
   WORD    nMaxPage;
   WORD    nCopies;
   HINSTANCE hInstance;
   LPARAM  lCustData;
   LPPRINTHOOKPROC lpfnPrintHook;
   LPSETUPHOOKPROC lpfnSetupHook;
   LPCWSTR  lpPrintTemplateName;
   LPCWSTR  lpSetupTemplateName;
   HGLOBAL  hPrintTemplate;
   HGLOBAL  hSetupTemplate;
}  PRINTDLGW;

#ifdef UNICODE
#define PRINTDLG PRINTDLGW
#else
#define PRINTDLG PRINTDLGA
#endif // ! UNICODE

typedef PRINTDLGA * LPPRINTDLGA;
typedef PRINTDLGW * LPPRINTDLGW;
typedef PRINTDLG  * LPPRINTDLG;

BOOL  APIENTRY     PrintDlgA(LPPRINTDLGA);
BOOL  APIENTRY     PrintDlgW(LPPRINTDLGW);

#ifdef UNICODE
#define PrintDlg PrintDlgW
#else
#define PrintDlg PrintDlgA
#endif // ! UNICODE

#define PD_ALLPAGES                  0x00000000
#define PD_SELECTION                 0x00000001
#define PD_PAGENUMS                  0x00000002
#define PD_NOSELECTION               0x00000004
#define PD_NOPAGENUMS                0x00000008
#define PD_COLLATE                   0x00000010
#define PD_PRINTTOFILE               0x00000020
#define PD_PRINTSETUP                0x00000040
#define PD_NOWARNING                 0x00000080
#define PD_RETURNDC                  0x00000100
#define PD_RETURNIC                  0x00000200
#define PD_RETURNDEFAULT             0x00000400
#define PD_SHOWHELP                  0x00000800
#define PD_ENABLEPRINTHOOK           0x00001000
#define PD_ENABLESETUPHOOK           0x00002000
#define PD_ENABLEPRINTTEMPLATE       0x00004000
#define PD_ENABLESETUPTEMPLATE       0x00008000
#define PD_ENABLEPRINTTEMPLATEHANDLE 0x00010000
#define PD_ENABLESETUPTEMPLATEHANDLE 0x00020000
#define PD_USEDEVMODECOPIES          0x00040000
#define PD_DISABLEPRINTTOFILE        0x00080000
#define PD_HIDEPRINTTOFILE           0x00100000
#define PD_NONETWORKBUTTON           0x00200000

typedef struct tagDEVNAMES {
   WORD wDriverOffset;
   WORD wDeviceOffset;
   WORD wOutputOffset;
   WORD wDefault;
} DEVNAMES;

typedef DEVNAMES * LPDEVNAMES;

#define DN_DEFAULTPRN      0x0001

DWORD APIENTRY     CommDlgExtendedError(VOID);


#endif  /* __FLAT__  */

#ifdef __cplusplus
}                                                                /* end extern "C"  */
#endif  /* __cplusplus */

#ifndef RC_INVOKED
#pragma option -a.      /* Revert to default packing */
#endif  /* RC_INVOKED */

#pragma warn .nak   /* Ignore non-ansi keywords */

#endif  /* __COMMDLG_H */

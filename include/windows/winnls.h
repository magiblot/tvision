/*++ BUILD Version: 0003    // Increment this if a change has global effects

Copyright (c) 1991, Microsoft Corporation

Module Name:

    winnls.h

Abstract:

    Procedure declarations, constant definitions, and macros for the
    NLS component.

Author:

    Julie Bennett (julieb) 31-May-1991

--*/

#ifndef __WINNLS_H
#define __WINNLS_H


#ifndef __WINDEF_H
#include <windef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NONLS


/***************************************************************************\
* Constants
*
* Define all constants for the NLS component here.
\***************************************************************************/

/*
 *  String Length Maximums.
 */
#define MAX_LEADBYTES        12             /* 5 ranges, 2 bytes ea., 0 term. */
#define MAX_DEFAULTCHAR      2              /* single or double byte */


/*
 *  MBCS and Unicode Translation Flags.
 */
#define MB_PRECOMPOSED       0x00000001     /* use precomposed chars */
#define MB_COMPOSITE         0x00000002     /* use composite chars */
#define MB_USEGLYPHCHARS     0x00000004     /* use glyph chars, not ctrl chars */

#define WC_DEFAULTCHECK      0x00000100     /* check for default char */
#define WC_COMPOSITECHECK    0x00000200     /* convert composite to precomposed */
#define WC_DISCARDNS         0x00000010     /* discard non-spacing chars */
#define WC_SEPCHARS          0x00000020     /* generate separate chars */
#define WC_DEFAULTCHAR       0x00000040     /* replace w/ default char */



/*
 *  Character Type Flags.
 */
#define CT_CTYPE1            0x00000001     /* ctype 1 information */
#define CT_CTYPE2            0x00000002     /* ctype 2 information */
#define CT_CTYPE3            0x00000004     /* ctype 3 information */

/*
 *  CType 1 Flag Bits.
 */
#define C1_UPPER             0x0001         /* upper case */
#define C1_LOWER             0x0002         /* lower case */
#define C1_DIGIT             0x0004         /* decimal digits */
#define C1_SPACE             0x0008         /* spacing characters */
#define C1_PUNCT             0x0010         /* punctuation characters */
#define C1_CNTRL             0x0020         /* control characters */
#define C1_BLANK             0x0040         /* blank characters */
#define C1_XDIGIT            0x0080         /* other digits */
#define C1_ALPHA             0x0100         /* any letter */

/*
 *  CType 2 Flag Bits.
 */
#define C2_LEFTTORIGHT       0x0001         /* left to right */
#define C2_RIGHTTOLEFT       0x0002         /* right to left */

#define C2_EUROPENUMBER      0x0003         /* European number, digit */
#define C2_EUROPESEPARATOR   0x0004         /* European numeric separator */
#define C2_EUROPETERMINATOR  0x0005         /* European numeric terminator */
#define C2_ARABICNUMBER      0x0006         /* Arabic number */
#define C2_COMMONSEPARATOR   0x0007         /* common numeric separator */

#define C2_BLOCKSEPARATOR    0x0008         /* block separator */
#define C2_SEGMENTSEPARATOR  0x0009         /* segment separator */
#define C2_WHITESPACE        0x000A         /* white space */
#define C2_OTHERNEUTRAL      0x000B         /* other neutrals */

#define C2_NOTAPPLICABLE     0x0000         /* no implicit directionality */

/*
 *  CType 3 Flag Bits.
 */
#define C3_NONSPACING        0x0001         /* nonspacing character */
#define C3_DIACRITIC         0x0002         /* diacritic mark */
#define C3_VOWELMARK         0x0004         /* vowel mark */
#define C3_SYMBOL            0x0008         /* symbols */

#define C3_NOTAPPLICABLE     0x0000         /* ctype 3 is not applicable */


/*
 *  String Flags.
 */
#define NORM_IGNORECASE      0x00000001     /* ignore case */
#define NORM_IGNORENONSPACE  0x00000002     /* ignore nonspacing chars */
#define NORM_IGNORESYMBOLS   0x00000004     /* ignore symbols */


/*
 *  Locale Independent Mapping Flags.
 */
#define MAP_FOLDCZONE        0x00000010     /* fold compatibility zone chars */
#define MAP_PRECOMPOSED      0x00000020     /* convert to precomposed chars */
#define MAP_COMPOSITE        0x00000040     /* convert to composite chars */
#define MAP_FOLDDIGITS       0x00000080     /* all digits to ASCII 0-9 */


/*
 *  Locale Dependent Mapping Flags.
 */
#define LCMAP_LOWERCASE      0x00000100     /* lower case letters */
#define LCMAP_UPPERCASE      0x00000200     /* upper case letters */
#define LCMAP_SORTKEY        0x00000400     /* WC sort key (normalize) */
#define LCMAP_BYTEREV        0x00000800     /* byte reversal */


/*
 *  Sorting Flags.
 *
 *    WORD Sort:    culturally correct sort
 *                  hyphen and apostrophe are special cased
 *                  example: "coop" and "co-op" will sort together in a list
 *
 *                        co_op     <-------  underscore (symbol)
 *                        coat
 *                        comb
 *                        coop
 *                        co-op     <-------  hyphen (punctuation)
 *                        cork
 *                        went
 *                        were
 *                        we're     <-------  apostrophe (punctuation)
 *
 *
 *    STRING Sort:  hyphen and apostrophe will sort with all other symbols
 *
 *                        co-op     <-------  hyphen (punctuation)
 *                        co_op     <-------  underscore (symbol)
 *                        coat
 *                        comb
 *                        coop
 *                        cork
 *                        we're     <-------  apostrophe (punctuation)
 *                        went
 *                        were
 */
#define SORT_STRINGSORT      0x00001000     /* use string sort method */


/*
 *  Code Page Default Values.
 */
#define CP_ACP               0              /* default to ANSI code page */
#define CP_OEMCP             1              /* default to OEM  code page */


/*
 *  Country Codes.
 */
#define CTRY_DEFAULT                     0

#define CTRY_AUSTRALIA                   61      /* Australia */
#define CTRY_AUSTRIA                     43      /* Austria */
#define CTRY_BELGIUM                     32      /* Belgium */
#define CTRY_BRAZIL                      55      /* Brazil */
#define CTRY_CANADA                      2       /* Canada */
#define CTRY_DENMARK                     45      /* Denmark */
#define CTRY_FINLAND                     358     /* Finland */
#define CTRY_FRANCE                      33      /* France */
#define CTRY_GERMANY                     49      /* Germany */
#define CTRY_ICELAND                     354     /* Iceland */
#define CTRY_IRELAND                     353     /* Ireland */
#define CTRY_ITALY                       39      /* Italy */
#define CTRY_JAPAN                       81      /* Japan */
#define CTRY_MEXICO                      52      /* Mexico */
#define CTRY_NETHERLANDS                 31      /* Netherlands */
#define CTRY_NEW_ZEALAND                 64      /* New Zealand */
#define CTRY_NORWAY                      47      /* Norway */
#define CTRY_PORTUGAL                    351     /* Portugal */
#define CTRY_PRCHINA                     86      /* PR China */
#define CTRY_SOUTH_KOREA                 82      /* South Korea */
#define CTRY_SPAIN                       34      /* Spain */
#define CTRY_SWEDEN                      46      /* Sweden */
#define CTRY_SWITZERLAND                 41      /* Switzerland */
#define CTRY_TAIWAN                      886     /* Taiwan */
#define CTRY_UNITED_KINGDOM              44      /* United Kingdom */
#define CTRY_UNITED_STATES               1       /* United States */


/*
 *  Locale Types.
 *
 *  These types are used for the GetLocaleInfoW NLS API routine.
 *
 *  LOCALE_NOUSEROVERRIDE is also used in GetTimeFormatW and GetDateFormatW.
 */
#define LOCALE_NOUSEROVERRIDE      0x80000000  /* do not use user overrides */

#define LOCALE_ILANGUAGE           0x00000001  /* language id */
#define LOCALE_SLANGUAGE           0x00000002  /* localized name of language */
#define LOCALE_SENGLANGUAGE        0x00001001  /* English name of language */
#define LOCALE_SABBREVLANGNAME     0x00000003  /* abbreviated language name */
#define LOCALE_SNATIVELANGNAME     0x00000004  /* native name of language */
#define LOCALE_ICOUNTRY            0x00000005  /* country code */
#define LOCALE_SCOUNTRY            0x00000006  /* localized name of country */  
#define LOCALE_SENGCOUNTRY         0x00001002  /* English name of country */  
#define LOCALE_SABBREVCTRYNAME     0x00000007  /* abbreviated country name */
#define LOCALE_SNATIVECTRYNAME     0x00000008  /* native name of country */  
#define LOCALE_IDEFAULTLANGUAGE    0x00000009  /* default language id */
#define LOCALE_IDEFAULTCOUNTRY     0x0000000A  /* default country code */
#define LOCALE_IDEFAULTCODEPAGE    0x0000000B  /* default code page */
                                           
#define LOCALE_SLIST               0x0000000C  /* list item separator */
#define LOCALE_IMEASURE            0x0000000D  /* 0 = metric, 1 = US */
                                           
#define LOCALE_SDECIMAL            0x0000000E  /* decimal separator */
#define LOCALE_STHOUSAND           0x0000000F  /* thousand separator */
#define LOCALE_SGROUPING           0x00000010  /* digit grouping */
#define LOCALE_IDIGITS             0x00000011  /* number of fractional digits */
#define LOCALE_ILZERO              0x00000012  /* leading zeros for decimal */
#define LOCALE_SNATIVEDIGITS       0x00000013  /* native ascii 0-9 */
                                           
#define LOCALE_SCURRENCY           0x00000014  /* local monetary symbol */
#define LOCALE_SINTLSYMBOL         0x00000015  /* intl monetary symbol */
#define LOCALE_SMONDECIMALSEP      0x00000016  /* monetary decimal separator */
#define LOCALE_SMONTHOUSANDSEP     0x00000017  /* monetary thousand separator */
#define LOCALE_SMONGROUPING        0x00000018  /* monetary grouping */
#define LOCALE_ICURRDIGITS         0x00000019  /* # local monetary digits */
#define LOCALE_IINTLCURRDIGITS     0x0000001A  /* # intl monetary digits */
#define LOCALE_ICURRENCY           0x0000001B  /* positive currency mode */
#define LOCALE_INEGCURR            0x0000001C  /* negative currency mode */
                                           
#define LOCALE_SDATE               0x0000001D  /* date separator */
#define LOCALE_STIME               0x0000001E  /* time separator */
#define LOCALE_SSHORTDATE          0x0000001F  /* short date format string */
#define LOCALE_SLONGDATE           0x00000020  /* long date format string */
#define LOCALE_STIMEFORMAT         0x00001003  /* time format string */
#define LOCALE_IDATE               0x00000021  /* short date format ordering */
#define LOCALE_ILDATE              0x00000022  /* long date format ordering */
#define LOCALE_ITIME               0x00000023  /* time format specifier */
#define LOCALE_ICENTURY            0x00000024  /* century format specifier */
#define LOCALE_ITLZERO             0x00000025  /* leading zeros in time field */
#define LOCALE_IDAYLZERO           0x00000026  /* leading zeros in day field */
#define LOCALE_IMONLZERO           0x00000027  /* leading zeros in month field */
#define LOCALE_S1159               0x00000028  /* AM designator */
#define LOCALE_S2359               0x00000029  /* PM designator */
                                           
#define LOCALE_SDAYNAME1           0x0000002A  /* long name for Monday */
#define LOCALE_SDAYNAME2           0x0000002B  /* long name for Tuesday */
#define LOCALE_SDAYNAME3           0x0000002C  /* long name for Wednesday */
#define LOCALE_SDAYNAME4           0x0000002D  /* long name for Thursday */
#define LOCALE_SDAYNAME5           0x0000002E  /* long name for Friday */
#define LOCALE_SDAYNAME6           0x0000002F  /* long name for Saturday */
#define LOCALE_SDAYNAME7           0x00000030  /* long name for Sunday */
#define LOCALE_SABBREVDAYNAME1     0x00000031  /* abbreviated name for Monday */   
#define LOCALE_SABBREVDAYNAME2     0x00000032  /* abbreviated name for Tuesday */  
#define LOCALE_SABBREVDAYNAME3     0x00000033  /* abbreviated name for Wednesday */
#define LOCALE_SABBREVDAYNAME4     0x00000034  /* abbreviated name for Thursday */ 
#define LOCALE_SABBREVDAYNAME5     0x00000035  /* abbreviated name for Friday */   
#define LOCALE_SABBREVDAYNAME6     0x00000036  /* abbreviated name for Saturday */ 
#define LOCALE_SABBREVDAYNAME7     0x00000037  /* abbreviated name for Sunday */   
#define LOCALE_SMONTHNAME1         0x00000038  /* long name for January */
#define LOCALE_SMONTHNAME2         0x00000039  /* long name for February */
#define LOCALE_SMONTHNAME3         0x0000003A  /* long name for March */
#define LOCALE_SMONTHNAME4         0x0000003B  /* long name for April */
#define LOCALE_SMONTHNAME5         0x0000003C  /* long name for May */
#define LOCALE_SMONTHNAME6         0x0000003D  /* long name for June */
#define LOCALE_SMONTHNAME7         0x0000003E  /* long name for July */
#define LOCALE_SMONTHNAME8         0x0000003F  /* long name for August */
#define LOCALE_SMONTHNAME9         0x00000040  /* long name for September */
#define LOCALE_SMONTHNAME10        0x00000041  /* long name for October */
#define LOCALE_SMONTHNAME11        0x00000042  /* long name for November */
#define LOCALE_SMONTHNAME12        0x00000043  /* long name for December */
#define LOCALE_SABBREVMONTHNAME1   0x00000044  /* abbreviated name for January */
#define LOCALE_SABBREVMONTHNAME2   0x00000045  /* abbreviated name for February */
#define LOCALE_SABBREVMONTHNAME3   0x00000046  /* abbreviated name for March */
#define LOCALE_SABBREVMONTHNAME4   0x00000047  /* abbreviated name for April */
#define LOCALE_SABBREVMONTHNAME5   0x00000048  /* abbreviated name for May */
#define LOCALE_SABBREVMONTHNAME6   0x00000049  /* abbreviated name for June */
#define LOCALE_SABBREVMONTHNAME7   0x0000004A  /* abbreviated name for July */
#define LOCALE_SABBREVMONTHNAME8   0x0000004B  /* abbreviated name for August */
#define LOCALE_SABBREVMONTHNAME9   0x0000004C  /* abbreviated name for September */
#define LOCALE_SABBREVMONTHNAME10  0x0000004D  /* abbreviated name for October */
#define LOCALE_SABBREVMONTHNAME11  0x0000004E  /* abbreviated name for November */
#define LOCALE_SABBREVMONTHNAME12  0x0000004F  /* abbreviated name for December */
                                           
#define LOCALE_SPOSITIVESIGN       0x00000050  /* positive sign */
#define LOCALE_SNEGATIVESIGN       0x00000051  /* negative sign */
#define LOCALE_IPOSSIGNPOSN        0x00000052  /* positive sign position */
#define LOCALE_INEGSIGNPOSN        0x00000053  /* negative sign position */
#define LOCALE_IPOSSYMPRECEDES     0x00000054  /* mon sym precedes pos amt */
#define LOCALE_IPOSSEPBYSPACE      0x00000055  /* mon sym sep by space from pos amt */ 
#define LOCALE_INEGSYMPRECEDES     0x00000056  /* mon sym precedes neg amt */
#define LOCALE_INEGSEPBYSPACE      0x00000057  /* mon sym sep by space from neg amt */


/*
 *  Time Flags for GetTimeFormatW.
 */
#define TIME_NOMINUTESORSECONDS    0x00000001  /* do not use minutes or seconds */
#define TIME_NOSECONDS             0x00000002  /* do not use seconds */
#define TIME_NOTIMEMARKER          0x00000004  /* do not use time marker */
#define TIME_FORCE24HOURFORMAT     0x00000008  /* always use 24 hour format */


/*
 *  Date Flags for GetDateFormatW.
 */
#define DATE_SHORTDATE             0x00000001  /* use short date picture */
#define DATE_LONGDATE              0x00000002  /* use long date picture */



/***************************************************************************\
* Typedefs
*
* Define all types for the NLS component here.
\***************************************************************************/

/*
 *  Locale type constant
 */

typedef DWORD LCTYPE;


/*
 *  CP Info.
 */
typedef struct _cpinfo {
    UINT   MaxCharSize;                     /* max length (bytes) of a char */
    BYTE   DefaultChar[MAX_DEFAULTCHAR];    /* default character */
    BYTE   LeadByte[MAX_LEADBYTES];         /* lead byte ranges */
} CPINFO, *LPCPINFO;



/***************************************************************************\
* Macros
*
* Define all macros for the NLS component here.
\***************************************************************************/



/***************************************************************************\
* Function Prototypes
*
* Only prototypes for the NLS APIs should go here.
\***************************************************************************/

/*
 *  Code Page Dependent APIs.
 */
BOOL
WINAPI
IsValidCodePage(
    UINT  CodePage);

UINT
WINAPI
GetACP(void);

UINT
WINAPI
GetOEMCP(void);

BOOL
WINAPI
GetCPInfo(
    UINT      CodePage,
    LPCPINFO  lpCPInfo);

BOOL
WINAPI
IsDBCSLeadByte(
    BYTE  TestChar);

int
WINAPI
MultiByteToWideChar(
    UINT    CodePage,
    DWORD   dwFlags,
    LPCSTR  lpMultiByteStr,
    int     cchMultiByte,
    LPWSTR  lpWideCharStr,
    int     cchWideChar);

int
WINAPI
WideCharToMultiByte(
    UINT     CodePage,
    DWORD    dwFlags,
    LPCWSTR  lpWideCharStr,
    int      cchWideChar,
    LPSTR    lpMultiByteStr,
    int      cchMultiByte,
    LPCSTR   lpDefaultChar,
    LPBOOL   lpUsedDefaultChar);


/*
 *  Locale Dependent APIs.
 */
int
WINAPI
CompareStringW(
    LCID     Locale,
    DWORD    dwCmpFlags,
    LPCWSTR  lpString1,
    int      cchCount1,
    LPCWSTR  lpString2,
    int      cchCount2);

int
WINAPI
LCMapStringW(
    LCID     Locale,
    DWORD    dwMapFlags,
    LPCWSTR  lpSrcStr,
    int      cchSrc,
    LPWSTR   lpDestStr,
    int      cchDest);

int
WINAPI
GetLocaleInfoW(
    LCID    Locale,
    LCTYPE  LCType,
    LPWSTR  lpLCData,
    int     cchData);

int
WINAPI
GetTimeFormatW(
    LCID Locale,
    DWORD dwFlags,
    CONST SYSTEMTIME *lpTime,
    LPCWSTR lpFormat,
    LPWSTR lpTimeStr,
    int cchTime);

int
WINAPI
GetDateFormatW(
    LCID Locale,
    DWORD dwFlags,
    CONST SYSTEMTIME *lpDate,
    LPCWSTR lpFormat,
    LPWSTR lpDateStr,
    int cchDate);

LCID
WINAPI
GetThreadLocale(void);

BOOL
WINAPI
SetThreadLocale(
    LCID Locale
    );

LANGID
WINAPI
GetSystemDefaultLangID(void);

LANGID
WINAPI
GetUserDefaultLangID(void);

LCID
WINAPI
GetSystemDefaultLCID(void);

LCID
WINAPI
GetUserDefaultLCID(void);


/*
 *  Locale Independent APIs.
 */
BOOL
WINAPI
GetStringTypeW(
    DWORD    dwInfoType,
    LPCWSTR  lpSrcStr,
    int      cchSrc,
    LPWORD   lpCharType);

int
WINAPI
FoldStringW(
    DWORD    dwMapFlags,
    LPCWSTR  lpSrcStr,
    int      cchSrc,
    LPWSTR   lpDestStr,
    int      cchDest);



#endif   // NONLS

#ifdef __cplusplus
}
#endif

#endif   // _WINNLS_

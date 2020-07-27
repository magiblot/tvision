/*------------------------------------------------------------------------*/
/*                                                                        */
/*  filename - drivers2.cpp                                               */
/*                                                                        */
/*  function(s)                                                           */
/*      ctrlToArrow -- map control keys to arrow keys                     */
/*      cstrlen     -- calculate length of a control string               */
/*                                                                        */
/*------------------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TKeys
#include <tvision/tv.h>

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  ctrlToArrow                                                           */
/*                                                                        */
/*  argument:                                                             */
/*                                                                        */
/*      keyCode - scan code to be mapped to keypad arrow code             */
/*                                                                        */
/*  returns:                                                              */
/*                                                                        */
/*      scan code for arrow key corresponding to Wordstar key,            */
/*      or original key code if no correspondence exists                  */
/*                                                                        */
/*------------------------------------------------------------------------*/
ushort ctrlToArrow(ushort keyCode)
{

const uchar ctrlCodes[] =
    {
    kbCtrlS, kbCtrlD, kbCtrlE, kbCtrlX, kbCtrlA,
    kbCtrlF, kbCtrlG, kbCtrlV, kbCtrlR, kbCtrlC, kbCtrlH
    };

const ushort arrowCodes[] =
    {
    kbLeft, kbRight, kbUp, kbDown, kbHome,
    kbEnd,  kbDel,   kbIns,kbPgUp, kbPgDn, kbBack
    };

    for( int i = 0; i < sizeof(ctrlCodes); i++ )
        if( (keyCode & 0x00ff) == ctrlCodes[i] )
            return arrowCodes[i];
    return keyCode;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  cstrlen                                                               */
/*                                                                        */
/*  argument:                                                             */
/*                                                                        */
/*      s       - pointer to 0-terminated string                          */
/*                                                                        */
/*  returns                                                               */
/*                                                                        */
/*      length of string, ignoring '~' characters.                        */
/*                                                                        */
/*  Comments:                                                             */
/*                                                                        */
/*      Used in determining the displayed length of command strings,      */
/*      which use '~' to toggle between display attributes                */
/*                                                                        */
/*------------------------------------------------------------------------*/

int cstrlen( const char *s )
{
#ifdef __BORLANDC__
    int len = 0;
    while( *s != EOS )
        {
        if( *s++ != '~' )
            len++;
        }
    return len;
#else
    return cstrlen(std::string_view {s});
#endif
}

#ifndef __BORLANDC__
int cstrlen( std::string_view s )
{
    std::mbstate_t state = {};
    size_t i = 0, width = 0;
    while (i < s.size()) {
        if (s[i] != '~')
            utf8next(s.substr(i, s.size() - i), i, width, state);
        else
            ++i;
    }
    return width;
}
#endif

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  strwidth                                                              */
/*                                                                        */
/*  argument:                                                             */
/*                                                                        */
/*      s       - pointer to 0-terminated string                          */
/*                                                                        */
/*  returns                                                               */
/*                                                                        */
/*      displayed length of string.                                       */
/*                                                                        */
/*------------------------------------------------------------------------*/

int strwidth( const char *s )
{
#ifdef __BORLANDC__
    return strlen(s);
#else
    return strwidth(std::string_view {s});
#endif
}

#ifndef __BORLANDC__
int strwidth( std::string_view s )
{
    std::mbstate_t state = {};
    size_t i = 0, width = 0;
    while (i < s.size())
        utf8next(s.substr(i, s.size() - i), i, width, state);
    return width;
}
#endif

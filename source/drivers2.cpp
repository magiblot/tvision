/*------------------------------------------------------------------------*/
/*                                                                        */
/*  filename - drivers2.cpp                                               */
/*                                                                        */
/*  function(s)                                                           */
/*      ctrlToArrow -- map control keys to arrow keys                     */
/*      cstrlen     -- calculate length of a control string               */
/*                                                                        */
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/*                                                                        */
/*    Turbo Vision -  Version 1.0                                         */
/*                                                                        */
/*                                                                        */
/*    Copyright (c) 1991 by Borland International                         */
/*    All Rights Reserved.                                                */
/*                                                                        */
/*------------------------------------------------------------------------*/

#define Uses_TKeys
#include <tv.h>

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
    int len = 0;
    while( *s != EOS )
        {
        if( *s++ != '~' )
            len++;
        }
    return len;
}


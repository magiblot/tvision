/*------------------------------------------------------------*/
/* filename -       tparamte.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TParamText member functions               */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TParamText
#include <tvision\tv.h>

#if !defined( __STDIO_H )
#include <Stdio.h>
#endif  // __STDIO_H

#if !defined( __STDARG_H )
#include <Stdarg.h>
#endif  // __STDARG_H

#if !defined( __STRING_H )
#include <String.h>
#endif  // __STRING_H

TParamText::TParamText( const TRect& bounds ) :
    TStaticText(bounds, 0 ),
    str( new char [256] )
{
    str[0] = EOS;
}

TParamText::~TParamText()
{
    delete str;
}

void TParamText::getText( char *s )
{
    if( str != 0 )
        strcpy( s, str );
    else
        *s = EOS;
}

int TParamText::getTextLen()
{
    return (str != 0) ? strlen( str ) : 0;
}

void TParamText::setText( char *fmt, ... )
{
    va_list ap;

    va_start( ap, fmt );
    vsprintf( str, fmt, ap );
    va_end( ap );

    drawView();
}

#if !defined(NO_STREAMABLE)

TStreamable *TParamText::build()
{
    return new TParamText( streamableInit );
}

TParamText::TParamText( StreamableInit ) : TStaticText( streamableInit )
{
}

#endif

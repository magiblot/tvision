/*------------------------------------------------------------*/
/* filename -       tparamte.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TParamText member functions               */
/*------------------------------------------------------------*/

/*------------------------------------------------------------*/
/*                                                            */
/*    Turbo Vision -  Version 1.0                             */
/*                                                            */
/*                                                            */
/*    Copyright (c) 1991 by Borland International             */
/*    All Rights Reserved.                                    */
/*                                                            */
/*------------------------------------------------------------*/

#define Uses_TParamText
#include <tv.h>

#if !defined( __STDIO_H )
#include <Stdio.h>
#endif  // __STDIO_H

TParamText::TParamText( const TRect& bounds,
                        const char *aText,
                        int aParamCount ) :
    TStaticText(bounds, aText),
    paramList( 0 ),
    paramCount( aParamCount )
{
}

ushort TParamText::dataSize()
{
    return paramCount * sizeof(long);
}

void TParamText::getText( char *s )
{
    if( text == 0 )
        *s = EOS;
    else
        vsprintf( s, text, paramList );
}

void TParamText::setData( void *rec )
{
    paramList = &rec;
}

void TParamText::write( opstream& os )
{
    TStaticText::write( os );
    os << paramCount;
}

void *TParamText::read( ipstream& is )
{
    TStaticText::read( is );
    is >> paramCount;
    paramList = 0;
    return this;
}

TStreamable *TParamText::build()
{
    return new TParamText( streamableInit );
}

TParamText::TParamText( StreamableInit ) : TStaticText( streamableInit )
{
}



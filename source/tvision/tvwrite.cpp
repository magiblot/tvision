/*------------------------------------------------------------*/
/* filename -       tvwrite.cpp                               */
/*                                                            */
/* function(s)                                                */
/*                  TView write member functions              */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TView
#include <tvision/tv.h>

#ifndef __BORLANDC__

#include <assert.h>

void TView::writeView( write_args )
{
    BREAK;
}

void TView::writeBuf( short x, short y, short w, short h, const void _FAR* b )
{
    BREAK;
}

void TView::writeChar( short x, short y, char c, uchar color, short count )
{
    BREAK;
}

void TView::writeLine( short x, short y, short w, short h, const void _FAR *b )
{
    BREAK;
}

void TView::writeStr( short x, short y, const char *str, uchar color )
{
    BREAK;
}

#endif

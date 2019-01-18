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
    assert(false);
}

void TView::writeBuf( short x, short y, short w, short h, const void _FAR* b )
{
    assert(false);
}

void TView::writeChar( short x, short y, char c, uchar color, short count )
{
    assert(false);
}

void TView::writeLine( short x, short y, short w, short h, const void _FAR *b )
{
    assert(false);
}

void TView::writeStr( short x, short y, const char *str, uchar color )
{
    assert(false);
}

#endif

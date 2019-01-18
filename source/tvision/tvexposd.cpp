/*------------------------------------------------------------*/
/* filename -       tvexposd.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TView exposed member function             */
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

Boolean TView::exposed()
{
    assert(false);
}

#endif

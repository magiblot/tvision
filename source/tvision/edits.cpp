/*------------------------------------------------------------*/
/* filename -       edits.cpp                                 */
/*                                                            */
/* function(s)                                                */
/*                  TEditor member functions                  */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TEditor
#include <tvision/tv.h>

#ifndef __BORLANDC__

#include <assert.h>

char TEditor::bufChar( ushort P )
{
    assert(false);
}

ushort TEditor::bufPtr( ushort P )
{
    assert(false);
}

void TEditor::formatLine( void *DrawBuf,
                          ushort LinePtr,
                          int Width,
                          ushort Colors
                        )
{
    assert(false);
}

ushort TEditor::lineEnd( ushort P )
{
    assert(false);
}

ushort TEditor::lineStart( ushort P )
{
    assert(false);
}

ushort TEditor::nextChar( ushort P )
{
    assert(false);
}

ushort TEditor::prevChar( ushort P )
{
    assert(false);
}

extern "C" {

int countLines( void *buf, uint count )
{
    assert(false);
}

ushort scan( const void *block, ushort size, const char *str )
{
    assert(false);
}

ushort iScan( const void *block, ushort size, const char *str )
{
    assert(false);
}

}

#endif

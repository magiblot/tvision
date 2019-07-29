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
    BREAK;
}

ushort TEditor::bufPtr( ushort P )
{
    BREAK;
}

void TEditor::formatLine( ushort *DrawBuf,
                          ushort LinePtr,
                          int Width,
                          ushort Colors
                        )
{
    BREAK;
}

ushort TEditor::lineEnd( ushort P )
{
    BREAK;
}

ushort TEditor::lineStart( ushort P )
{
    BREAK;
}

ushort TEditor::nextChar( ushort P )
{
    BREAK;
}

ushort TEditor::prevChar( ushort P )
{
    BREAK;
}

extern "C" {

int countLines( void *buf, uint count )
{
    BREAK;
}

ushort scan( const void *block, ushort size, const char *str )
{
    BREAK;
}

ushort iScan( const void *block, ushort size, const char *str )
{
    BREAK;
}

}

#endif

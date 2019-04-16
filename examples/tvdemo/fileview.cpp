/*---------------------------------------------------------*/
/*                                                         */
/*   Turbo Vision FileViewer Demo Support File             */
/*                                                         */
/*---------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_MsgBox
#define Uses_TKeys
#define Uses_TScroller
#define Uses_TDrawBuffer
#define Uses_TRect
#define Uses_TProgram
#define Uses_TDeskTop
#define Uses_TStreamableClass
#include <tvision\tv.h>
__link(RScroller)
__link(RScrollBar)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <fstream.h>

#include "tvcmds.h"
#include "fileview.h"


const char * const TFileViewer::name = "TFileViewer";

TFileViewer::TFileViewer( const TRect& bounds,
                          TScrollBar *aHScrollBar,
                          TScrollBar *aVScrollBar,
                          const char *aFileName) :
    TScroller( bounds, aHScrollBar, aVScrollBar )
{
    growMode = gfGrowHiX | gfGrowHiY;
    isValid = True;
    fileName = 0;
    readFile( aFileName );
}

TFileViewer::~TFileViewer()
{
     delete fileName;
     destroy (fileLines);
}

void TFileViewer::draw()
{
    char *p;

    ushort c =  getColor(0x0301);
    for( short i = 0; i < size.y; i++ )
        {
        TDrawBuffer b;
        b.moveChar( 0, ' ', c, (short)size.x );

        if( delta.y + i < fileLines->getCount() )
            {
            char s[maxLineLength+1];
            p = (char *)( fileLines->at(delta.y+i) );
            if( p == 0 || strlen(p) < delta.x )
                s[0] = EOS;
            else
            {
                strncpy( s, p+delta.x, size.x );
                if( strlen( p + delta.x ) > size.x )
                    s[size.x] = EOS;
                }
            b.moveStr( 0, s, c );
            }
        writeBuf( 0, i, (short)size.x, 1, b );
        }
}

void TFileViewer::scrollDraw()
{
    TScroller::scrollDraw();
    draw();
}

void TFileViewer::readFile( const char *fName )
{
    delete fileName;

    limit.x = 0;
    fileName = newStr( fName );
    fileLines = new TLineCollection(5, 5);
    ifstream fileToView( fName );
    if( !fileToView )
        {
        messageBox( "Invalid drive or directory", mfError | mfOKButton );
        isValid = False;
        }
    else
        {
        char line[maxLineLength+1];
        while( !lowMemory() &&
               !fileToView.eof() && 
               fileToView.get( line, sizeof line ) != 0 
             )
            {
            char c;
            fileToView.get(c);      // grab trailing newline
            limit.x = max( limit.x, strlen( line ) );
            fileLines->insert( newStr( line ) );
            }
        isValid = True;
        }
    limit.y = fileLines->getCount();
}

void TFileViewer::setState( ushort aState, Boolean enable )
{
    TScroller::setState( aState, enable );
    if( enable && (aState & sfExposed) )
        setLimit( limit.x, limit.y );
}

Boolean TFileViewer::valid( ushort )
{
    return isValid;
}

void *TFileViewer::read(ipstream& is)
{
    char *fName;

    TScroller::read(is);
    fName = is.readString();
    fileName = 0;
    readFile(fName);
    delete fName; 
    return this;
}

void TFileViewer::write(opstream& os)
{
    TScroller::write(os);
    os.writeString(fileName);
}

TStreamable *TFileViewer::build()
{
    return new TFileViewer( streamableInit );
}


TStreamableClass RFileView( TFileViewer::name,
                            TFileViewer::build,
                              __DELTA(TFileViewer)
                          );



static short winNumber = 0;

TFileWindow::TFileWindow( const char *fileName ) :
    TWindow( TProgram::deskTop->getExtent(), fileName, winNumber++ ),
    TWindowInit( &TFileWindow::initFrame )
{
    options |= ofTileable;
    TRect r( getExtent() );
    r.grow(-1, -1);
    insert(new TFileViewer( r,
                            standardScrollBar(sbHorizontal | sbHandleKeyboard),
                            standardScrollBar(sbVertical | sbHandleKeyboard),
                            fileName) );
}



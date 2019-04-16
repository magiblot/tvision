/*----------------------------------------------------------*/
/*                                                          */
/*   Copyright (c) 1991 by Borland International            */
/*                                                          */
/*----------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TApplication
#define Uses_TEditWindow
#define Uses_TDeskTop
#define Uses_TRect
#define Uses_TEditor
#define Uses_TFileEditor
#define Uses_TFileDialog
#define Uses_TChDirDialog

#include <tvision\tv.h>

#include "tvedit.h"

#include <stdlib.h>
#include <stdarg.h>
#include <strstrea.h>
#include <iomanip.h>

TEditWindow *clipWindow;

TEditWindow *TEditorApp::openEditor( const char *fileName, Boolean visible )
{
    TRect r = deskTop->getExtent();
    TView *p = validView( new TEditWindow( r, fileName, wnNoNumber ) );
    if( !visible )
        p->hide();
    deskTop->insert( p );
    return (TEditWindow *)p;
}

TEditorApp::TEditorApp() :
    TProgInit( TEditorApp::initStatusLine,
               TEditorApp::initMenuBar,
               TEditorApp::initDeskTop
             ),
    TApplication()
{

    TCommandSet ts;
    ts.enableCmd( cmSave );
    ts.enableCmd( cmSaveAs );
    ts.enableCmd( cmCut );
    ts.enableCmd( cmCopy );
    ts.enableCmd( cmPaste );
    ts.enableCmd( cmClear );
    ts.enableCmd( cmUndo );
    ts.enableCmd( cmFind );
    ts.enableCmd( cmReplace );
    ts.enableCmd( cmSearchAgain );
    disableCommands( ts );

    TEditor::editorDialog = doEditDialog;
    clipWindow = openEditor( 0, False );
    if( clipWindow != 0 )
        {
        TEditor::clipboard = clipWindow->editor;
        TEditor::clipboard->canUndo = False;
        }
}

void TEditorApp::fileOpen()
{
    char fileName[MAXPATH];
    strcpy( fileName, "*.*" );

    if( execDialog( new TFileDialog( "*.*", "Open file",
            "~N~ame", fdOpenButton, 100 ), fileName) != cmCancel )
        openEditor( fileName, True );
}

void TEditorApp::fileNew()
{
    openEditor( 0, True );
}

void TEditorApp::changeDir()
{
    execDialog( new TChDirDialog( cdNormal, 0 ), 0 );
}

void TEditorApp::dosShell()
{
    suspend();
    system("cls");
    cout << "Type EXIT to return...";
    system( getenv( "COMSPEC"));
    resume();
    redraw();
}

void TEditorApp::showClip()
{
    clipWindow->select();
    clipWindow->show();
}

void TEditorApp::tile()
{
    deskTop->tile( deskTop->getExtent() );
}

void TEditorApp::cascade()
{
    deskTop->cascade( deskTop->getExtent() );
}

void TEditorApp::handleEvent( TEvent& event )
{
    TApplication::handleEvent( event );
    if( event.what != evCommand )
        return;
    else
        switch( event.message.command )
            {
            case cmOpen:
                fileOpen();
                break;

            case cmNew:
                fileNew();
                break;

            case cmChangeDrct:
                changeDir();
                break;

            case cmDosShell:
                dosShell();
                break;

            case cmShowClip:
                showClip();
                break;

            case cmTile:
                tile();
                break;

            case cmCascade:
                cascade();
                break;

            default:
                return ;
            }
    clearEvent( event );
}
int main()
{
    TEditorApp editorApp;
    editorApp.run();
    return 0;
}


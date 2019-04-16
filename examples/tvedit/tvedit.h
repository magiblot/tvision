/*----------------------------------------------------------*/
/*                                                          */
/*   Copyright (c) 1991 by Borland International            */
/*                                                          */
/*   Turbo Vision TVEDIT header file                        */
/*                                                          */
/*----------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

class TMenuBar;
class TStatusLine;
class TEditWindow;
class TDialog;

const
//  cmOpen       = 100,
//  cmNew        = 101,
  cmChangeDrct = 102,
//  cmDosShell   = 103,
  cmCalculator = 104,
  cmShowClip   = 105;

class TEditorApp : public TApplication
{

public:

    TEditorApp();

    virtual void handleEvent( TEvent& event );
    static TMenuBar *initMenuBar( TRect );
    static TStatusLine *initStatusLine( TRect );
    virtual void outOfMemory();

private:

    TEditWindow *openEditor( const char *fileName, Boolean visible );
    void fileOpen();
    void fileNew();
    void changeDir();
    void dosShell();
    void showClip();
    void tile();
    void cascade();
};

extern TEditWindow *clipWindow;

ushort execDialog( TDialog *d, void *data );
TDialog *createFindDialog();
TDialog *createReplaceDialog();
ushort doEditDialog( int dialog, ... );


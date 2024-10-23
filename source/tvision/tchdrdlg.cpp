/*------------------------------------------------------------*/
/* filename -       tchdrdlg.cpp                              */
/*                                                            */
/* function(s)                                                */
/*          TChDirDialog member functions                     */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_MsgBox
#define Uses_TChDirDialog
#define Uses_TRect
#define Uses_TInputLine
#define Uses_TLabel
#define Uses_THistory
#define Uses_TScrollBar
#define Uses_TDirListBox
#define Uses_TButton
#define Uses_TEvent
#define Uses_TDirEntry
#define Uses_TDirCollection
#define Uses_TChDirDialog
#define Uses_opstream
#define Uses_ipstream
#include <tvision/tv.h>

#if !defined( __CTYPE_H )
#include <ctype.h>
#endif	// __CTYPE_H

#if !defined( __STRING_H )
#include <string.h>
#endif  // __STRING_H

TChDirDialog::TChDirDialog( ushort opts, ushort histId ) noexcept :
    TWindowInit( &TChDirDialog::initFrame ),
    TDialog( TRect( 16, 2, 64, 20 ), changeDirTitle )
{
    options |= ofCentered;
    flags |= wfGrow;

    dirInput = new TInputLine( TRect( 3, 3, 42, 4 ), MAXPATH-1 );
    dirInput->growMode = gfGrowHiX;
    insert( dirInput );
    insert( new TLabel( TRect( 2, 2, 17, 3 ), dirNameText, dirInput ));
    THistory *history = new THistory( TRect( 42, 3, 45, 4 ), dirInput, histId );
    history->growMode = gfGrowLoX | gfGrowHiX;
    insert( history );

    TScrollBar *sb = new TScrollBar( TRect( 32, 6, 33, 16 ) );
    insert( sb );
    dirList = new TDirListBox( TRect( 3, 6, 32, 16 ), sb );
    dirList->growMode = gfGrowHiX | gfGrowHiY;
    insert( dirList );
    insert( new TLabel( TRect( 2, 5, 17, 6 ), dirTreeText, dirList ) );

    okButton = new TButton( TRect( 35, 6, 45, 8 ), okText, cmOK, bfDefault );
    okButton->growMode = gfGrowLoX | gfGrowHiX;
    insert( okButton );

    chDirButton = new TButton( TRect( 35, 9, 45, 11 ), chdirText, cmChangeDir, bfNormal );
    chDirButton->growMode = gfGrowLoX | gfGrowHiX;
    insert( chDirButton );

    TButton *revertButton = new TButton( TRect( 35, 12, 45, 14 ), revertText, cmRevert, bfNormal );
    revertButton->growMode = gfGrowLoX | gfGrowHiX;
    insert( revertButton );

    if( (opts & cdHelpButton) != 0 )
        {
        TButton *helpButton = new TButton( TRect( 35, 15, 45, 17 ), helpText, cmHelp, bfNormal );
        helpButton->growMode = gfGrowLoX | gfGrowHiX;
        insert( helpButton );
        }

    if( (opts & cdNoLoadDir) == 0 )
        setUpDialog();

    selectNext( False );
}

ushort TChDirDialog::dataSize()
{
    return 0;
}

void TChDirDialog::shutDown()
{
    dirList = 0;
    dirInput = 0;
    okButton = 0;
    chDirButton = 0;
    TDialog::shutDown();
}

void TChDirDialog::sizeLimits( TPoint& min, TPoint& max )
{
    TDialog::sizeLimits( min, max );
    min.x = 48;
    min.y = 18;
}

void TChDirDialog::getData( void * )
{
}

#define isSeparator(c) (c == '\\' || c == '/')

static void trimEndSeparator(char *path)
{
    int len = strlen( path );
    if( len > 3 && isSeparator(path[len-1]) )
        path[len-1] = EOS;
}

static void getCurrentDir( char *dir )
{
    getCurDir( dir );
#if defined( _TV_UNIX )
    // Remove drive letter.
    memmove(dir, dir + 2, strlen(dir) - 2 + 1);
#endif
}

void TChDirDialog::handleEvent( TEvent& event )
{
    TDialog::handleEvent( event );
    switch( event.what )
        {
        case evCommand:
            {
            char curDir[MAXPATH];
            switch( event.message.command )
                {
                case cmRevert:
                    getCurrentDir( curDir );
                    break;
                case cmChangeDir:
                    {
                    TDirEntry *p = dirList->list()->at( dirList->focused );
                    strcpy( curDir, p->dir() );
                    if( strcmp( curDir, drivesText ) == 0 )
                        break;
                    else if( isSeparator( curDir[0] ) || driveValid( curDir[0] ) )
                        {
                        int len = strlen( curDir );
                        if( !isSeparator(curDir[len-1]) )
                            strcat( curDir, "\\" );
                        }
                    else
                        return;
                    break;
                    }
                default:
                    return;
                }
            dirList->newDirectory( curDir );
            trimEndSeparator( curDir );
            strnzcpy( dirInput->data, curDir, dirInput->maxLen + 1 );
            dirInput->drawView();
            dirList->select();
            clearEvent( event );
            }
        default:
            break;
        }
}

void TChDirDialog::setData( void * )
{
}

void TChDirDialog::setUpDialog()
{
    if( dirList != 0 )
        {
        char curDir[MAXPATH];
        getCurrentDir( curDir );
        dirList->newDirectory( curDir );
        if( dirInput != 0 )
            {
            trimEndSeparator( curDir );
            strnzcpy( dirInput->data, curDir, dirInput->maxLen + 1 );
            dirInput->drawView();
            }
        }
}

static int changeDir( const char *path )
{
    if( path[1] == ':' )
        setdisk( (char) toupper((uchar) path[0]) - 'A' );
    return chdir( path );
}

Boolean TChDirDialog::valid( ushort command )
{
    if( command != cmOK )
        return True;

    char path[MAXPATH];
    strnzcpy( path, dirInput->data, MAXPATH );
    fexpand( path );

    trimEndSeparator( path );

    if( changeDir( path ) != 0 )
        {
        messageBox( mfError | mfOKButton, "%s: '%s'.", invalidText, path );
        return False;
        }
    return True;
}

#if !defined(NO_STREAMABLE)

void TChDirDialog::write( opstream& os )
{
    TDialog::write( os );
    os << dirList << dirInput << okButton << chDirButton;
}

void *TChDirDialog::read( ipstream& is )
{
    TDialog::read( is );
    is >> dirList >> dirInput >> okButton >> chDirButton;
    setUpDialog();
    return this;
}

TStreamable *TChDirDialog::build()
{
    return new TChDirDialog( streamableInit );
}

#endif

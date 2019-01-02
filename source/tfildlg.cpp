/*------------------------------------------------------------*/
/* filename -       tfildlg.cpp                               */
/*                                                            */
/* function(s)                                                */
/*                  TFileDialog member functions              */
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

#define Uses_TFileDialog
#define Uses_MsgBox
#define Uses_TRect
#define Uses_TFileInputLine
#define Uses_TButton
#define Uses_TLabel
#define Uses_TFileList
#define Uses_THistory
#define Uses_TScrollBar
#define Uses_TEvent
#define Uses_TFileInfoPane
#define Uses_opstream
#define Uses_ipstream
#include <tv.h>

#if !defined( __DIR_H )
#include <Dir.h>
#endif  // __DIR_H

#if !defined( __ERRNO_H )
#include <Errno.h>
#endif  // __ERRNO_H

#if !defined( __IO_H )
#include <IO.h>
#endif  // __IO_H

#if !defined( __STDIO_H )
#include <Stdio.h>
#endif  // __STDIO_H

#if !defined( __CTYPE_H )
#include <ctype.h>
#endif  // __CTYPE_H

#if !defined( __STRING_H )
#include <String.h>
#endif  // __STRING_H

// File dialog flags 
const
    ffOpen        = 0x0001,
    ffSaveAs      = 0x0002;

const
    cmOpenDialogOpen    = 100,
    cmOpenDialogReplace = 101;

TFileDialog::TFileDialog( const char *aWildCard,
                          const char *aTitle,
                          const char *inputName,
                          ushort aOptions,
                          uchar histId
                        ) :
    TDialog( TRect( 15, 1, 64, 20 ), aTitle ),
    directory( 0 ),
    TWindowInit( &TFileDialog::initFrame )
{
    options |= ofCentered;
    strcpy( wildCard, aWildCard );

    fileName = new TFileInputLine( TRect( 3, 3, 31, 4 ), 79 );
    strcpy( fileName->data, wildCard );
    insert( fileName );

    insert( new TLabel( TRect( 2, 2, 3+cstrlen(inputName), 3 ),
                        inputName,
                        fileName
                      ) );
    insert( new THistory( TRect( 31, 3, 34, 4 ), fileName, histId ) );
    TScrollBar *sb = new TScrollBar( TRect( 3, 14, 34, 15 ) );
    insert( sb );
    insert( fileList = new TFileList( TRect( 3, 6, 34, 14 ), sb ) );
    insert( new TLabel( TRect( 2, 5, 8, 6 ), filesText, fileList ) );

    ushort opt = bfDefault;
    TRect r( 35, 3, 46, 5 );

    if( (aOptions & fdOpenButton) != 0 )
        {
        insert( new TButton( r, openText, cmFileOpen, opt ) );
        opt = bfNormal;
        r.a.y += 3;
        r.b.y += 3;
        }

    if( (aOptions & fdOKButton) != 0 )
        {
        insert( new TButton( r, okText, cmFileOpen, opt ) );
        opt = bfNormal;
        r.a.y += 3;
        r.b.y += 3;
        }

    if( (aOptions & fdReplaceButton) != 0 )
        {
        insert( new TButton( r, replaceText, cmFileReplace, opt ) );
        opt = bfNormal;
        r.a.y += 3;
        r.b.y += 3;
        }

    if( (aOptions & fdClearButton) != 0 )
        {
        insert( new TButton( r, clearText, cmFileClear, opt ) );
        opt = bfNormal;
        r.a.y += 3;
        r.b.y += 3;
        }

    insert( new TButton( r, cancelText, cmCancel, bfNormal ) );
    r.a.y += 3;
    r.b.y += 3;

    if( (aOptions & fdHelpButton) != 0 )
        {
        insert( new TButton( r, helpText, cmHelp, bfNormal ) );
        opt = bfNormal;
        r.a.y += 3;
        r.b.y += 3;
        }

    insert( new TFileInfoPane( TRect( 1, 16, 48, 18 ) ) );

    selectNext( False );
    if( (aOptions & fdNoLoadDir) == 0 )
        readDirectory();
}

TFileDialog::~TFileDialog()
{
    delete (char *)directory;
}

void TFileDialog::shutDown()
{
    fileName = 0;
    fileList = 0;
    TDialog::shutDown();
}

static Boolean relativePath( const char *path )
{
    if( path[0] != EOS && (path[0] == '\\' || path[1] == ':') )
        return False;
    else
        return True;
}

static void noWildChars( char *dest, const char *src )
{
    while( *src != EOS )
        {
        if( *src != '?' && *src != '*' )
            *dest++ = *src;
        src++;
        }
    *dest = EOS;
}

static void trim( char *dest, const char *src )
{
    while( *src != EOS && isspace( *src ) )
        src++;
    while( *src != EOS && !isspace( *src ) )
        *dest++ = *src++;
    *dest = EOS;
}

void TFileDialog::getFileName( char *s )
{
char buf[2*MAXPATH];
char drive[MAXDRIVE];
char path[MAXDIR];
char name[MAXFILE];
char ext[MAXEXT];
char TName[MAXFILE];
char TExt[MAXEXT];

    trim( buf, fileName->data );
    if( relativePath( buf ) == True )
        {
        strcpy( buf, directory );
        trim( buf + strlen(buf), fileName->data );
        }
    fexpand( buf );
    fnsplit( buf, drive, path, name, ext );
    if( (name[0] == EOS || ext[0] == EOS) && !isDir( buf ) )
        {
        fnsplit( wildCard, 0, 0, TName, TExt );
        if( name[0] == EOS && ext[0] == EOS )
            fnmerge( buf, drive, path, TName, TExt );
        else if( name[0] == EOS )
            fnmerge( buf, drive, path, TName, ext );
        else if( ext[0] == EOS )
            {
            if( isWild( name ) )
                fnmerge( buf, drive, path, name, TExt );
            else
                {
                fnmerge( buf, drive, path, name, 0 );
                noWildChars( buf + strlen(buf), TExt );
                }
            }
        }
    strcpy( s, buf );
}

void TFileDialog::handleEvent(TEvent& event)
{
    TDialog::handleEvent(event);
    if( event.what == evCommand )
        switch( event.message.command )
            {
            case cmFileOpen:
            case cmFileReplace:
            case cmFileClear:
                {
                endModal(event.message.command);
                clearEvent(event);
                }
                break;
            default:
                break;
            }
}

void TFileDialog::readDirectory()
{
    fileList->readDirectory( wildCard );
    char curDir[MAXPATH];
    getCurDir( curDir );
    directory = newStr( curDir );
}

void TFileDialog::setData( void *rec )
{
    TDialog::setData( rec );
    if( *(char *)rec != EOS && isWild( (char *)rec ) )
        {
        valid( cmFileInit );
        fileName->select();
        }
}

void TFileDialog::getData( void *rec )
{
    getFileName( (char *)rec );
}

Boolean TFileDialog::checkDirectory( const char *str )
{
    if( pathValid( str ) )
        return True;
    else
        {
        messageBox( invalidDriveText, mfError | mfOKButton );
        fileName->select();
        return False;
        }
}

Boolean TFileDialog::valid(ushort command)
{
char fName[MAXPATH];
char drive[MAXDRIVE];
char dir[MAXDIR];
char name[MAXFILE];
char ext[MAXEXT];

    if( command == 0 )
        return True;

    if( TDialog::valid( command ) )
        {
        getFileName( fName );
        if( command != cmCancel && command != cmFileClear )
            {
            if( isWild( fName ) )
                {
                fnsplit( fName, drive, dir, name, ext );
                char path[MAXPATH];
                strcpy( path, drive );
                strcat( path, dir );
                if( checkDirectory( path ) )
                    {
                    delete (char *)directory;
                    directory = newStr( path );
                    strcpy( wildCard, name );
                    strcat( wildCard, ext );
                    if( command != cmFileInit )
                        fileList->select();
                    fileList->readDirectory( directory, wildCard );
                    }
                }
            else if( isDir( fName ) )
                {
                if( checkDirectory( fName ) )
                    {
                    delete (char *)directory;
                    strcat( fName, "\\" );
                    directory = newStr( fName );
                    if( command != cmFileInit )
                        fileList->select();
                    fileList->readDirectory( directory, wildCard );
                    }
                }
            else if( validFileName( fName ) )
                return True;
            else
                {
                messageBox( invalidFileText, mfError | mfOKButton );
                return False;
                }
            }
        else
            return True;
        }
    return False;
}

void TFileDialog::write( opstream& os )
{
    TDialog::write( os );
    os.writeString( wildCard );
    os << fileName << fileList;
}

void *TFileDialog::read( ipstream& is )
{
    TDialog::read( is );
    is.readString( wildCard, sizeof(wildCard) );
    is >> fileName >> fileList;
    readDirectory();
    return this;
}

TStreamable *TFileDialog::build()
{
    return new TFileDialog( streamableInit );
}

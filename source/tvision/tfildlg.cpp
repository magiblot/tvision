/*------------------------------------------------------------*/
/* filename -       tfildlg.cpp                               */
/*                                                            */
/* function(s)                                                */
/*                  TFileDialog member functions              */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

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
#define Uses_TProgram
#define Uses_opstream
#define Uses_ipstream
#include <tvision/tv.h>

#if !defined( __DIR_H )
#include <dir.h>
#endif  // __DIR_H

#if !defined( __ERRNO_H )
#include <errno.h>
#endif  // __ERRNO_H

#if !defined( __STDIO_H )
#include <stdio.h>
#endif  // __STDIO_H

#if !defined( __CTYPE_H )
#include <ctype.h>
#endif  // __CTYPE_H

#if !defined( __STRING_H )
#include <string.h>
#endif  // __STRING_H

TFileDialog::TFileDialog( TStringView aWildCard,
                          TStringView aTitle,
                          TStringView inputName,
                          ushort aOptions,
                          uchar histId
                        ) noexcept :
    TWindowInit( &TFileDialog::initFrame ),
    TDialog( TRect( 15, 1, 64, 20 ), aTitle ),
    directory( newStr("") )
{
    options |= ofCentered;
    flags |= wfGrow;
    strnzcpy( wildCard, aWildCard, sizeof( wildCard ) );

    TScrollBar *sb = new TScrollBar( TRect( 3, 11, 34, 12 ) );
    insert( sb );
    fileList = new TFileList( TRect( 3, 3, 34, 11 ), sb );
    fileList->growMode = gfGrowHiX | gfGrowHiY;
    insert( fileList );

    insert( new TLabel( TRect( 2, 2, 8, 3 ), filesText, fileList ) );
    first()->growMode = 0;

    ushort opt = bfDefault;
    TRect r( 35, 3, 46, 5 );

    if( (aOptions & fdOpenButton) != 0 )
        {
        insert( new TButton( r, openText, cmFileOpen, opt ) );
        first()->growMode = gfGrowLoX | gfGrowHiX;
        opt = bfNormal;
        r.a.y += 3;
        r.b.y += 3;
        }

    if( (aOptions & fdOKButton) != 0 )
        {
        insert( new TButton( r, okText, cmFileOpen, opt ) );
        first()->growMode = gfGrowLoX | gfGrowHiX;
        opt = bfNormal;
        r.a.y += 3;
        r.b.y += 3;
        }

    if( (aOptions & fdReplaceButton) != 0 )
        {
        insert( new TButton( r, replaceText, cmFileReplace, opt ) );
        first()->growMode = gfGrowLoX | gfGrowHiX;
        opt = bfNormal;
        r.a.y += 3;
        r.b.y += 3;
        }

    if( (aOptions & fdClearButton) != 0 )
        {
        insert( new TButton( r, clearText, cmFileClear, opt ) );
        first()->growMode = gfGrowLoX | gfGrowHiX;
        opt = bfNormal;
        r.a.y += 3;
        r.b.y += 3;
        }

    insert( new TButton( r, cancelText, cmCancel, bfNormal ) );
    first()->growMode = gfGrowLoX | gfGrowHiX;
    r.a.y += 3;
    r.b.y += 3;

    if( (aOptions & fdHelpButton) != 0 )
        {
        insert( new TButton( r, helpText, cmHelp, bfNormal ) );
        first()->growMode = gfGrowLoX | gfGrowHiX;
        opt = bfNormal;
        r.a.y += 3;
        r.b.y += 3;
        }

    // I feel too lazy to update all the sizes above. The new default size
    // is set by resizing the dialog.
    {
        // In the 21st century we should be using percentages, not absolute
        // units. :/
        TRect bounds = getBounds();
        TPoint screenSize = TProgram::application->size;
        TRect screenBounds = TProgram::application->getBounds();
        if (screenSize.x > 90)
            bounds.grow(15, 0); // New size 79
        else if (screenSize.x > 63)
            {
            screenBounds.grow(-7, 0);
            bounds.a.x = screenBounds.a.x;
            bounds.b.x = screenBounds.b.x;
            }
        if (screenSize.y > 34)
            bounds.grow(0, 5); // New height 29
        else if (screenSize.y > 25)
            {
            screenBounds.grow(0, -3);
            bounds.a.y = screenBounds.a.y;
            bounds.b.y = screenBounds.b.y;
            }
        // Making the dialog greater than this does not make much sense
        // as it would be too sparse.
        locate(bounds);
    }

    fileName = new TFileInputLine( TRect( 3, size.y - 5, size.x - 4, size.y - 4 ), MAXPATH );
    strnzcpy( fileName->data, wildCard, MAXPATH );
    fileName->growMode = gfGrowLoY | gfGrowHiY | gfGrowHiX;
    insert( fileName );

    insert( new TLabel( TRect( 2, size.y - 6, 3+cstrlen(inputName), size.y - 5 ),
                        inputName,
                        fileName
                      ) );
    first()->growMode = gfGrowLoY | gfGrowHiY;

    insert( new THistory( TRect( size.x - 4, size.y - 5, size.x - 1, size.y - 4 ), fileName, histId ) );
    first()->growMode = gfGrowLoX | gfGrowHiX | gfGrowLoY | gfGrowHiY; // This one shifts with the right edge

    insert( new TFileInfoPane( TRect( 1, size.y - 3, size.x - 1, size.y - 1 ) ) );
    first()->growMode = gfGrowHiX | gfGrowLoY | gfGrowHiY;

    selectNext( False );

    if( (aOptions & fdNoLoadDir) == 0 )
        readDirectory();
}

TFileDialog::~TFileDialog()
{
    delete[] (char *) directory;
}

void TFileDialog::shutDown()
{
    fileName = 0;
    fileList = 0;
    TDialog::shutDown();
}

void TFileDialog::sizeLimits( TPoint& min, TPoint& max )
{
    TDialog::sizeLimits( min, max );
    min.x = 49;
    min.y = 19;
}

/* 'src' is cast to unsigned char * so that isspace sign extends it
   correctly. */
static void trim( char *dest, const char *src ) noexcept
{
#ifndef __FLAT__
    while( *src != EOS && isspace( * (const unsigned char *) src ) )
        src++;
#endif
    while( *src != EOS
#ifndef __FLAT__
           && !isspace( * (const unsigned char *) src )
#endif
         )
        *dest++ = *src++;
    *dest = EOS;
}

void TFileDialog::getFileName( char *s ) noexcept
{
    char buf[2*MAXPATH];
    char drive[MAXDRIVE];
    char path[MAXDIR];
    char name[MAXFILE];
    char ext[MAXEXT];
    char TName[MAXFILE];
    char TExt[MAXEXT];

    trim( buf, fileName->data );
    fexpand( buf, directory );
    fnsplit( buf, drive, path, name, ext );
    if( name[0] == EOS && ext[0] == EOS )
        {
        fnsplit( wildCard, 0, 0, TName, TExt );
        fnmerge( buf, drive, path, TName, TExt );
        }
    strcpy( s, buf );
}

void TFileDialog::handleEvent(TEvent& event)
{
    TDialog::handleEvent(event);
    if( event.what == evCommand )
        {
        switch( event.message.command )
            {
            case cmFileOpen:
            case cmFileReplace:
            case cmFileClear:
                endModal(event.message.command);
                clearEvent(event);
                break;
            default:
                break;
            }
        }
    else if( event.what == evBroadcast && event.message.command == cmFileDoubleClicked )
        {
        event.what = evCommand;
        event.message.command = cmOK;
        putEvent( event );
        clearEvent( event );
        }
}

void TFileDialog::readDirectory()
{
    char curDir[MAXPATH];
    getCurDir( curDir );
    fexpand( curDir );
    if( directory )
        delete[] (char *) directory;
    directory = newStr( curDir );
    fileList->readDirectory( wildCard );
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
        messageBox( mfError | mfOKButton, "%s: '%s'", invalidDriveText, str );
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
char oldParent[MAXPATH];

    oldParent[0] = '\0';

    if( command == 0 )
        return True;

    if( TDialog::valid( command ) )
        {
        if( command != cmCancel && command != cmFileClear )
            {
            getFileName( fName );

            // Find the position of the first difference
            const char *p1 = fName;
            const char *p2 = directory;
            while (*p1 && *p2 && *p1 == *p2) {
                p1++; p2++;
            }

            // p2 points to the beginning of the old parent folder name
            // copy until next '/'
            const char *end = strpbrk(p2, "/\\");
            if (!end) end = p2 + strlen(p2);
            size_t len = end - p2;
            if (len < MAXPATH) {
                strncpy(oldParent, p2, len);
                oldParent[len] = '\0';
            }

            if( isWild( fName ) )
                {
                fnsplit( fName, drive, dir, name, ext );
                char path[MAXPATH];
                strcpy( path, drive );
                strcat( path, dir );
                if( checkDirectory( path ) )
                    {
                    delete[] (char *) directory;
                    directory = newStr( path );
                    strcpy( wildCard, name );
                    strcat( wildCard, ext );
                    if( command != cmFileInit )
                        fileList->select();
                    fileList->readDirectory( directory, wildCard );

                    TCollection *items = fileList->list();
                    if (items != 0)
                    {
                        for (short i = 0; i < items->getCount(); i++)
                        {
                            TSearchRec *item = (TSearchRec *)items->at(i);
                            if (item != NULL && stricmp(item->name, oldParent) == 0)
                            {
                                fileList->focusItem(i);
                                break;
                            }
                        }
                    }

                    }
                }
            else if( isDir( fName ) )
                {
                if( checkDirectory( fName ) )
                    {
                    delete[] (char *) directory;
                    strcat( fName, "\\" );
                    directory = newStr( fName );
                    if( command != cmFileInit )
                        fileList->select();
                    fileList->readDirectory( directory, wildCard );

                    TCollection *items = fileList->list();
                    if (items != 0)
                    {
                        for (short i = 0; i < items->getCount(); i++)
                        {
                            TSearchRec *item = (TSearchRec *)items->at(i);
                            if (item != NULL && stricmp(item->name, oldParent) == 0)
                            {
                                fileList->focusItem(i);
                                break;
                            }
                        }
                    }

                    }
                }
            else if( validFileName( fName ) )
                return True;
            else
                {
                messageBox( mfError | mfOKButton, "%s: '%s'", invalidFileText, fName );
                return False;
                }
            }
        else
            return True;
        }
    return False;
}

#if !defined(NO_STREAMABLE)

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


#endif

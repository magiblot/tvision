/*------------------------------------------------------------*/
/* filename -       tdircoll.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TDirCollection member functions           */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TDirCollection
#define Uses_TDirEntry
#define Uses_opstream
#define Uses_ipstream
#include <tvision\tv.h>

#if !defined( __DIR_H )
#include <Dir.h>
#endif  // __DIR_H

#if !defined( __STRING_H )
#include <String.h>
#endif  // __STRING_H

#if !defined( __DOS_H )
#include <Dos.h>
#endif  // __DOS_H

#pragma warn -asc

Boolean driveValid( char drive )
{
#if !defined( __FLAT__ )
I       MOV     AH, 19H     // Save the current drive in BL
I       INT     21H
I       MOV     BL, AL
I       MOV     DL, drive   // Select the given drive
I       SUB     DL, 'A'
I       MOV     AH, 0EH
I       INT     21H
I       MOV     AH, 19H     // Retrieve what DOS thinks is current
I       INT     21H
I       MOV     CX, 0       // Assume false
I       CMP     AL, DL      // Is the current drive the given drive?
I       JNE   __1
I       MOV     CX, 1       // It is, so the drive is valid
I       MOV     DL, BL      // Restore the old drive
I       MOV     AH, 0EH
I       INT     21H
__1:
I       XCHG    AX, CX      // Put the return value into AX
    return Boolean(_AX);
#else
    drive = (char) toupper( drive );
    DWORD mask = 0x01 << (drive - 'A');
    return (Boolean) (GetLogicalDrives() & mask);
#endif
}

#pragma warn .asc

Boolean isDir( const char *str )
{
    ffblk ff;
    return Boolean( findfirst( str, &ff, FA_DIREC ) == 0 &&
                    (ff.ff_attrib & FA_DIREC) != 0 );
}

Boolean pathValid( const char *path )
{
    char expPath[MAXPATH];
    strcpy( expPath, path );
    fexpand( expPath );
    int len = strlen(expPath);
    if( len <= 3 )
        return driveValid(expPath[0]);

    if( expPath[len-1] == '\\' )
        expPath[len-1] = EOS;

    return isDir( expPath );
}

Boolean validFileName( const char *fileName )
{
    static const char * const illegalChars = ";,=+<>|\"[] \\";

    char path[MAXPATH];
    char dir[MAXDIR];
    char name[MAXFILE];
    char ext[MAXEXT];

    fnsplit( fileName, path, dir, name, ext );
    strcat( path, dir );
    if( *dir != EOS && !pathValid( path ) )
        return False;
    if( strpbrk( name, illegalChars ) != 0 ||
        strpbrk( ext+1, illegalChars) != 0 ||
        strchr( ext+1, '.' ) != 0
      )
        return False;
    return True;
}

void getCurDir( char *dir )
{
    dir[0] = (char) (getdisk() + 'A');
    dir[1] = ':';
    dir[2] = '\\';
    dir[3] = '\0';
    getcurdir( 0, dir+3 );
    if( strlen( dir ) > 3 )
        strcat( dir, "\\" );
}

Boolean isWild( const char *f )
{
    return Boolean( strpbrk( f, "?*" ) != 0 );
}


TStreamable *TDirCollection::build()
{
    return new TDirCollection( streamableInit );
}

void TDirCollection::writeItem( void *obj, opstream& os )
{
    TDirEntry *item = (TDirEntry *)obj;
    os.writeString( item->text() );
    os.writeString( item->dir() );
}

void *TDirCollection::readItem( ipstream& is )
{
    char *txt = is.readString();
    char *dir = is.readString();
    TDirEntry *entry = new TDirEntry( txt, dir );
    delete txt;
    delete dir;
    return entry;
}


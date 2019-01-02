/*------------------------------------------------------------*/
/* filename -       tdircoll.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TDirCollection member functions           */
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

#define Uses_TDirCollection
#define Uses_TDirEntry
#define Uses_opstream
#define Uses_ipstream
#include <tv.h>

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
    asm {
        MOV DL, drive
        MOV AH, 36h
        SUB DL, 'A'-1
        INT 21h
        INC AX
        JE  __1
        MOV AL, 1
        }
__1:
    return Boolean(_AX); 
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
    dir[0] = getdisk() + 'A';
    dir[1] = ':';
    dir[2] = '\\';
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
    const char *txt = is.readString();
    const char *dir = is.readString();
    return new TDirEntry( txt, dir );
}

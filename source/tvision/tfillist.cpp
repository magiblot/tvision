/*------------------------------------------------------------*/
/* filename -       tfillist.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TFileList member functions                */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TVMemMgr
#define Uses_MsgBox
#define Uses_TFileList
#define Uses_TRect
#define Uses_TSearchRec
#define Uses_TEvent
#define Uses_TGroup
#define Uses_TKeys
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

#if !defined( __ASSERT_H )
#include <assert.h>
#endif  // __ASSERT_H

#if !defined( __DOS_H )
#include <dos.h>
#endif  // __DOS_H

#if !defined( __STRING_H )
#include <string.h>
#endif  // __STRING_H

#ifndef __BORLANDC__
#include <internal/filesys.h>
#include <system_error>
#include <ctime>
using std::tm;
static std::error_code ec = {};
#endif

#if defined( __FLAT__ ) && defined( __BORLANDC__)
extern "C" char _FAR * _CType _FARFUNC strupr(char _FAR *__s);
#endif


TFileList::TFileList( const TRect& bounds,
                      TScrollBar *aScrollBar) :
    TSortedListBox( bounds, 2, aScrollBar )
{
}

TFileList::~TFileList()
{
   if ( list() )
      destroy ( list() );
}

void TFileList::focusItem( short item )
{
    TSortedListBox::focusItem( item );
    message( owner, evBroadcast, cmFileFocused, list()->at(item) );
}

void TFileList::selectItem( short item )
{
    message( owner, evBroadcast, cmFileDoubleClicked, list()->at(item) );
}

void TFileList::getData( void * )
{
}

void TFileList::setData( void * )
{
}

ushort TFileList::dataSize()
{
    return 0;
}

void* TFileList::getKey( const char *s )
{
static TSearchRec sR;

    if( (shiftState & kbShift) != 0 || *s == '.' )
        sR.attr = FA_DIREC;
    else
        sR.attr = 0;
    strcpy( sR.name, s );
    strupr( sR.name );
    return &sR;
}

void TFileList::getText( char *dest, short item, short maxChars )
{
    TSearchRec *f = (TSearchRec *)(list()->at(item));

    strncpy( dest, f->name, maxChars );
    dest[maxChars] = '\0';
    if( f->attr & FA_DIREC )
        strcat( dest, dirSeparator );
}


void TFileList::readDirectory( const char *dir, const char *wildCard )
{
    char path[MAXPATH];
    strcpy( path, dir );
    strcat( path, wildCard );
    readDirectory( path );
}

struct DirSearchRec : public TSearchRec
{
    void readFf_blk(ffblk *f)
    {
        attr = (char)f->ff_attrib;
    time = (((long)(unsigned)f->ff_fdate)<<16) | f->ff_ftime;
    size = f->ff_fsize;
    memcpy(name, f->ff_name, sizeof(name));
    }

#ifndef __BORLANDC__
    void readDirEntry(const fs::directory_entry &d) 
    {
        attr = d.is_regular_file() ? FA_NORMAL : d.is_directory() ? FA_DIREC : FA_SYSTEM;
        size = max(d.file_size(ec), 0);
        readTime(to_time_t(d.last_write_time(ec)));
        strncpy(name, d.path().filename().c_str(), sizeof(DirSearchRec::name));
        name[sizeof(DirSearchRec::name) - 1] = '\0';
    }

    void readTime(const time_t &t)
    {
        struct tm *lt = localtime(&t);
        // From ffblk spec: http://www.delorie.com/djgpp/doc/libc/libc_326.html
        // unsigned short ff_ftime;  /* hours:5, minutes:6, (seconds/2):5 */
        // unsigned short ff_fdate;  /* (year-1980):7, month:4, day:5 */
        time = (((( // Concatenation of ff_fdate and ff_ftime.
            (lt->tm_year - 80)
            << 4 | (lt->tm_mon + 1))
            << 5 | lt->tm_mday)
            << 5 | lt->tm_hour)
            << 6 | lt->tm_min)
            << 5 | (lt->tm_sec/2);
    }
#endif

    void *operator new( size_t );

};

void *DirSearchRec::operator new( size_t sz )
{
    void *temp = ::operator new( sz );
    if( TVMemMgr::safetyPoolExhausted() )
        {
        delete temp;
        temp = 0;
        }
    return temp;
}

void TFileList::readDirectory( const char *aWildCard )
{
    TFileCollection *fileList = new TFileCollection( 5, 5 );
#ifdef __BORLANDC__
    ffblk s;
    char path[MAXPATH];
    char drive[MAXDRIVE];
    char dir[MAXDIR];
    char file[MAXFILE];
    char ext[MAXEXT];
    const unsigned findAttr = FA_RDONLY | FA_ARCH;
    memset(&s, 0, sizeof(s));
    strcpy( path, aWildCard );
    fexpand( path );
    fnsplit( path, drive, dir, file, ext );

    int res = findfirst( aWildCard, &s, findAttr );
    DirSearchRec *p = (DirSearchRec *)&p;
    while( p != 0 && res == 0 )
        {
        if( (s.ff_attrib & FA_DIREC) == 0 )
            {
            p = new DirSearchRec;
            if( p != 0 )
                {
        p->readFf_blk(&s);
                fileList->insert( p );
                }
            }
        res = findnext( &s );
        }

    fnmerge( path, drive, dir, "*", ".*" );

    res = findfirst( path, &s, FA_DIREC );
    while( p != 0 && res == 0 )
        {
        if( (s.ff_attrib & FA_DIREC) != 0 && s.ff_name[0] != '.' )
            {
            p = new DirSearchRec;
            if( p != 0 )
                {
        p->readFf_blk(&s);
                fileList->insert( p );
                }
            }
        res = findnext( &s );
        }

    if( strlen( dir ) > 1 )
        {
        p = new DirSearchRec;
        if( p != 0 )
            {
            if( findfirst( path, &s, FA_DIREC ) == 0 &&
                findnext( &s ) == 0 &&
                strcmp( s.ff_name, ".." ) == 0
            )
            p->readFf_blk(&s);
            else
                {
                strcpy( p->name, ".." );
                p->size = 0;
                p->time = 0x210000uL;
                p->attr = FA_DIREC;
                }
            fileList->insert( p );
            }
        }

    if( p == 0 )
        messageBox( tooManyFiles, mfOKButton | mfWarning );
#else
    fs::path wild(aWildCard);
    fs::path dir(wild.has_parent_path() ? wild.parent_path() : ".");
    wild = wild.has_filename() ? wild.filename() : "*";
    
    if ( const auto &parent = fs::directory_entry(fs::path(".."), ec); !ec )
        {
        DirSearchRec *p = new DirSearchRec;
        p->readDirEntry( parent );
        fileList->insert( p );
        }

    for ( const fs::directory_entry &d : fs::directory_iterator(dir, ec) )
        {
        fs::path fName = d.path().filename();
        if ( d.is_directory(ec) || match(wild.c_str(), fName.c_str()) )
            {
            DirSearchRec *p = new DirSearchRec;
            p->readDirEntry( d );
            fileList->insert( p );
            }
        }
#endif
    newList(fileList);
    if( list()->getCount() > 0 )
        message( owner, evBroadcast, cmFileFocused, list()->at(0) );
    else
        {
        static DirSearchRec noFile;
        message( owner, evBroadcast, cmFileFocused, &noFile );
        }
}

#ifndef __BORLANDC__
bool match(char const *wildcard, char const *filename)
{
    // https://stackoverflow.com/a/3300547
    for (; *wildcard != '\0'; ++wildcard)
        switch (*wildcard)
        {
            case '?': 
                if (*filename == '\0')
                    return false;
                ++filename;
                break;
            case '*':
                if (wildcard[1] == '\0')
                    return true;
                for (size_t i = 0; filename[i] != '\0'; ++i)
                    if (match(&wildcard[1], &filename[i]))
                        return true;
                return false;
            default:
                if (*filename != *wildcard)
                    return false;
                ++filename;
        }
    return *filename == '\0';
}
#endif

/*
    fexpand:    reimplementation of pascal's FExpand routine.  Takes a
                relative DOS path and makes an absolute path of the form

                    drive:\[subdir\ ...]filename.ext

                works with '/' or '\' as the subdir separator on input;
                changes all to '\' on output.

*/

void squeeze( char *path )
{
    char *dest = path;
    char *src = path;
    while( *src != 0 )
        {
        if( *src != '.' )
            *dest++ = *src++;   // just copy it...
        else
            {
            src++;
            if( *src == '.' && *(src + 1) == '\\') // if it's a '..' followed by '\'
                {               // have a '..'
                src += 2;       // skip the following '\'
                dest--;         // back up to the previous '\'
                while( dest > path && *--dest != '\\' ) // back up to the previous '\'
                    ;
                dest++;         // move to the next position
                }
            else if (*src == '\\') // if it's a '.' followed by '\'
                src++;          // skip the following '\'
            else
                *dest++ = *(src - 1); // copy the '.' we just skipped
            }
        }
    *dest = EOS;                // zero terminator
}

void fexpand( char *rpath )
{
#ifdef __BORLANDC__
    char path[MAXPATH];
    char drive[MAXDRIVE];
    char dir[MAXDIR];
    char file[MAXFILE];
    char ext[MAXEXT];

    int flags = fnsplit( rpath, drive, dir, file, ext );
    if( (flags & DRIVE) == 0 )
        {
        drive[0] = getdisk() + 'A';
        drive[1] = ':';
        drive[2] = '\0';
        }
    drive[0] = toupper(drive[0]);
    if( (flags & DIRECTORY) == 0 || (dir[0] != '\\' && dir[0] != '/') )
        {
        char curdir[MAXDIR];
        getcurdir( drive[0] - 'A' + 1, curdir );
        strcat( curdir, dir );
        if( *curdir != '\\' && *curdir != '/' )
            {
            *dir = '\\';
            strcpy( dir+1, curdir );
            }
        else
            strcpy( dir, curdir );
        }

    squeeze( dir );
    char *p = dir;
    while( (p = strchr( p, '/' )) != 0 )
        *p = '\\';
    fnmerge( path, drive, dir, file, ext );
    strupr( path );
    strcpy( rpath, path );
#else
    strnzcpy(rpath, fexpand(fs::path(rpath)).c_str(), MAXPATH);
#endif
}

#ifndef __BORLANDC__
fs::path fexpand(const fs::path &p)
{
    return fs::absolute(p.lexically_normal());
}
#endif

#if !defined(NO_STREAMABLE)

TStreamable *TFileList::build()
{
    return new TFileList( streamableInit );
}

#endif

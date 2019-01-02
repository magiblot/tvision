/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   STDDLG.H                                                              */
/*                                                                         */
/*   Copyright (c) Borland International 1991                              */
/*   All Rights Reserved.                                                  */
/*                                                                         */
/*   defines the classes TFileInputLine, TFileCollection, TSortedListBox,  */
/*   TFileList, TFileInfoPane, TFileDialog, TDirCollection, TDirListBox,   */
/*   and TChDirDialog                                                      */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#pragma warn -hid

#pragma option -Vo-
#if defined( __BCOPT__ )
#pragma option -po-
#endif

#if !defined( __FILE_CMDS )
#define __FILE_CMDS

const

//  Commands

    cmFileOpen    = 1001,   // Returned from TFileDialog when Open pressed
    cmFileReplace = 1002,   // Returned from TFileDialog when Replace pressed
    cmFileClear   = 1003,   // Returned from TFileDialog when Clear pressed
    cmFileInit    = 1004,   // Used by TFileDialog internally
    cmChangeDir   = 1005,   // Used by TChDirDialog internally
    cmRevert      = 1006,   // Used by TChDirDialog internally

//  Messages

    cmFileFocused = 102,    // A new file was focused in the TFileList
    cmFileDoubleClicked     // A file was selected in the TFileList
            = 103;

#endif  // __FILE_CMDS

#if defined( Uses_TSearchRec ) && !defined( __TSearchRec )
#define __TSearchRec
                
#if !defined( __DIR_H )
#include <Dir.h>
#endif  // __DIR_H

struct TSearchRec
{
    uchar attr;
    long time;
    long size;
    char name[MAXFILE+MAXEXT-1];
};

#endif  // Uses_TSearchRec

#if defined( Uses_TFileInputLine ) && !defined( __TFileInputLine )
#define __TFileInputLine

class far TRect;
class far TEvent;

class TFileInputLine : public TInputLine
{

public:

    TFileInputLine( const TRect& bounds, short aMaxLen );

    virtual void handleEvent( TEvent& event );

private:

    virtual const char *streamableName() const
        { return name; }

protected:

    TFileInputLine( StreamableInit );

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TFileInputLine& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TFileInputLine*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TFileInputLine& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TFileInputLine* cl )
    { return os << (TStreamable *)cl; }

#endif  // Uses_TFileInputLine

#if defined( Uses_TFileCollection ) && !defined( __TFileCollection )
#define __TFileCollection

class far TSearchRec;

class TFileCollection: public TSortedCollection
{

public:

    TFileCollection( ccIndex aLimit, ccIndex aDelta) :
        TSortedCollection( aLimit, aDelta ) {}

    TSearchRec *at( ccIndex index )
        { return (TSearchRec *)TSortedCollection::at( index ); }
    virtual ccIndex indexOf( TSearchRec *item )
        { return TSortedCollection::indexOf( item ); }

    void remove( TSearchRec *item )
        { TSortedCollection::remove( item ); }
    void free( TSearchRec *item )
        { TSortedCollection::free( item ); }
    void atInsert( ccIndex index, TSearchRec *item )
        { TSortedCollection::atInsert( index, item ); }
    void atPut( ccIndex index, TSearchRec *item )
        { TSortedCollection::atPut( index, item ); }
    virtual ccIndex insert( TSearchRec *item )
        { return TSortedCollection::insert( item ); }

    TSearchRec *firstThat( ccTestFunc Test, void *arg );
    TSearchRec *lastThat( ccTestFunc Test, void *arg );

private:

    virtual void freeItem( void *item )
        { delete (TSearchRec *)item; }

    virtual int compare( void *key1, void *key2 );

    virtual const char *streamableName() const
        { return name; }

    virtual void *readItem( ipstream& );
    virtual void writeItem( void *, opstream& );

protected:

    TFileCollection( StreamableInit ) : TSortedCollection ( streamableInit ) {}

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TFileCollection& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TFileCollection*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TFileCollection& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TFileCollection* cl )
    { return os << (TStreamable *)cl; }

inline TSearchRec *TFileCollection::firstThat( ccTestFunc func, void *arg )
{
    return (TSearchRec *)TSortedCollection::firstThat( ccTestFunc(func), arg );
}

inline TSearchRec *TFileCollection::lastThat( ccTestFunc func, void *arg )
{
    return (TSearchRec *)TSortedCollection::lastThat( ccTestFunc(func), arg );
}

#endif  // Uses_TFileCollection


#if defined( Uses_TSortedListBox ) && !defined( __TSortedListBox )
#define __TSortedListBox

class far TRect;
class far TScrollBar;
class far TEvent;
class far TSortedCollection;

class TSortedListBox: public TListBox
{

public:

    TSortedListBox( const TRect& bounds,
                    ushort aNumCols,
                    TScrollBar *aScrollBar
                  );

    virtual void handleEvent( TEvent& event );
    void newList( TSortedCollection *aList );

    TSortedCollection *list();

protected:

    uchar shiftState;

private:

    virtual void *getKey( const char *s );

    ushort searchPos;

    virtual const char *streamableName() const
        { return name; }

protected:

    TSortedListBox( StreamableInit ) : TListBox ( streamableInit ) {}

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TSortedListBox& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TSortedListBox*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TSortedListBox& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TSortedListBox* cl )
    { return os << (TStreamable *)cl; }

inline TSortedCollection *TSortedListBox::list()
{
    return (TSortedCollection *)TListBox::list();
}

#endif  // Uses_TSortedListBox

#if defined( Uses_TFileList ) && !defined( __TFileList )
#define __TFileList

class far TRect;
class far TScrollBar;
class far TEvent;

class TFileList : public TSortedListBox
{

public:

    TFileList( const TRect& bounds,
               TScrollBar *aScrollBar
             );
    ~TFileList();

    virtual void focusItem( short item );
    virtual void getText( char *dest, short item, short maxLen );
    virtual void handleEvent( TEvent& event );
    void newList( TFileCollection *aList );
    void readDirectory( const char *dir, const char *wildCard );
    void readDirectory( const char *wildCard );

    virtual ushort dataSize();
    virtual void getData( void *rec );
    virtual void setData( void *rec );

    TFileCollection *list();

private:

    virtual void *getKey( const char *s );

    static const char * near tooManyFiles;
    
    virtual const char *streamableName() const
        { return name; }

protected:

    TFileList( StreamableInit ) : TSortedListBox ( streamableInit ) {}

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TFileList& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TFileList*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TFileList& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TFileList* cl )
    { return os << (TStreamable *)cl; }

inline void TFileList::newList( TFileCollection *f )
{
    TSortedListBox::newList( f );
}

inline TFileCollection *TFileList::list()
{
    return (TFileCollection *)TSortedListBox::list();
}

#endif  // Uses_TFileList


#if defined( Uses_TFileInfoPane ) && !defined( __TFileInfoPane )
#define __TFileInfoPane

class far TRect;
class far TEvent;

class TFileInfoPane : public TView
{

public:

    TFileInfoPane( const TRect& bounds );

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& event );

private:

    TSearchRec file_block;

    static const char * const near months[13];
    static const char * near pmText;
    static const char * near amText;

    virtual const char *streamableName() const
        { return name; }

protected:

    TFileInfoPane( StreamableInit ) : TView ( streamableInit ) {}

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TFileInfoPane& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TFileInfoPane*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TFileInfoPane& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TFileInfoPane* cl )
    { return os << (TStreamable *)cl; }

#endif  // Uses_TFileInfoPane

#if defined( Uses_TFileDialog ) && !defined( __TFileDialog )
#define __TFileDialog

const
    fdOKButton      = 0x0001,      // Put an OK button in the dialog
    fdOpenButton    = 0x0002,      // Put an Open button in the dialog
    fdReplaceButton = 0x0004,      // Put a Replace button in the dialog
    fdClearButton   = 0x0008,      // Put a Clear button in the dialog
    fdHelpButton    = 0x0010,      // Put a Help button in the dialog
    fdNoLoadDir     = 0x0100;      // Do not load the current directory
                                   // contents into the dialog at Init.
                                   // This means you intend to change the
                                   // WildCard by using SetData or store
                                   // the dialog on a stream.

#if !defined( __DIR_H )
#include <Dir.h>
#endif  // __DIR_H

class far TEvent;
class far TFileInputLine;
class far TFileList;

class TFileDialog : public TDialog
{

public:

    TFileDialog( const char *aWildCard, const char *aTitle,
                 const char *inputName, ushort aOptions, uchar histId );
    ~TFileDialog();

    virtual void getData( void *rec );
    void getFileName( char *s );
    virtual void handleEvent( TEvent& event );
    virtual void setData( void *rec );
    virtual Boolean valid( ushort command );
    virtual void shutDown();

    TFileInputLine *fileName;
    TFileList *fileList;
    char wildCard[MAXPATH];
    const char *directory;

private:

    void readDirectory();

    Boolean checkDirectory( const char * );

    static const char * near filesText;
    static const char * near openText;
    static const char * near okText;
    static const char * near replaceText;
    static const char * near clearText;
    static const char * near cancelText;
    static const char * near helpText;
    static const char * near invalidDriveText;
    static const char * near invalidFileText;
    
    virtual const char *streamableName() const
        { return name; }

protected:

    TFileDialog( StreamableInit ) : TDialog ( streamableInit ), 
        TWindowInit( &TFileDialog::initFrame ) {}       
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TFileDialog& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TFileDialog*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TFileDialog& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TFileDialog* cl )
    { return os << (TStreamable *)cl; }

#endif  // Uses_TFileDialog


#if defined( Uses_TDirEntry ) && !defined( __TDirEntry )
#define __TDirEntry

class TDirEntry
{

public:

    TDirEntry( const char *, const char * );
    ~TDirEntry();
    char *dir() { return directory; }
    char *text() { return displayText; }

private:

    char *displayText;
    char *directory;

};

inline TDirEntry::TDirEntry( const char *txt, const char *dir ) :
    displayText( newStr( txt ) ), directory( newStr( dir ) )
{
}

inline TDirEntry::~TDirEntry()
{
    delete displayText;
    delete directory;
}

#endif  // Uses_TDirEntry

#if defined( Uses_TDirCollection ) && !defined( __TDirCollection )
#define __TDirCollection

class far TDirEntry;

class TDirCollection : public TCollection
{

public:

    TDirCollection( ccIndex aLimit, ccIndex aDelta) :
        TCollection( aLimit, aDelta ) {}

    TDirEntry *at( ccIndex index )
        { return (TDirEntry *)TCollection::at( index );}
    virtual ccIndex indexOf( TDirEntry *item )
        { return TCollection::indexOf( item ); }

    void remove( TDirEntry *item )
        { TCollection::remove( item ); }
    void free( TDirEntry *item )
        { TCollection::free( item ); }
    void atInsert( ccIndex index, TDirEntry *item )
        { TCollection::atInsert( index, item ); }
    void atPut( ccIndex index, TDirEntry *item )
        { TCollection::atPut( index, item ); }
    virtual ccIndex insert( TDirEntry *item )
        { return TCollection::insert( item ); }

    TDirEntry *firstThat( ccTestFunc Test, void *arg );
    TDirEntry *lastThat( ccTestFunc Test, void *arg );

private:

    virtual void freeItem( void *item )
        { delete (TDirEntry *)item; }

    virtual const char *streamableName() const
        { return name; }
    virtual void *readItem( ipstream& );
    virtual void writeItem( void *, opstream& );

protected:

    TDirCollection( StreamableInit ) : TCollection ( streamableInit ) {}

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TDirCollection& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TDirCollection*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TDirCollection& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TDirCollection* cl )
    { return os << (TStreamable *)cl; }

inline TDirEntry *TDirCollection::firstThat( ccTestFunc func, void *arg )
{
    return (TDirEntry *)TCollection::firstThat( ccTestFunc(func), arg );
}

inline TDirEntry *TDirCollection::lastThat( ccTestFunc func, void *arg )
{
    return (TDirEntry *)TCollection::lastThat( ccTestFunc(func), arg );
}

#endif  // Uses_TDirCollection


#if defined( Uses_TDirListBox ) && !defined( __TDirListBox )
#define __TDirListBox

#if !defined( __DIR_H )
#include <Dir.h>
#endif  // __DIR_H

class far TRect;
class far TScrollBar;
class far TEvent;
class far TDirCollection;

class TDirListBox : public TListBox
{

public:

    TDirListBox( const TRect& bounds, TScrollBar *aScrollBar );
    ~TDirListBox();

    virtual void getText( char *, short, short );
    virtual void handleEvent( TEvent& );
    virtual Boolean isSelected( short );
    void newDirectory( const char * );
    virtual void setState( ushort aState, Boolean enable );

    TDirCollection *list();

private:

    void showDrives( TDirCollection * );
    void showDirs( TDirCollection * );

    char dir[MAXPATH];
    ushort cur;

    static const char * near pathDir;
    static const char * near firstDir;
    static const char * near middleDir;
    static const char * near lastDir;
    static const char * near drives;
    static const char * near graphics;

    virtual const char *streamableName() const
        { return name; }

protected:

    TDirListBox( StreamableInit ): TListBox( streamableInit ) {}

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TDirListBox& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TDirListBox*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TDirListBox& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TDirListBox* cl )
    { return os << (TStreamable *)cl; }

inline TDirCollection *TDirListBox::list()
{
    return (TDirCollection *)TListBox::list();
}

#endif  // Uses_TDirListBox

#if defined( Uses_TChDirDialog ) && !defined( __TChDirDialog )
#define __TChDirDialog

const
    cdNormal     = 0x0000, // Option to use dialog immediately
    cdNoLoadDir  = 0x0001, // Option to init the dialog to store on a stream
    cdHelpButton = 0x0002; // Put a help button in the dialog

class far TEvent;
class far TInputLine;
class far TDirListBox;
class far TButton;

class TChDirDialog : public TDialog
{

public:

    friend class TDirListBox;

    TChDirDialog( ushort aOptions, ushort histId );
    virtual ushort dataSize();
    virtual void getData( void *rec );
    virtual void handleEvent( TEvent& );
    virtual void setData( void *rec );
    virtual Boolean valid( ushort );
    virtual void shutDown();

private:

    void setUpDialog();

    TInputLine *dirInput;
    TDirListBox *dirList;
    TButton *okButton;
    TButton *chDirButton;

    static const char * near changeDirTitle;
    static const char * near dirNameText;
    static const char * near dirTreeText;
    static const char * near okText;
    static const char * near chdirText;
    static const char * near revertText;
    static const char * near helpText;
    static const char * near drivesText;
    static const char * near invalidText;

    virtual const char *streamableName() const
        { return name; }

protected:

    TChDirDialog( StreamableInit ) : TDialog( streamableInit ),
        TWindowInit( &TChDirDialog::initFrame ) {}
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TChDirDialog& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TChDirDialog*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TChDirDialog& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TChDirDialog* cl )
    { return os << (TStreamable *)cl; }

#endif  // Uses_TChDirDialog

#pragma option -Vo.
#if defined( __BCOPT__ )
#pragma option -po.
#endif


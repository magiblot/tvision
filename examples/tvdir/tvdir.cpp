/*---------------------------------------------------------*/
/*                                                         */
/*---------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TKeys
#define Uses_TApplication
#define Uses_TEvent
#define Uses_TRect
#define Uses_TDialog
#define Uses_TStaticText
#define Uses_TButton
#define Uses_TMenuBar
#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_TDeskTop
#define Uses_TOutline
#define Uses_TScrollBar
#define Uses_TParamText
#include <tvision\tv.h>
#include <dos.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>

const cmDirTree       = 100;
const cmAbout         = 101;
const cmNewDirFocused = 102;

class QuickMessage: public TWindow
{
    TParamText *currentDir;
public:
    QuickMessage( const char *drive ):
        TWindow( TRect( 25,10,55,16 ), "Please Wait...", 0 ),
        TWindowInit( TWindow::initFrame ) {

    flags = 0; // no move, close, grow or zoom
    options |= ofCentered;
    palette = wpGrayWindow;
    char temp[30];
    sprintf(temp,"Scanning Drive %s\n",drive );
    insert( new TStaticText( TRect( 2,2,28,3 ), temp ) );
    currentDir = new TParamText( TRect( 2,3,28,4 ) );
    insert( currentDir );
  }
  virtual void handleEvent( TEvent &event ) {
    TWindow::handleEvent( event );
  }
  void setCurrentDir( char *newDir ) {
    currentDir->setText( newDir );
  }
};

class TDirOutline: public TOutline {
  static TNode *parentSearch;
public:
  TDirOutline( TRect &bounds, TScrollBar *hsb, TScrollBar *vsb, TNode *root ):
    TOutline( bounds, hsb, vsb, root ) {}
  virtual void focused( i ) {
    foc=i;
    message( owner, evCommand, cmNewDirFocused, 0 );
  }
  static Boolean isParent( TOutlineViewer *, TNode *cur, int, int, long, ushort );
  TNode *getParent( TNode *child ) {
    parentSearch=child;
    return firstThat( isParent );
  }
  void getCurrentPath( char *buffer, short bufferSize );
};

Boolean TDirOutline::isParent( TOutlineViewer *, TNode *cur, int, int, long, ushort ) {
    TNode *temp = cur->childList;
    while (temp!=0) {
      if (temp==parentSearch)
        return True;
      temp=temp->next;
    }
    return False;
}

void TDirOutline::getCurrentPath( char *buffer, short bufferSize ) {
    char temp1[128],temp2[128];
    TNode *current = getNode( foc );
    TNode *root = getRoot();

    temp1[0]=0;
    while (current!=root) {
      strcpy(temp2,temp1);
      strcpy(temp1,current->text);
      strcat(temp1,"\\");
      strcat(temp1,temp2);
      current = getParent( current );
    }
    strncpy(buffer,root->text,bufferSize);
    strncat(buffer,"\\",bufferSize);
    strncat(buffer,temp1,bufferSize);
}

TNode *TDirOutline::parentSearch;

TNode *getDirList( const char *path, QuickMessage *qm = 0 ) {
  TNode  *dirList,
         *current;
  char   searchPath[128];
  find_t searchRec;
  int    result;
  TNode  *temp;

  sprintf(searchPath,"%s\\*.*",path);
  result = _dos_findfirst( searchPath, 0xff, &searchRec );
  dirList = 0;
  current = 0;

  while (result==0) {
    if (searchRec.name[0]!='.') {
      if (searchRec.attrib & FA_DIREC) {
        sprintf(searchPath,"%s\\%s",path,searchRec.name);
        qm->setCurrentDir(searchPath);
        temp = new TNode( searchRec.name, getDirList(searchPath,qm), 0, False );
        if (current) {
          current->next = temp;
          current=current->next;
        } else
          current = dirList = temp;
      }
    }
    result = _dos_findnext( &searchRec );
  }
  return dirList;
}

class TFilePane: public TScroller {
  char *currentPath;
  char **files;
  short fileCount;

public:
  TFilePane( TRect &bounds, TScrollBar *hsb, TScrollBar *vsb ):
    TScroller( bounds, hsb, vsb ) {
    fileCount=0;
    files=0;
  }
  void newDir( const char *path );
  virtual void draw();
};

void TFilePane::draw() {
    TDrawBuffer dBuf;
    short i;
    for (i=0;i<size.y;i++) {
      dBuf.moveChar(0, ' ', getColor(0x0101), (short)size.x );
      if ((fileCount==0)&&(i==0))
        dBuf.moveCStr( 2, "<no files>", getColor(0x0101) );
      if ((i+delta.y)<fileCount)
        dBuf.moveCStr( 2, &files[i+delta.y][delta.x], getColor(0x0101) );
      writeLine( 0, i, (short)size.x, 1, dBuf );
    }
  }

void TFilePane::newDir( const char *path ) {
    char searchPath[128];
    find_t searchRec;
    int result;
    short i;

    for (i=0;i<fileCount;i++)
      delete files[i];
    delete [] files;

    sprintf(searchPath,"%s*.*",path);
    fileCount=0;
    result = _dos_findfirst( searchPath, 0xff, &searchRec );
    while (result==0) {
      if (!(searchRec.attrib & FA_DIREC))
        fileCount++;
      result=_dos_findnext( &searchRec );
    }
    files = new char *[fileCount];
    result = _dos_findfirst( searchPath, 0xff, &searchRec );
    i=0;
    while (result==0) {
      if (!(searchRec.attrib & FA_DIREC)) {
          sprintf(searchPath,"%-12s  %8ld %2d-%02d-%02d  %2d:%02d  %c%c%c%c",searchRec.name,searchRec.size,
                    ((searchRec.wr_date & 0x01E0) >> 5),
                    (searchRec.wr_date & 0x001F),
                    ((searchRec.wr_date >> 9)+1980)%100,
                    ((searchRec.wr_time & 0xF800) >> 11)%13,
                    ((searchRec.wr_time & 0x07E0) >> 5),
                    searchRec.attrib & FA_ARCH   ? 'a' : 'ú',
                    searchRec.attrib & FA_RDONLY ? 'r' : 'ú',
                    searchRec.attrib & FA_SYSTEM ? 's' : 'ú',
                    searchRec.attrib & FA_HIDDEN ? 'h' : 'ú' );
        files[i++] = newStr(searchPath);
      }
      result=_dos_findnext( &searchRec );
    }
    if (fileCount==0)
      setLimit( 1, 1 );
    else
      setLimit( strlen(files[0]), fileCount );
    drawView();
  }

class TDirWindow: public TWindow {
  char *drive;
  TNode *dirTree;
  TDirOutline *ol;
  TFilePane   *fp;
  TScrollBar *hsb, *vsb;
public:
  TDirWindow( const char *driveInit ):
    TWindow( TRect( 1,1,76,21 ), driveInit, 0 ),
    TWindowInit( TWindow::initFrame ) {

    char temp[12];

    drive = newStr( driveInit );

    vsb = new TScrollBar( TRect( 74,1,75,15 ) );
    hsb = new TScrollBar( TRect( 22,15,73,16 ) );

    fp = new TFilePane( TRect( 21,1,74,15 ), hsb, vsb );
    fp->options |= ofFramed;
    fp->growMode = gfGrowHiY | gfGrowHiX | gfFixed;

    insert( hsb );
    insert( vsb );
    insert( fp );

    vsb = new TScrollBar( TRect( 20,1,21,19 ) );
    hsb = new TScrollBar( TRect( 2,19,19,20 ) );

    QuickMessage *qm = new QuickMessage( drive );
    TProgram::deskTop->insert( qm );

    dirTree = new TNode( drive, getDirList( drive, qm ), 0, True );

    TProgram::deskTop->remove( qm );
    destroy(qm);

    ol = new TDirOutline( TRect( 1,1,20,19 ), hsb, vsb, dirTree );
    ol->options |= ofFramed;
    ol->growMode = gfGrowHiY | gfFixed;
    vsb->growMode = gfGrowHiY;
    hsb->growMode = gfGrowHiY | gfGrowLoY;

    insert( hsb );
    insert( vsb );
    insert( ol );

    ol->getCurrentPath( temp, 10 );
    fp->newDir( temp );

  }
  virtual void handleEvent( TEvent &event ) {
    char buffer[128];
    if ((event.what == evCommand) &&
        (event.message.command == cmNewDirFocused )) {
       ol->getCurrentPath(buffer,128);
       fp->newDir(buffer);
       delete (void *)title;
       title = newStr(buffer);
       clearEvent(event);
       ((TView *)frame)->drawView();
    }
    TWindow::handleEvent( event );
  }
  virtual void sizeLimits( TPoint &min, TPoint &max ) {
    min.x = 40;
    min.y = 10;
    max = owner->size;
  }
};

class TDirApp : public TApplication
{
   char *drive;

public:

    TDirApp( const char *driveInit );

    virtual void handleEvent( TEvent& event );
    static TMenuBar *initMenuBar( TRect );
    static TStatusLine *initStatusLine( TRect );
    void aboutBox( void );

};

TDirApp::TDirApp( const char *driveInit ) :
    TProgInit( &TDirApp::initStatusLine,
               &TDirApp::initMenuBar,
               &TDirApp::initDeskTop
             )
{
   drive = newStr(driveInit);
   insertWindow( new TDirWindow( driveInit ) );
}

void TDirApp::handleEvent( TEvent& event )
{
    TApplication::handleEvent( event );
    if( event.what == evCommand )
        {
        switch( event.message.command )
            {
            case cmAbout:
                aboutBox();
                clearEvent( event );
                break;
            case cmDirTree:
                insertWindow( new TDirWindow( drive ) );
                clearEvent( event );
                break;
            default:
                break;
            }
        }
}

TMenuBar *TDirApp::initMenuBar( TRect r )
{

    r.b.y = r.a.y+1;

    return new TMenuBar( r,
      *new TSubMenu( "~ð~", kbAltSpace ) +
        *new TMenuItem( "~A~bout...", cmAbout, kbAltA ) +
      *new TSubMenu( "~F~ile", kbAltF ) +
        *new TMenuItem( "~N~ew Window...", cmDirTree, kbAltN ) +
         newLine() +
        *new TMenuItem( "E~x~it", cmQuit, cmQuit, hcNoContext, "Alt-X" )
        );

}

TStatusLine *TDirApp::initStatusLine( TRect r )
{
    r.a.y = r.b.y-1;
    return new TStatusLine( r,
        *new TStatusDef( 0, 0xFFFF ) +
            *new TStatusItem( "~Alt-X~ Exit", kbAltX, cmQuit ) +
            *new TStatusItem( 0, kbF10, cmMenu )
            );
}

void TDirApp::aboutBox( void ) {
    TDialog *aboutBox = new TDialog(TRect(0, 0, 39, 11), "About");

    aboutBox->insert(
      new TStaticText(TRect(9, 2, 30, 7),
        "\003Outline Viewer Demo\n\n"       // These strings will be
        "\003Copyright (c) 1994\n\n"       // The \003 centers the line.
        "\003Borland International"
        )
      );

    aboutBox->insert(
      new TButton(TRect(14, 8, 25, 10), " OK", cmOK, bfDefault)
      );

    aboutBox->options |= ofCentered;

    executeDialog(aboutBox);

}

int main( int argc, char *argv[] )
{
    char drive[20];

    if (argc==2)
      strcpy(drive,argv[1]);
    else
      strcpy(drive,"C:");

    TDirApp *dirApp = new TDirApp( drive );
    dirApp->run();
    TObject::destroy(dirApp);
    return 0;
}

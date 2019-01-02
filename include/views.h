/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   VIEWS.H                                                               */
/*                                                                         */
/*   Copyright (c) Borland International 1991                              */
/*   All Rights Reserved.                                                  */
/*                                                                         */
/*   defines the classes TView, TFrame, TScrollBar, TScroller,             */
/*   TListViewer, TGroup, and TWindow                                      */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#pragma option -Vo-
#if defined( __BCOPT__ )
#pragma option -po-
#endif

#if !defined( __COMMAND_CODES )
#define __COMMAND_CODES

const ushort

//  Standard command codes

    cmValid         = 0,
    cmQuit          = 1,
    cmError         = 2,
    cmMenu          = 3,
    cmClose         = 4,
    cmZoom          = 5,
    cmResize        = 6,
    cmNext          = 7,
    cmPrev          = 8,
    cmHelp          = 9,

//  TDialog standard commands

    cmOK            = 10,
    cmCancel        = 11,
    cmYes           = 12,
    cmNo            = 13,
    cmDefault       = 14,

//  TView State masks 

    sfVisible       = 0x001,
    sfCursorVis     = 0x002,
    sfCursorIns     = 0x004,
    sfShadow        = 0x008,
    sfActive        = 0x010,
    sfSelected      = 0x020,
    sfFocused       = 0x040,
    sfDragging      = 0x080,
    sfDisabled      = 0x100,
    sfModal         = 0x200,
    sfDefault       = 0x400,
    sfExposed       = 0x800,

// TView Option masks 

    ofSelectable    = 0x001,
    ofTopSelect     = 0x002,
    ofFirstClick    = 0x004,
    ofFramed        = 0x008,
    ofPreProcess    = 0x010,
    ofPostProcess   = 0x020,
    ofBuffered      = 0x040,
    ofTileable      = 0x080,
    ofCenterX       = 0x100,
    ofCenterY       = 0x200,
    ofCentered      = 0x300,

// TView GrowMode masks 

    gfGrowLoX       = 0x01,
    gfGrowLoY       = 0x02,
    gfGrowHiX       = 0x04,
    gfGrowHiY       = 0x08,
    gfGrowAll       = 0x0f,
    gfGrowRel       = 0x10,

// TView DragMode masks 

    dmDragMove      = 0x01,
    dmDragGrow      = 0x02,
    dmLimitLoX      = 0x10,
    dmLimitLoY      = 0x20,
    dmLimitHiX      = 0x40,
    dmLimitHiY      = 0x80,
    dmLimitAll      = dmLimitLoX | dmLimitLoY | dmLimitHiX | dmLimitHiY,

// TView Help context codes

    hcNoContext     = 0,
    hcDragging      = 1,

// TScrollBar part codes 

    sbLeftArrow     = 0,
    sbRightArrow    = 1,
    sbPageLeft      = 2,
    sbPageRight     = 3,
    sbUpArrow       = 4,
    sbDownArrow     = 5,
    sbPageUp        = 6,
    sbPageDown      = 7,
    sbIndicator     = 8,

// TScrollBar options for TWindow.StandardScrollBar 

    sbHorizontal    = 0x000,
    sbVertical      = 0x001,
    sbHandleKeyboard = 0x002,

// TWindow Flags masks

    wfMove          = 0x01,
    wfGrow          = 0x02,
    wfClose         = 0x04,
    wfZoom          = 0x08,

//  TView inhibit flags

    noMenuBar       = 0x0001,
    noDeskTop       = 0x0002,
    noStatusLine    = 0x0004,
    noBackground    = 0x0008,
    noFrame         = 0x0010,
    noViewer        = 0x0020,
    noHistory       = 0x0040,

// TWindow number constants

    wnNoNumber      = 0,

// TWindow palette entries 

    wpBlueWindow    = 0,
    wpCyanWindow    = 1,
    wpGrayWindow    = 2,

//  Application command codes

    cmCut           = 20,
    cmCopy          = 21,
    cmPaste         = 22,
    cmUndo          = 23,
    cmClear         = 24,
    cmTile          = 25,
    cmCascade       = 26,

// Standard messages 

    cmReceivedFocus     = 50,
    cmReleasedFocus     = 51,
    cmCommandSetChanged = 52,

// TScrollBar messages 

    cmScrollBarChanged  = 53,
    cmScrollBarClicked  = 54,

// TWindow select messages 

    cmSelectWindowNum   = 55,

//  TListViewer messages

    cmListItemSelected  = 56,

//  Event masks

    positionalEvents    = evMouse,
    focusedEvents       = evKeyboard | evCommand;

#endif  // __COMMAND_CODES

#if defined( Uses_TCommandSet ) && !defined( __TCommandSet )
#define __TCommandSet

class TCommandSet
{

public:

    TCommandSet();
    TCommandSet( const TCommandSet& );

    Boolean has( int cmd );

    void disableCmd( int cmd );
    void enableCmd( int cmd );
    void operator += ( int cmd );
    void operator -= ( int cmd );

    void disableCmd( const TCommandSet& );
    void enableCmd( const TCommandSet& );
    void operator += ( const TCommandSet& );
    void operator -= ( const TCommandSet& );

    Boolean TCommandSet::isEmpty();

    TCommandSet& operator &= ( const TCommandSet& );
    TCommandSet& operator |= ( const TCommandSet& );

    friend TCommandSet operator & ( const TCommandSet&, const TCommandSet& );
    friend TCommandSet operator | ( const TCommandSet&, const TCommandSet& );

    friend int operator == ( const TCommandSet& tc1, const TCommandSet& tc2 );
    friend int operator != ( const TCommandSet& tc1, const TCommandSet& tc2 );

private:

    int loc( int );
    int mask( int );

    static int near masks[8];

    uchar cmds[32];

};

inline void TCommandSet::operator += ( int cmd )
{
    enableCmd( cmd );
}

inline void TCommandSet::operator -= ( int cmd )
{
    disableCmd( cmd );
}

inline void TCommandSet::operator += ( const TCommandSet& tc )
{
    enableCmd( tc );
}

inline void TCommandSet::operator -= ( const TCommandSet& tc )
{
    disableCmd( tc );
}

inline int operator != ( const TCommandSet& tc1, const TCommandSet& tc2 )
{
    return !operator == ( tc1, tc2 );
}

inline int TCommandSet::loc( int cmd )
{
    return cmd / 8;
}

inline int TCommandSet::mask( int cmd )
{
    return masks[ cmd & 0x07 ];
}

#endif  // Uses_TCommandSet

#if defined( Uses_TPalette ) && !defined( __TPalette )
#define __TPalette

class TPalette
{

public:

    TPalette( const char *, ushort );
    TPalette( const TPalette& );
    ~TPalette();

    TPalette& operator = ( const TPalette& );

    char& operator[]( int ) const;

    char *data;

};

#endif  // Uses_TPalette

#if defined( Uses_TView ) && !defined( __TView )
#define __TView

struct write_args
{
    void far *self;
    void far *target;
    void far *buf;
    ushort offset;
};

class far TRect;
class far TEvent;
class far TGroup;

class TView : public TObject, public TStreamable
{

public:

    friend void genRefs();

    enum phaseType { phFocused, phPreProcess, phPostProcess };
    enum selectMode{ normalSelect, enterSelect, leaveSelect };

    TView( const TRect& bounds );
    ~TView();

    virtual void sizeLimits( TPoint& min, TPoint& max );
    TRect getBounds();
    TRect getExtent();
    TRect getClipRect();
    Boolean mouseInView( TPoint mouse );
    Boolean containsMouse( TEvent& event );

    void locate( TRect& bounds );
    virtual void dragView( TEvent& event, uchar mode,   //  temporary fix
      TRect& limits, TPoint minSize, TPoint maxSize ); //  for Miller's stuff
    virtual void calcBounds( TRect& bounds, TPoint delta );
    virtual void changeBounds( const TRect& bounds );
    void growTo( short x, short y );
    void moveTo( short x, short y );
    void setBounds( const TRect& bounds );

    virtual ushort getHelpCtx();

    virtual Boolean valid( ushort command );

    void hide();
    void show();
    virtual void draw();
    void drawView();
    Boolean exposed();
    void hideCursor();
    void drawHide( TView *lastView );
    void drawShow( TView *lastView );
    void drawUnderRect( TRect& r, TView *lastView );
    void drawUnderView( Boolean doShadow, TView *lastView );

    virtual ushort dataSize();
    virtual void getData( void *rec );
    virtual void setData( void *rec );

    void blockCursor();
    void normalCursor();
    virtual void resetCursor();
    void setCursor( short x, short y );
    void showCursor();
    void drawCursor();

    void clearEvent( TEvent& event );
    Boolean eventAvail();
    virtual void getEvent( TEvent& event );
    virtual void handleEvent( TEvent& event );
    virtual void putEvent( TEvent& event );

    static Boolean commandEnabled( ushort command );
    static void disableCommands( TCommandSet& commands );
    static void enableCommands( TCommandSet& commands );
    static void disableCommand( ushort command );
    static void enableCommand( ushort command );
    static void getCommands( TCommandSet& commands );
    static void setCommands( TCommandSet& commands );

    virtual void endModal( ushort command );
    virtual ushort execute();

    ushort getColor( ushort color );
    virtual TPalette& getPalette() const;
    uchar mapColor( uchar );

    Boolean getState( ushort aState );
    void select();
    virtual void setState( ushort aState, Boolean enable );

    void keyEvent( TEvent& event );
    Boolean mouseEvent( TEvent& event, ushort mask );


    TPoint makeGlobal( TPoint source );
    TPoint makeLocal( TPoint source );

    TView *nextView();
    TView *prevView();
    TView *prev();
    TView *next;

    void makeFirst();
    void putInFrontOf( TView *Target );
    TView *TopView();

    void writeBuf(  short x, short y, short w, short h, const void far *b );
    void writeBuf(  short x, short y, short w, short h, const TDrawBuffer& b );
    void writeChar( short x, short y, char c, uchar color, short count );
    void writeLine( short x, short y, short w, short h, const TDrawBuffer& b );
    void writeLine( short x, short y, short w, short h, const void far *b );
    void writeStr( short x, short y, const char *str, uchar color );

    TPoint size;
    ushort options;
    ushort eventMask;
    ushort state;
    TPoint origin;
    TPoint cursor;
    uchar growMode;
    uchar dragMode;
    ushort helpCtx;
    static Boolean near commandSetChanged;
    static TCommandSet near curCommandSet;
    TGroup *owner;

    static Boolean near showMarkers;
    static uchar near errorAttr;

    virtual void shutDown();

private:

    void moveGrow( TPoint p,
                   TPoint s,
                   TRect& limits,
                   TPoint minSize,
                   TPoint maxSize,
                   uchar mode
                 );
    void change( uchar, TPoint delta, TPoint& p, TPoint& s );
    static void writeView( write_args );

    virtual const char *streamableName() const
        { return name; }

protected:

    TView( StreamableInit );

public:

    static const char * const near name;
    static TStreamable *build();

protected:

    virtual void write( opstream& );
    virtual void *read( ipstream& );

};

inline ipstream& operator >> ( ipstream& is, TView& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TView*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TView& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TView* cl )
    { return os << (TStreamable *)cl; }

inline void TView::writeBuf( short x, short y, short w, short h,
                             const TDrawBuffer& b )
{
    writeBuf( x, y, w, h, b.data );
}

inline void TView::writeLine( short x, short y, short w, short h,
                              const TDrawBuffer& b )
{
    writeLine( x, y, w, h, b.data );
}

#endif  // Uses_TView

/* ---------------------------------------------------------------------- */
/*      class TFrame                                                      */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Passive frame                                               */
/*        2 = Passive title                                               */
/*        3 = Active frame                                                */
/*        4 = Active title                                                */
/*        5 = Icons                                                       */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TFrame ) && !defined( __TFrame )
#define __TFrame

class far TRect;
class far TEvent;
class far TDrawBuffer;

class TFrame : public TView
{

public:

    TFrame( const TRect& bounds );

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& event );
    virtual void setState( ushort aState, Boolean enable );

private:

    void frameLine( TDrawBuffer& frameBuf, short y, short n, uchar color );
    void dragWindow( TEvent& event, uchar dragMode );

    friend class TDisplay;
    static const char near initFrame[19];
    static char near frameChars[33];
    static const char * near closeIcon;
    static const char * near zoomIcon;
    static const char * near unZoomIcon;
    static const char * near dragIcon;

    virtual const char *streamableName() const
        { return name; }

protected:

    TFrame( StreamableInit );

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TFrame& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TFrame*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TFrame& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TFrame* cl )
    { return os << (TStreamable *)cl; }

#endif  // Uses_TFrame

/* ---------------------------------------------------------------------- */
/*      class TScrollBar                                                  */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Page areas                                                  */
/*        2 = Arrows                                                      */
/*        3 = Indicator                                                   */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TScrollBar ) && !defined( __TScrollBar )
#define __TScrollBar

class far TRect;
class far TEvent;

typedef char TScrollChars[5];

class TScrollBar : public TView
{

public:

    TScrollBar( const TRect& bounds );

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& event );
    virtual void scrollDraw();
    virtual short scrollStep( short part );
    void setParams( short aValue, short aMin, short aMax, 
                    short aPgStep, short aArStep );
    void setRange( short aMin, short aMax );
    void setStep( short aPgStep, short aArStep );
    void setValue( short aValue );

    void drawPos( short pos );
    short getPos();
    short getSize();

    short value;

    TScrollChars chars;
    short minVal;
    short maxVal;
    short pgStep;
    short arStep;

private:

    short getPartCode(void);

    static TScrollChars near vChars;
    static TScrollChars near hChars;

    virtual const char *streamableName() const
        { return name; }

protected:

    TScrollBar( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TScrollBar& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TScrollBar*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TScrollBar& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TScrollBar* cl )
    { return os << (TStreamable *)cl; }

#endif  // Uses_TScrollBar

/* ---------------------------------------------------------------------- */
/*      class TScroller                                                   */
/*                                                                        */
/*      Palette layout                                                    */
/*      1 = Normal text                                                   */
/*      2 = Selected text                                                 */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TScroller ) && !defined( __TScroller )
#define __TScroller

class far TRect;
class far TScrollBar;
class far TEvent;

class TScroller : public TView
{

public:

    TScroller( const TRect& bounds,
               TScrollBar *aHScrollBar,
               TScrollBar *aVScrollBar
             );

    virtual void changeBounds( const TRect& bounds );
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& event );
    virtual void scrollDraw();
    void scrollTo( short x, short y );
    void setLimit( short x, short y );
    virtual void setState( ushort aState, Boolean enable );
    void checkDraw();
    virtual void shutDown();

protected:

    uchar drawLock;
    Boolean drawFlag;
    TScrollBar *hScrollBar;
    TScrollBar *vScrollBar;
    TPoint delta;
    TPoint limit;

private:

    void showSBar( TScrollBar *sBar );

    virtual const char *streamableName() const
        { return name; }

protected:

    TScroller( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TScroller& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TScroller*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TScroller& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TScroller* cl )
    { return os << (TStreamable *)cl; }

#endif  // Uses_TScroller

#if defined( Uses_TListViewer ) && !defined( __TListViewer )
#define __TListViewer

class far TRect;
class far TScrollBar;
class far TEvent;

class TListViewer : public TView
{

public:

    TListViewer( const TRect& bounds,
                 ushort aNumCols,
                 TScrollBar *aHScrollBar,
                 TScrollBar *aVScrollBar
               );

    virtual void changeBounds( const TRect& bounds );
    virtual void draw();
    virtual void focusItem( short item );
    virtual TPalette& getPalette() const;
    virtual void getText( char *dest, short item, short maxLen );
    virtual Boolean isSelected( short item );
    virtual void handleEvent( TEvent& event );
    virtual void selectItem( short item );
    void setRange( short aRange );
    virtual void setState( ushort aState, Boolean enable );

    virtual void focusItemNum( short item );
    virtual void shutDown();

    TScrollBar *hScrollBar;
    TScrollBar *vScrollBar;
    short numCols;
    short topItem;
    short focused;
    short range;

private:

    virtual const char *streamableName() const
        { return name; }

protected:

    TListViewer( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TListViewer& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TListViewer*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TListViewer& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TListViewer* cl )
    { return os << (TStreamable *)cl; }

#endif  // Uses_TListViewer

#if defined( Uses_TGroup ) && !defined( __TGroup )
#define __TGroup

class far TView;

class TGroup : public TView
{

public:

    friend void genRefs();

    TGroup( const TRect& bounds );
    ~TGroup();

    virtual void shutDown();

    ushort execView( TView *p );
    virtual ushort execute();

    void insertView( TView *p, TView *Target );
    void remove( TView *p );
    void removeView( TView *p );
    void resetCurrent();
    void setCurrent( TView *p, selectMode mode );
    void selectNext( Boolean forwards );
    TView *firstThat( Boolean (*func)( TView *, void * ), void *args );
    void forEach( void (*func)( TView *, void * ), void *args );
    void insert( TView *p );
    void insertBefore( TView *p, TView *Target );
    TView *current;
    TView *at( short index );
    TView *firstMatch( ushort aState, ushort aOptions );
    short indexOf( TView *p );
    Boolean matches( TView *p );
    TView *first();

    virtual void setState( ushort aState, Boolean enable );

    virtual void handleEvent( TEvent& event );

    void drawSubViews( TView *p, TView *bottom );

    virtual void changeBounds( const TRect& bounds );

    virtual ushort dataSize();
    virtual void getData( void *rec );
    virtual void setData( void *rec );

    virtual void draw();
    void redraw();
    void lock();
    void unlock();
    virtual void resetCursor();

    virtual void endModal( ushort command );

    virtual void eventError( TEvent& event );

    virtual ushort getHelpCtx();

    virtual Boolean valid( ushort command );

    void freeBuffer();
    void getBuffer();

    TView *last;

    TRect clip;
    phaseType phase;

    ushort far *buffer;
    uchar lockFlag;
    ushort endState;

private:

    Boolean invalid( TView *p, ushort command );
    void focusView( TView *p, Boolean enable );
    void selectView( TView *p, Boolean enable );

    virtual const char *streamableName() const
        { return name; }

protected:

    TGroup( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TGroup& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TGroup*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TGroup& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TGroup* cl )
    { return os << (TStreamable *)cl; }

#endif  // Uses_TGroup

#if defined( Uses_TWindow ) && !defined( __TWindow )
#define __TWindow

class far TFrame;
class far TRect;
class far TPoint;
class far TEvent;
class far TFrame;
class far TScrollBar;

class TWindowInit
{

public:

    TWindowInit( TFrame *(*cFrame)( TRect ) );

protected:

    TFrame *(*createFrame)( TRect );

};

/* ---------------------------------------------------------------------- */
/*      class TWindow                                                     */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Frame passive                                               */
/*        2 = Frame active                                                */
/*        3 = Frame icon                                                  */
/*        4 = ScrollBar page area                                         */
/*        5 = ScrollBar controls                                          */
/*        6 = Scroller normal text                                        */
/*        7 = Scroller selected text                                      */
/*        8 = Reserved                                                    */
/* ---------------------------------------------------------------------- */

class TWindow: public TGroup, public virtual TWindowInit
{

public:

    TWindow( const TRect& bounds,
             const char *aTitle,
             short aNumber
           );
    ~TWindow();

    virtual void close();
    virtual TPalette& getPalette() const;
    virtual const char *getTitle( short maxSize );
    virtual void handleEvent( TEvent& event );
    static TFrame *initFrame( TRect );
    virtual void setState( ushort aState, Boolean enable );
    virtual void sizeLimits( TPoint& min, TPoint& max );
    TScrollBar *standardScrollBar( ushort aOptions );
    virtual void zoom();
    virtual void shutDown();

    uchar flags;
    TRect zoomRect;
    short number;
    short palette;
    TFrame *frame;
    const char *title;

private:

    virtual const char *streamableName() const
        { return name; }            

protected:

    TWindow( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TWindow& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TWindow*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TWindow& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TWindow* cl )
    { return os << (TStreamable *)cl; }

#endif  // Uses_TWindow

#pragma option -Vo.
#if defined( __BCOPT__ )
#pragma option -po.
#endif


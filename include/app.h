/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   APP.H                                                                 */
/*                                                                         */
/*   Copyright (c) Borland International 1991                              */
/*   All Rights Reserved.                                                  */
/*                                                                         */
/*   defines the classes TBackground, TDeskTop, TProgram, and TApplication */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#pragma option -Vo-
#if defined( __BCOPT__ )
#pragma option -po-
#endif

#if defined( Uses_TBackground ) && !defined( __TBackground )
#define __TBackground
    
class far TRect;

class TBackground : public TView
{

public:

    TBackground( const TRect& bounds, char aPattern );
    virtual void draw();
    virtual TPalette& getPalette() const;

protected:

    char pattern;

private:

    virtual const char *streamableName() const
        { return name; }

protected:

    TBackground( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TBackground& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TBackground*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TBackground& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TBackground* cl )
    { return os << (TStreamable *)cl; }

#endif  // Uses_TBackground


#if defined( Uses_TDeskTop )  && !defined( __TDeskTop )
#define __TDeskTop

class far TBackground;
class far TRect;
class far TEvent;

class TDeskInit
{

public:

    TDeskInit( TBackground *(*cBackground)( TRect ) );

protected:

    TBackground *(*createBackground)( TRect );

};

class TDeskTop : public TGroup, public virtual TDeskInit
{

public:

    TDeskTop( const TRect& );

    void cascade( const TRect& );
    virtual void handleEvent( TEvent& );
    static TBackground *initBackground( TRect );
    void tile( const TRect& );
    virtual void tileError();
    virtual void shutDown();

protected:

    TBackground *background;

private:

    static const char near defaultBkgrnd;
    
    virtual const char *streamableName() const
        { return name; }

protected:

    TDeskTop( StreamableInit );

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TDeskTop& cl )
    { return is >> (TStreamable&)(TGroup&)cl; }
inline ipstream& operator >> ( ipstream& is, TDeskTop*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TDeskTop& cl )
    { return os << (TStreamable&)(TGroup&)cl; }
inline opstream& operator << ( opstream& os, TDeskTop* cl )
    { return os << (TStreamable *)(TGroup *)cl; }

#endif


#if defined( Uses_TProgram ) && !defined( __TProgram )
#define __TProgram

#define cpColor \
    "\x71\x70\x78\x74\x20\x28\x24\x17\x1F\x1A\x31\x31\x1E\x71\x00" \
    "\x37\x3F\x3A\x13\x13\x3E\x21\x00\x70\x7F\x7A\x13\x13\x70\x7F\x00" \
    "\x70\x7F\x7A\x13\x13\x70\x70\x7F\x7E\x20\x2B\x2F\x78\x2E\x70\x30" \
    "\x3F\x3E\x1F\x2F\x1A\x20\x72\x31\x31\x30\x2F\x3E\x31\x13\x00\x00"

#define cpBlackWhite \
    "\x70\x70\x78\x7F\x07\x07\x0F\x07\x0F\x07\x70\x70\x07\x70\x00" \
    "\x07\x0F\x07\x70\x70\x07\x70\x00\x70\x7F\x7F\x70\x07\x70\x07\x00" \
    "\x70\x7F\x7F\x70\x07\x70\x70\x7F\x7F\x07\x0F\x0F\x78\x0F\x78\x07" \
    "\x0F\x0F\x0F\x70\x0F\x07\x70\x70\x70\x07\x70\x0F\x07\x07\x00\x00"

#define cpMonochrome \
    "\x70\x07\x07\x0F\x70\x70\x70\x07\x0F\x07\x70\x70\x07\x70\x00" \
    "\x07\x0F\x07\x70\x70\x07\x70\x00\x70\x70\x70\x07\x07\x70\x07\x00" \
    "\x70\x70\x70\x07\x07\x70\x70\x70\x0F\x07\x07\x0F\x70\x0F\x70\x07" \
    "\x0F\x0F\x07\x70\x07\x07\x70\x07\x07\x07\x70\x0F\x07\x07\x00\x00"


class far TStatusLine;
class far TMenuBar;
class far TDeskTop;
class far TEvent;
class far TView;

class TProgInit
{

public:

    TProgInit( TStatusLine *(*cStatusLine)( TRect ),
               TMenuBar *(*cMenuBar)( TRect ),
               TDeskTop *(*cDeskTop )( TRect )
             );

protected:

    TStatusLine *(*createStatusLine)( TRect );
    TMenuBar *(*createMenuBar)( TRect );
    TDeskTop *(*createDeskTop)( TRect );

};

/* ---------------------------------------------------------------------- */
/*      class TProgram                                                    */
/*                                                                        */
/*      Palette layout                                                    */
/*          1 = TBackground                                               */
/*       2- 7 = TMenuView and TStatusLine                                 */
/*       8-15 = TWindow(Blue)                                             */
/*      16-23 = TWindow(Cyan)                                             */
/*      24-31 = TWindow(Gray)                                             */
/*      32-63 = TDialog                                                   */
/* ---------------------------------------------------------------------- */

const

//  TApplication palette entries

    apColor      = 0,
    apBlackWhite = 1,
    apMonochrome = 2;

class TProgram : public TGroup, public virtual TProgInit
{

public:

    TProgram();
    virtual ~TProgram();

    virtual void getEvent(TEvent& event);
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    virtual void idle();
    virtual void initScreen();
    virtual void outOfMemory();
    virtual void putEvent( TEvent& event );
    virtual void run();
    void setScreenMode( ushort mode );
    TView *validView( TView *p );
    virtual void shutDown();

    virtual void suspend() {}
    virtual void resume() {}

    static TStatusLine *initStatusLine( TRect );
    static TMenuBar *initMenuBar( TRect );
    static TDeskTop *initDeskTop( TRect );

    static TProgram * near application;
    static TStatusLine * near statusLine;
    static TMenuBar * near menuBar;
    static TDeskTop * near deskTop;
    static int near appPalette;

protected:

    static TEvent near pending;

private:

    static const char * near exitText;

};

#endif

#if defined( Uses_TApplication ) && !defined( __TApplication )
#define __TApplication

class TApplication : public TProgram
{

protected:

    TApplication();
    virtual ~TApplication();

    virtual void suspend();
    virtual void resume();

};

#endif

#pragma option -Vo.
#if defined( __BCOPT__ )
#pragma option -po.
#endif


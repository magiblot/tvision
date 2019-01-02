/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   DIALOGS.H                                                             */
/*                                                                         */
/*   Copyright (c) Borland International 1991                              */
/*   All Rights Reserved.                                                  */
/*                                                                         */
/*   defines the classes TDialog, TInputLine, TButton, TCluster,           */
/*   TRadioButtons, TCheckBoxes, TStaticText, TParamText, TLabel,          */
/*   THistoryViewer, and THistoryWindow.                                   */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#pragma option -Vo-
#if defined( __BCOPT__ )
#pragma option -po-
#endif

#if !defined( __BUTTON_TYPE )
#define __BUTTON_TYPE

const
    bfNormal    = 0x00,
    bfDefault   = 0x01,
    bfLeftJust  = 0x02,
    bfBroadcast = 0x04,

    cmRecordHistory = 60;

#endif  // __BUTTON_TYPE

/* ---------------------------------------------------------------------- */
/*      class TDialog                                                     */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Frame passive                                               */
/*        2 = Frame active                                                */
/*        3 = Frame icon                                                  */
/*        4 = ScrollBar page area                                         */
/*        5 = ScrollBar controls                                          */
/*        6 = StaticText                                                  */
/*        7 = Label normal                                                */
/*        8 = Label selected                                              */
/*        9 = Label shortcut                                              */
/*       10 = Button normal                                               */
/*       11 = Button default                                              */
/*       12 = Button selected                                             */
/*       13 = Button disabled                                             */
/*       14 = Button shortcut                                             */
/*       15 = Button shadow                                               */
/*       16 = Cluster normal                                              */
/*       17 = Cluster selected                                            */
/*       18 = Cluster shortcut                                            */
/*       19 = InputLine normal text                                       */
/*       20 = InputLine selected text                                     */
/*       21 = InputLine arrows                                            */
/*       22 = History arrow                                               */
/*       23 = History sides                                               */
/*       24 = HistoryWindow scrollbar page area                           */
/*       25 = HistoryWindow scrollbar controls                            */
/*       26 = ListViewer normal                                           */
/*       27 = ListViewer focused                                          */
/*       28 = ListViewer selected                                         */
/*       29 = ListViewer divider                                          */
/*       30 = InfoPane                                                    */
/*       31 = Reserved                                                    */
/*       32 = Reserved                                                    */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TDialog ) && !defined( __TDialog )
#define __TDialog

class far TRect;
class far TEvent;

class TDialog : public TWindow
{

public:

    TDialog( const TRect& bounds, const char *aTitle );

    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& event );
    virtual Boolean valid( ushort command );

private:

    virtual const char *streamableName() const
        { return name; }

protected:

    TDialog( StreamableInit );

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TDialog& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TDialog*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TDialog& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TDialog* cl )
    { return os << (TStreamable *)cl; }

#endif  // Uses_TDialog

/* ---------------------------------------------------------------------- */
/*      class TInputLine                                                  */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Passive                                                     */
/*        2 = Active                                                      */
/*        3 = Selected                                                    */
/*        4 = Arrows                                                      */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TInputLine ) && !defined( __TInputLine )
#define __TInputLine

class far TRect;
class far TEvent;

class TInputLine : public TView
{

public:

    TInputLine( const TRect& bounds, int aMaxLen );
    ~TInputLine();

    virtual ushort dataSize();
    virtual void draw();
    virtual void getData( void *rec );
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& event );
    void selectAll( Boolean enable );
    virtual void setData( void *rec );
    virtual void setState( ushort aState, Boolean enable );

    char* data;
    int maxLen;
    int curPos;
    int firstPos;
    int selStart;
    int selEnd;

private:

    Boolean canScroll( int delta );
    int mouseDelta( TEvent& event );
    int mousePos( TEvent& event );
    void deleteSelect();

    static const char near rightArrow;
    static const char near leftArrow;
    
    virtual const char *streamableName() const
        { return name; }

protected:

    TInputLine( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TInputLine& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TInputLine*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TInputLine& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TInputLine* cl )
    { return os << (TStreamable *)cl; }

#endif  // Uses_TInputLine


/* ---------------------------------------------------------------------- */
/*      TButton object                                                    */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Default text                                                */
/*        3 = Selected text                                               */
/*        4 = Disabled text                                               */
/*        5 = Normal shortcut                                             */
/*        6 = Default shortcut                                            */
/*        7 = Selected shortcut                                           */
/*        8 = Shadow                                                      */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TButton ) && !defined( __TButton )
#define __TButton

class far TRect;
class far TEvent;
class far TDrawBuffer;

class TButton : public TView
{

public:

    TButton( const TRect& bounds,
             const char *aTitle,
             ushort aCommand,
             ushort aFlags
           );
    ~TButton();

    virtual void draw();
    void drawState( Boolean down );
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& event );
    void makeDefault( Boolean enable );
    virtual void press();
    virtual void setState( ushort aState, Boolean enable );

    const char *title;

protected:

    ushort command;
    uchar flags;
    Boolean amDefault;

private:

    void drawTitle( TDrawBuffer&, int, int, ushort, Boolean );
    void pressButton( TEvent& );
    TRect getActiveRect();

    static const char * near shadows;
    static const char * near markers;
    
    virtual const char *streamableName() const
        { return name; }

protected:

    TButton( StreamableInit ): TView( streamableInit ) {};
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TButton& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TButton*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TButton& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TButton* cl )
    { return os << (TStreamable *)cl; }

#endif  // Uses_TButton


#if defined( Uses_TSItem ) && !defined( __TSItem )
#define __TSItem

class TSItem
{

public:

    TSItem( const char *aValue, TSItem *aNext )
        { value = newStr(aValue); next = aNext; }
    ~TSItem() { delete (void *)value; }

    const char *value;
    TSItem *next;
};

#endif  // Uses_TSItem

/* ---------------------------------------------------------------------- */
/*      class TCluster                                                    */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Selected text                                               */
/*        3 = Normal shortcut                                             */
/*        4 = Selected shortcut                                           */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TCluster ) && !defined( __TCluster )
#define __TCluster

class far TRect;
class far TSItem;
class far TEvent;
class far TPoint;
class far TStringCollection;

class TCluster : public TView
{

public:

    TCluster( const TRect& bounds, TSItem *aStrings );
    ~TCluster();

    virtual ushort dataSize();
    void drawBox( const char *icon, char marker );
    virtual void getData( void *rec );
    ushort getHelpCtx();
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& event );
    virtual Boolean mark( int item );
    virtual void press( int item );
    virtual void movedTo( int item );
    virtual void setData( void *rec );
    virtual void setState( ushort aState, Boolean enable );

protected:

    ushort value;
    int sel;
    TStringCollection *strings;

private:

    int column( int item );
    int findSel( TPoint p );
    int row( int item );

    virtual const char *streamableName() const
        { return name; }

protected:

    TCluster( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TCluster& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TCluster*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TCluster& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TCluster* cl )
    { return os << (TStreamable *)cl; }

#endif  // Uses_TCluster


/* ---------------------------------------------------------------------- */
/*      class TRadioButtons                                               */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Selected text                                               */
/*        3 = Normal shortcut                                             */
/*        4 = Selected shortcut                                           */
/* ---------------------------------------------------------------------- */


#if defined( Uses_TRadioButtons ) && !defined( __TRadioButtons )
#define __TRadioButtons

class far TRect;
class far TSItem;

class TRadioButtons : public TCluster
{

public:

    TRadioButtons( const TRect& bounds, TSItem *aStrings );

    virtual void draw();
    virtual Boolean mark( int item );
    virtual void movedTo( int item );
    virtual void press( int item );
    virtual void setData( void *rec );

private:

    static const char * near button;
    virtual const char *streamableName() const
        { return name; }

protected:

    TRadioButtons( StreamableInit );

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TRadioButtons& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TRadioButtons*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TRadioButtons& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TRadioButtons* cl )
    { return os << (TStreamable *)cl; }

inline TRadioButtons::TRadioButtons( const TRect& bounds, TSItem *aStrings ) :
    TCluster( bounds, aStrings )
{
}

#endif  // Uses_TRadioButtons


/* ---------------------------------------------------------------------- */
/*      TCheckBoxes                                                       */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Selected text                                               */
/*        3 = Normal shortcut                                             */
/*        4 = Selected shortcut                                           */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TCheckBoxes ) && !defined( __TCheckBoxes )
#define __TCheckBoxes

class far TRect;
class far TSItem;

class TCheckBoxes : public TCluster
{

public:

    TCheckBoxes( const TRect& bounds, TSItem *aStrings);

    virtual void draw();
    
    virtual Boolean mark( int item );
    virtual void press( int item );

private:

    static const char * near button;

    virtual const char *streamableName() const
        { return name; }

protected:

    TCheckBoxes( StreamableInit );

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TCheckBoxes& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TCheckBoxes*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TCheckBoxes& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TCheckBoxes* cl )
    { return os << (TStreamable *)cl; }

inline TCheckBoxes::TCheckBoxes( const TRect& bounds, TSItem *aStrings) :
    TCluster( bounds, aStrings )
{
}

#endif  // Uses_TCheckBoxes


#if defined( Uses_TListBox ) && !defined( __TListBox )
#define __TListBox

class far TRect;
class far TScrollBar;
class far TCollection;

class TListBox : public TListViewer
{

public:

    TListBox( const TRect& bounds, ushort aNumCols, TScrollBar *aScrollBar );
    ~TListBox();

    virtual ushort dataSize();
    virtual void getData( void *rec );
    virtual void getText( char *dest, short item, short maxLen );
    virtual void newList( TCollection *aList );
    virtual void setData( void *rec );

    TCollection *list();

private:

    TCollection *items;
    
    virtual const char *streamableName() const
        { return name; }

protected:

    TListBox( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TListBox& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TListBox*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TListBox& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TListBox* cl )
    { return os << (TStreamable *)cl; }

inline TCollection *TListBox::list()
{
    return items;
}

#endif  // Uses_TListBox


/* ---------------------------------------------------------------------- */
/*      class TStaticText                                                 */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Text                                                        */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TStaticText ) && !defined( __TStaticText )
#define __TStaticText

class far TRect;

class TStaticText : public TView
{

public:

    TStaticText( const TRect& bounds, const char *aText );
    ~TStaticText();

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void getText( char * );

protected:

    const char *text;

private:

    virtual const char *streamableName() const
        { return name; }

protected:

    TStaticText( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TStaticText& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TStaticText*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TStaticText& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TStaticText* cl )
    { return os << (TStreamable *)cl; }

#endif  // Uses_TStaticText


/* ---------------------------------------------------------------------- */
/*      class TParamText                                                  */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Text                                                        */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TParamText ) && !defined( __TParamText )
#define __TParamText

class far TRect;

class TParamText : public TStaticText
{

public:
    TParamText( const TRect& bounds, const char *aText, int aParamCount );

    virtual ushort dataSize();
    virtual void getText( char * );
    virtual void setData( void *rec );

protected:

    short paramCount;
    void *paramList;

private:

    virtual const char *streamableName() const
        { return name; }

protected:

    TParamText( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TParamText& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TParamText*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TParamText& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TParamText* cl )
    { return os << (TStreamable *)cl; }

#endif  // Uses_TParamText


/* ---------------------------------------------------------------------- */
/*      class TLabel                                                      */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Selected text                                               */
/*        3 = Normal shortcut                                             */
/*        4 = Selected shortcut                                           */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TLabel ) && !defined( __TLabel )
#define __TLabel

class far TRect;
class far TEvent;
class far TView;

class TLabel : public TStaticText
{

public:

    TLabel( const TRect& bounds, const char *aText, TView *aLink );

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& event );
    virtual void shutDown();

protected:

    TView *link;
    Boolean light;

private:

    virtual const char *streamableName() const
        { return name; }

protected:

    TLabel( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TLabel& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TLabel*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TLabel& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TLabel* cl )
    { return os << (TStreamable *)cl; }

#endif  // Uses_TLabel


/* ---------------------------------------------------------------------- */
/*      class THistoryViewer                                              */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Active                                                      */
/*        2 = Inactive                                                    */
/*        3 = Focused                                                     */
/*        4 = Selected                                                    */
/*        5 = Divider                                                     */
/* ---------------------------------------------------------------------- */

#if defined( Uses_THistoryViewer ) && !defined( __THistoryViewer )
#define __THistoryViewer

class far TRect;
class far TScrollBar;

class THistoryViewer : public TListViewer
{

public:

    THistoryViewer( const TRect& bounds,
                    TScrollBar *aHScrollBar,
                    TScrollBar *aVScrollBar,
                    ushort aHistoryId
                  );

    virtual TPalette& getPalette() const;
    virtual void getText( char *dest, short item, short maxLen );
    virtual void handleEvent( TEvent& event );
    int historyWidth();

protected:

    ushort historyId;

};

#endif  // Uses_THistoryViewer

#if defined( Uses_THistoryWindow ) && !defined( __THistoryWindow )
#define __THistoryWindow

class far TListViewer;
class far TRect;
class far TWindow;
class far TInputLine;

class THistInit
{

public:

    THistInit( TListViewer *(*cListViewer)( TRect, TWindow *, ushort ) );

protected:

    TListViewer *(*createListViewer)( TRect, TWindow *, ushort );

};

/* ---------------------------------------------------------------------- */
/*      THistoryWindow                                                    */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Frame passive                                               */
/*        2 = Frame active                                                */
/*        3 = Frame icon                                                  */
/*        4 = ScrollBar page area                                         */
/*        5 = ScrollBar controls                                          */
/*        6 = HistoryViewer normal text                                   */
/*        7 = HistoryViewer selected text                                 */
/* ---------------------------------------------------------------------- */

class THistoryWindow : public TWindow, public virtual THistInit
{

public:

    THistoryWindow( const TRect& bounds, ushort historyId );

    virtual TPalette& getPalette() const;
    virtual void getSelection( char *dest );
    static TListViewer *initViewer( TRect, TWindow *, ushort );

protected:

    TListViewer *viewer;
};

#endif  // Uses_THistoryWindow

#if defined( Uses_THistory ) && !defined( __THistory )
#define __THistory

class far TRect;
class far TInputLine;
class far TEvent;
class far THistoryWindow;

class THistory : public TView
{

public:

    THistory( const TRect& bounds, TInputLine *aLink, ushort aHistoryId );

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& event );
    virtual THistoryWindow *initHistoryWindow( const TRect& bounds );
    virtual void shutDown();
protected:

    TInputLine *link;
    ushort historyId;

private:

    static const char * near icon;

    virtual const char *streamableName() const
        { return name; }

protected:

    THistory( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, THistory& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, THistory*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, THistory& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, THistory* cl )
    { return os << (TStreamable *)cl; }

#endif  // Uses_THistory

#pragma option -Vo.
#if defined( __BCOPT__ )
#pragma option -po.
#endif


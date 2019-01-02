/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   MENUS.H                                                               */
/*                                                                         */
/*   Copyright (c) Borland International 1991                              */
/*   All Rights Reserved.                                                  */
/*                                                                         */
/*   defines the classes TMenuItem, TMenu, TMenuView, TSubMenu,            */
/*   TMenuBar, TMenuBox, TStatusItem, TStatusDef, and TStatusLine          */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#pragma option -Vo-
#if defined( __BCOPT__ )
#pragma option -po-
#endif

class far TSubMenu;
class far TMenuItem;
class far TStatusDef;
class far TStatusItem;

TSubMenu& operator + ( TSubMenu& s, TMenuItem& i );
TSubMenu& operator + ( TSubMenu& s1, TSubMenu& s2 );
TStatusDef& operator + ( TStatusDef& s1, TStatusItem& s2 );
TStatusDef& operator + ( TStatusDef& s1, TStatusDef& s2 );

#if defined( Uses_TMenuItem ) && !defined( __TMenuItem )
#define __TMenuItem

class far TMenu;

class TMenuItem
{

public:

    TMenuItem( const char *aName,
               ushort aCommand,
               ushort aKeyCode,
               ushort aHelpCtx = hcNoContext,
               char *p = 0,
               TMenuItem *aNext = 0
             );
    TMenuItem( const char *aName,
               ushort aKeyCode,
               TMenu *aSubMenu,
               ushort aHelpCtx = hcNoContext,
               TMenuItem *aNext = 0
             );

    ~TMenuItem();

    void append( TMenuItem *aNext );

    TMenuItem *next;
    const char *name;
    ushort command;
    Boolean disabled;
    ushort keyCode;
    ushort helpCtx;
    union
        {
        const char *param;
        TMenu *subMenu;
        };
};

inline void TMenuItem::append( TMenuItem *aNext )
{
    next = aNext;
}

inline TMenuItem &newLine()
{
    return *new TMenuItem( 0, 0, 0, hcNoContext, 0, 0 );
}

#endif  // Uses_TMenuItem

#if defined( Uses_TSubMenu ) && !defined( __TSubMenu )
#define __TSubMenu

class TSubMenu : public TMenuItem
{

public:

    TSubMenu( const char *, ushort, ushort = hcNoContext );

};

#endif  // Uses_TSubMenu

#if defined( Uses_TMenu ) && !defined( __TMenu )
#define __TMenu

class TMenu
{

public:

    TMenu() : items(0), deflt(0) {};
    TMenu( TMenuItem& itemList )
        { items = &itemList; deflt = &itemList; }
    TMenu( TMenuItem& itemList, TMenuItem& TheDefault )
        { items = &itemList; deflt = &TheDefault; }
    ~TMenu();

    TMenuItem *items;
    TMenuItem *deflt;

};

#endif  // Uses_TMenu

/* ---------------------------------------------------------------------- */
/*      class TMenuView                                                   */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Disabled text                                               */
/*        3 = Shortcut text                                               */
/*        4 = Normal selection                                            */
/*        5 = Disabled selection                                          */
/*        6 = Shortcut selection                                          */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TMenuView ) && !defined( __TMenuView )
#define __TMenuView

class far TRect;
class far TMenu;
class far TEvent;

class TMenuView : public TView
{

public:

    TMenuView( const TRect& bounds, TMenu *aMenu, TMenuView *aParent );
    TMenuView( const TRect& bounds );

    void setBounds( const TRect& bounds );
    virtual ushort execute();
    TMenuItem *findItem( char ch );
    virtual TRect getItemRect( TMenuItem *item );
    virtual ushort getHelpCtx();
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& event );
    TMenuItem *hotKey( ushort keyCode );
    TMenuView *newSubView( const TRect& bounds,
                           TMenu *aMenu,
                           TMenuView *aParentMenu
                         );

protected:

    TMenuView *parentMenu;
    TMenu *menu;
    TMenuItem *current;

private:

    void nextItem();
    void prevItem();
    void trackKey( Boolean findNext );
    Boolean mouseInOwner( TEvent& e );
    Boolean mouseInMenus( TEvent& e );
    void trackMouse( TEvent& e );
    TMenuView *topMenu();
    Boolean updateMenu( TMenu *menu );
    void do_a_select( TEvent& );
    TMenuItem *findHotKey( TMenuItem *p, ushort keyCode );

private:

    virtual const char *streamableName() const
        { return name; }
    static void writeMenu( opstream&, TMenu * );
    static TMenu *readMenu( ipstream& );

protected:

    TMenuView( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TMenuView& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TMenuView*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TMenuView& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TMenuView* cl )
    { return os << (TStreamable *)cl; }

inline TMenuView::TMenuView( const TRect& bounds,
                             TMenu *aMenu,
                             TMenuView *aParent
                           ) :
    TView(bounds), current( 0 ), menu( aMenu ), parentMenu( aParent )
{
     eventMask |= evBroadcast;
}

inline TMenuView::TMenuView( const TRect& bounds ) :
    TView(bounds), parentMenu(0), menu(0), current(0)
{
     eventMask |= evBroadcast;
}

#endif  // Uses_TMenuView

/* ---------------------------------------------------------------------- */
/*      class TMenuBar                                                    */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Disabled text                                               */
/*        3 = Shortcut text                                               */
/*        4 = Normal selection                                            */
/*        5 = Disabled selection                                          */
/*        6 = Shortcut selection                                          */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TMenuBar ) && !defined( __TMenuBar )
#define __TMenuBar

class far TRect;
class far TMenu;

class TMenuBar : public TMenuView
{

public:

    TMenuBar( const TRect& bounds, TMenu *aMenu );
    TMenuBar( const TRect& bounds, TSubMenu &aMenu );
    ~TMenuBar();

    virtual void draw();
    virtual TRect getItemRect( TMenuItem *item );

private:

    virtual const char *streamableName() const
        { return name; }

protected:

    TMenuBar( StreamableInit );

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TMenuBar& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TMenuBar*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TMenuBar& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TMenuBar* cl )
    { return os << (TStreamable *)cl; }

#endif  // Uses_TMenuBar

/* ---------------------------------------------------------------------- */
/*      class TMenuBox                                                    */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Disabled text                                               */
/*        3 = Shortcut text                                               */
/*        4 = Normal selection                                            */
/*        5 = Disabled selection                                          */
/*        6 = Shortcut selection                                          */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TMenuBox ) && !defined( __TMenuBox )
#define __TMenuBox

class far TRect;
class far TMenu;
class far TMenuView;
class far TDrawBuffer;

class TMenuBox : public TMenuView
{

public:

    TMenuBox( const TRect& bounds, TMenu *aMenu, TMenuView *aParentMenu);

    virtual void draw();
    virtual TRect getItemRect( TMenuItem *item );

private:

    void frameLine( TDrawBuffer&, short n );
    void drawLine( TDrawBuffer& );

    static const char * near frameChars;
    virtual const char *streamableName() const
        { return name; }

protected:

    TMenuBox( StreamableInit );

public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TMenuBox& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TMenuBox*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TMenuBox& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TMenuBox* cl )
    { return os << (TStreamable *)cl; }

#endif  // Uses_TMenuBox


#if defined( Uses_TStatusItem ) && !defined( __TStatusItem )
#define __TStatusItem

class TStatusItem
{

public:

    TStatusItem( const char *aText,
                 ushort key,
                 ushort cmd,
                 TStatusItem *aNext = 0
                );

    TStatusItem *next;
    const char *text;
    ushort keyCode;
    ushort command;

};

inline TStatusItem::TStatusItem( const char *aText,
                                 ushort key,
                                 ushort cmd,
                                 TStatusItem *aNext
                                ) :
    text( aText ), keyCode( key ), command( cmd ), next( aNext )
{
}

#endif  // Uses_TStatusItem

#if defined( Uses_TStatusDef ) && !defined( __TStatusDef )
#define __TStatusDef

class TStatusDef
{

public:

    TStatusDef( ushort aMin,
                ushort aMax,
                TStatusItem *someItems = 0,
                TStatusDef *aNext = 0
              );

    TStatusDef *next;
    ushort min;
    ushort max;
    TStatusItem *items;
};

inline TStatusDef::TStatusDef( ushort aMin,
                               ushort aMax,
                               TStatusItem *someItems,
                               TStatusDef *aNext
                             ) :
    min( aMin ), max( aMax ), items( someItems ), next( aNext )
{
}

#endif  // Uses_TStatusDef

/* ---------------------------------------------------------------------- */
/*      class TStatusLine                                                 */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal text                                                 */
/*        2 = Disabled text                                               */
/*        3 = Shortcut text                                               */
/*        4 = Normal selection                                            */
/*        5 = Disabled selection                                          */
/*        6 = Shortcut selection                                          */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TStatusLine ) && !defined( __TStatusLine )
#define __TStatusLine

class far TRect;
class far TEvent;
class far TPoint;

class TStatusLine : public TView
{

public:

    TStatusLine( const TRect& bounds, TStatusDef& aDefs );
    ~TStatusLine();

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& event );
    virtual const char* hint( ushort aHelpCtx );
    void update();

protected:

    TStatusItem *items;
    TStatusDef *defs;

private:

    void drawSelect( TStatusItem *selected );
    void findItems();
    TStatusItem *itemMouseIsIn( TPoint );
    void disposeItems( TStatusItem *item );

    static const char * near hintSeparator;
    
    virtual const char *streamableName() const
        { return name; }

    static void writeItems( opstream&, TStatusItem * );
    static void writeDefs( opstream&, TStatusDef * );
    static TStatusItem *readItems( ipstream& );
    static TStatusDef *readDefs( ipstream& );


protected:

    TStatusLine( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );
 
public:

    static const char * const near name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, TStatusLine& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TStatusLine*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TStatusLine& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TStatusLine* cl )
    { return os << (TStreamable *)cl; }

#endif  // Uses_TStatusLine

#pragma option -Vo.
#if defined( __BCOPT__ )
#pragma option -po.
#endif


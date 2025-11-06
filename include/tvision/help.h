/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   HELP.H                                                                */
/*                                                                         */
/*   defines the classes THelpViewer and THelpWindow                       */
/*                                                                         */
/* ------------------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#if !defined( __HELP_H )
#define __HELP_H

#define Uses_TStreamable
#define Uses_ipstream
#define Uses_opstream
#define Uses_fpstream
#define Uses_TObject
#define Uses_TPoint
#define Uses_TRect
#define Uses_TEvent
#define Uses_TScroller
#define Uses_TScrollBar
#define Uses_TWindow
#include <tvision/tv.h>

#include <tvision/helpbase.h>

/* ---------------------------------------------------------------------- */
/*      class THelpViewer                                                 */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Normal                                                      */
/*        2 = Keyword                                                     */
/*        3 = Selected keyword                                            */
/* ---------------------------------------------------------------------- */

class THelpViewer : public TScroller
{
public:

    THelpViewer( const TRect&, TScrollBar*, TScrollBar*, THelpFile*, ushort ) noexcept;
    ~THelpViewer();

    virtual void changeBounds( const TRect& );
    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& );
    void makeSelectVisible( int, TPoint&, uchar&, int& );
    void switchToTopic( int );

    THelpFile *hFile;
    THelpTopic *topic;
    int selected;
};

/* ---------------------------------------------------------------------- */
/*      class THelpWindow                                                 */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Frame passive                                               */
/*        2 = Frame active                                                */
/*        3 = Frame icon                                                  */
/*        4 = ScrollBar page area                                         */
/*        5 = ScrollBar controls                                          */
/*        6 = HelpViewer normal                                           */
/*        7 = HelpViewer keyword                                          */
/*        8 = HelpViewer selected keyword                                 */
/* ---------------------------------------------------------------------- */

class THelpWindow : public TWindow
{

    static const char * _NEAR helpWinTitle;

public:

    THelpWindow( THelpFile*, ushort ) noexcept;

    virtual TPalette& getPalette() const;
};


extern void notAssigned( opstream& s, int value );

extern TCrossRefHandler crossRefHandler;

#endif  // __HELP_H

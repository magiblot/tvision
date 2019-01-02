/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   HELP.H                                                                */
/*                                                                         */
/*   Copyright (c) Borland International 1991                              */
/*   All Rights Reserved.                                                  */
/*                                                                         */
/*   defines the classes THelpViewer and THelpWindow                       */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if !defined( __HELP_H )
#define __HELP_H

#define Uses_TStreamable
#define Uses_ipstream
#define Uses_opstream
#define Uses_TObject
#define Uses_TPoint
#define Uses_TRect
#define Uses_TEvent
#define Uses_TScroller
#define Uses_TScrollBar
#define Uses_TWindow
#include <tv.h>

#include <helpbase.h>

// THelpViewer

class THelpViewer : public TScroller
{
public:

    THelpViewer( const TRect&, TScrollBar*, TScrollBar*, THelpFile*, ushort );
    ~THelpViewer();

    virtual void changeBounds( TRect& );
    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& );
    void makeSelectVisible( int, TPoint&, uchar&, int& );
    void switchToTopic( int );

    THelpFile *hFile;
    THelpTopic *topic;
    int selected;
};

// THelpWindow

class THelpWindow : public TWindow
{
public:

    THelpWindow( THelpFile*, ushort );

    virtual TPalette& getPalette() const;
};


extern void notAssigned( opstream& s, int value );

extern TCrossRefHandler crossRefHandler;

#endif  // __HELP_H

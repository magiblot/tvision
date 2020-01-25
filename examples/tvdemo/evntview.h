#ifndef EVNTVIEW_H
#define EVNTVIEW_H

#define Uses_TWindow
#define Uses_TRect
#define Uses_TTerminal
#define Uses_otstream
#include <tvision/tv.h>

// TEventViewer: a TTerminal window displaying the attributes of TEvents
// received by the application.
//
// toggle() will create a new window if there's not one, or will stop or resume
// the printing of events on the already existing window.
//
// Inspired by TTYWindow from Daniel Ambrose.

class TEventViewer : public TWindow
{

public:

    static TEventViewer *toggle();
    static void print( const TEvent & );

private:

    static void printEvent( ostream &, const TEvent & );
    static TEventViewer *viewer;

    TEventViewer( TRect bounds, const char *aTitle, ushort aBufSize );
    ~TEventViewer();

    TTerminal *interior;
    otstream *out;
    Boolean resumed;
    uint eventCount;

};

#endif

#ifndef EVNTVIEW_H
#define EVNTVIEW_H

#define Uses_TWindow
#define Uses_TRect
#include <tvision/tv.h>

// TEventViewer: a TTerminal window displaying the attributes of TEvents
// received by the application.
//
// Inspired by TTYWindow from Daniel Ambrose.

class TEventViewer : public TWindow
{
    ostream *out;
    Boolean stopped;
    size_t eventCount;

    static const char * const titles[2];

    static void printEvent(ostream &out, const TEvent &ev);

public:

    TEventViewer(const TRect &bounds, ushort aBufSize);
    ~TEventViewer();

    virtual void handleEvent(TEvent &ev);
    virtual void shutDown();

    void toggle();
    void print(const TEvent &ev);
};

#endif

#ifndef GPMINPUT_H
#define GPMINPUT_H

#include <internal/platform.h>

#ifdef HAVE_GPM

#define Uses_TPoint
#define Uses_TEvent
#include <tvision/tv.h>

#include <internal/cursor.h>
#include <gpm.h>

class GpmInput : public FdInputStrategy {

    NegativeScreenCursor cursor;
    uchar buttonState;

    static void fitEvent(Gpm_Event&);

public:

    GpmInput();
    ~GpmInput();
    bool getEvent(TEvent &ev);
    int getButtonCount();

};

#else

class GpmInput : public FdInputStrategy {};

#endif // HAVE_GPM

#endif

#ifndef GPMINPUT_H
#define GPMINPUT_H

#define Uses_TPoint
#define Uses_TEvent
#include <tvision/tv.h>

#include <internal/platform.h>
#include <internal/cursor.h>

class GpmInput : public FdInputStrategy {

    ScreenCursor cursor;
    uchar buttonState;
    int buttonCount;

public:

    GpmInput();
    ~GpmInput();
    bool getEvent(TEvent &ev);
    int getButtonCount();

};

#endif

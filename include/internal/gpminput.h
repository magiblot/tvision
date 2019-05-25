#ifndef GPMINPUT_H
#define GPMINPUT_H

#define Uses_TPoint
#define Uses_TEvent
#include <tvision/tv.h>

#include <internal/platform.h>

class GpmInput : public AsyncInputStrategy {

    TPoint mousePos;
    uchar buttonState;
    int buttonCount;

public:

    GpmInput();
    ~GpmInput();
    bool getEvent(TEvent &ev);
    int getButtonCount();
    void drawPointer();

};

#endif

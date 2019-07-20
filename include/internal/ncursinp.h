#ifndef NCURSINP_H
#define NCURSINP_H

#define Uses_TKeys
#define Uses_TPoint
#define Uses_TEvent
#include <tvision/tv.h>

#include <internal/platform.h>
#include <unordered_map>
#include <string>
#include <functional>

class NcursesInput : public FdInputStrategy {

    static std::unordered_map<int, KeyDownEvent> fromCursesKeyCode;
    static std::unordered_map<std::string, KeyDownEvent> fromCursesHighKey;

    TPoint lastMousePos;
    uchar buttonState;
    int buttonCount;

    void detectAlt(int keys[4], bool &Alt);
    void parsePrintableChar(TEvent &ev, int keys[4], int &num_keys);
    void setAltModifier(TEvent &ev);
    void readUtf8Char(int keys[4], int &num_keys);
    bool parseMouseEvent(TEvent&);

public:

    NcursesInput(bool mouse = true);
    ~NcursesInput();

    bool getEvent(TEvent &ev);
    int getButtonCount();

};

#endif

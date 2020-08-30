#ifndef NCURSINP_H
#define NCURSINP_H

#include <internal/platform.h>

#ifdef HAVE_NCURSES

#define Uses_TKeys
#define Uses_TPoint
#define Uses_TEvent
#include <tvision/tv.h>

#include <internal/sigwinch.h>
#include <unordered_map>
#include <string_view>
#include <functional>

class NcursesInput : public FdInputStrategy, SigwinchAware {

    static std::unordered_map<int, KeyDownEvent> fromCursesKeyCode;
    static std::unordered_map<std::string_view, KeyDownEvent> fromCursesHighKey;

    TPoint lastMousePos;
    uchar buttonState;
    int buttonCount;
    bool mouseForced;

    void detectAlt(int keys[4], bool &Alt);
    void parsePrintableChar(TEvent &ev, int keys[4], int &num_keys);
    void setAltModifier(TEvent &ev);
    void readUtf8Char(int keys[4], int &num_keys);
    bool parseMouseEvent(TEvent&);

    static void printEscapeSeq(const char *);

public:

    NcursesInput(bool mouse = true);
    ~NcursesInput();

    bool getEvent(TEvent &ev);
    int getButtonCount();

};

#else

class NcursesInput : public InputStrategy {};

#endif // HAVE_NCURSES

#endif

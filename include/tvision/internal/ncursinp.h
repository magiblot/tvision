#ifndef TVISION_NCURSINP_H
#define TVISION_NCURSINP_H

#include <internal/platform.h>

#ifdef HAVE_NCURSES

#define Uses_TKeys
#define Uses_TPoint
#define Uses_TEvent
#include <tvision/tv.h>

namespace tvision
{

class NcursesDisplay;
struct InputState;

class NcursesInput : public InputStrategy
{
    enum : char { KEY_ESC = '\x1B' };
    enum { readTimeout = 10 };

    StdioCtl &io;
    InputState &state;
    bool mouseEnabled;

    static int getch_nb() noexcept;
    void detectAlt(int keys[4], bool &Alt) noexcept;
    void parsePrintableChar(TEvent &ev, int keys[4], int &num_keys) noexcept;
    void readUtf8Char(int keys[4], int &num_keys) noexcept;

    bool parseCursesMouse(TEvent&) noexcept;
    void consumeUnprocessedInput() noexcept;

public:

    // Lifetimes of 'io', 'display' and 'state' must exceed that of 'this'.
    NcursesInput(StdioCtl &io, NcursesDisplay &display, InputState &state, bool mouse) noexcept;
    ~NcursesInput();

    bool getEvent(TEvent &ev) noexcept;
    int getButtonCount() noexcept;
    bool hasPendingEvents() noexcept;
};

} // namespace tvision

#endif // HAVE_NCURSES

#endif // TVISION_NCURSINP_H

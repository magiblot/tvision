#ifndef TVISION_NCURSINP_H
#define TVISION_NCURSINP_H

#include <internal/platform.h>

#ifdef HAVE_NCURSES

#define Uses_TKeys
#define Uses_TPoint
#define Uses_TEvent
#include <tvision/tv.h>

#include <internal/terminal.h>

namespace tvision
{

class NcursesDisplay;

class NcursesInput : public InputStrategy
{
    enum : char { KEY_ESC = '\x1B' };
    enum { readTimeout = 5 };

    const StdioCtl &io;
    InputState state {};
    bool mouseEnabled;

    static int getch_nb() noexcept;
    void detectAlt(int keys[4], bool &Alt) noexcept;
    void parsePrintableChar(TEvent &ev, int keys[4], int &num_keys) noexcept;
    void readUtf8Char(int keys[4], int &num_keys) noexcept;

    bool parseCursesMouse(TEvent&) noexcept;

public:

    // Lifetimes of 'io' and 'display' must exceed that of 'this'.
    NcursesInput(const StdioCtl &io, NcursesDisplay &display, bool mouse) noexcept;
    ~NcursesInput();

    bool getEvent(TEvent &ev) noexcept;
    int getButtonCount() noexcept;
    bool hasPendingEvents() noexcept;
};

} // namespace tvision

#endif // HAVE_NCURSES

#endif // TVISION_NCURSINP_H

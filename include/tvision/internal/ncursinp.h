#ifndef TVISION_NCURSINP_H
#define TVISION_NCURSINP_H

#include <internal/platform.h>

#ifdef HAVE_NCURSES

#define Uses_TKeys
#define Uses_TPoint
#define Uses_TEvent
#include <tvision/tv.h>

#include <internal/termio.h>

namespace tvision
{

class NcursesDisplay;

struct NcursesInputGetter final : public InputGetter
{
    size_t pendingCount {0};

    int get() noexcept override;
    void unget(int k) noexcept override;
};

class NcursesInput : public InputAdapter
{
    enum : char { KEY_ESC = '\x1B' };
    enum { readTimeoutMs = 10 };

    ConsoleCtl &con;
    InputState &state;
    bool mouseEnabled;
    NcursesInputGetter in;

    int getChNb() noexcept;
    void detectAlt(int keys[4], bool &Alt) noexcept;
    void parsePrintableChar(TEvent &ev, int keys[4], int &num_keys) noexcept;
    void readUtf8Char(int keys[4], int &num_keys) noexcept;

    bool parseCursesMouse(TEvent&) noexcept;

public:

    // Lifetimes of 'con', 'display' and 'state' must exceed that of 'this'.
    NcursesInput(ConsoleCtl &con, NcursesDisplay &display, InputState &state, bool mouse) noexcept;
    ~NcursesInput();

    bool getEvent(TEvent &ev) noexcept override;
    bool hasPendingEvents() noexcept override;
};

} // namespace tvision

#endif // HAVE_NCURSES

#endif // TVISION_NCURSINP_H

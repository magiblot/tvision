#ifndef TVISION_NCURSINP_H
#define TVISION_NCURSINP_H

#include <internal/platform.h>

#ifdef HAVE_NCURSES

#define Uses_TKeys
#define Uses_TPoint
#define Uses_TEvent
#include <tvision/tv.h>

#include <internal/sigwinch.h>
#include <internal/terminal.h>

class NcursesInput : public InputStrategy
{
    enum : char { KEY_ESC = '\x1B' };
    enum { readTimeout = 5 };

    MouseState mstate;
    int buttonCount;
    bool mouseEnabled;

    static int getch_nb() noexcept;
    void detectAlt(int keys[4], bool &Alt) noexcept;
    void parsePrintableChar(TEvent &ev, int keys[4], int &num_keys) noexcept;
    void readUtf8Char(int keys[4], int &num_keys) noexcept;

    bool parseCursesMouse(TEvent&) noexcept;

    class NcGetChBuf : public GetChBuf
    {

    protected:

        int do_getch() noexcept override;
        bool do_ungetch(int) noexcept override;

    };

public:

    NcursesInput(bool mouse = true) noexcept;
    ~NcursesInput();

    bool getEvent(TEvent &ev) noexcept;
    int getButtonCount() noexcept;
    bool hasPendingEvents() noexcept;
};

#endif // HAVE_NCURSES

#endif // TVISION_NCURSINP_H

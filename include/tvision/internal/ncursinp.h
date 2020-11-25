#ifndef NCURSINP_H
#define NCURSINP_H

#include <internal/platform.h>

#ifdef HAVE_NCURSES

#define Uses_TKeys
#define Uses_TPoint
#define Uses_TEvent
#include <tvision/tv.h>

#include <internal/sigwinch.h>
#include <internal/terminal.h>

class NcursesInput : public FdInputStrategy, SigwinchAware {

    enum : char { KEY_ESC = '\x1B' };
    enum { readTimeout = 5 };

    MouseState mstate;
    int buttonCount;
    bool mouseEnabled;

    static int getch_nb();
    void detectAlt(int keys[4], bool &Alt);
    void parsePrintableChar(TEvent &ev, int keys[4], int &num_keys);
    void readUtf8Char(int keys[4], int &num_keys);

    bool parseCursesMouse(TEvent&);

    class NcGetChBuf : public GetChBuf
    {

    protected:

        int do_getch() override;
        bool do_ungetch(int) override;

    };

public:

    NcursesInput(bool mouse = true);
    ~NcursesInput();

    bool getEvent(TEvent &ev);
    int getButtonCount();
    bool hasPendingEvents();

};

#else

class NcursesInput : public InputStrategy {};

#endif // HAVE_NCURSES

#endif

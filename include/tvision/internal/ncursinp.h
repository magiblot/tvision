#ifndef NCURSINP_H
#define NCURSINP_H

#include <internal/platform.h>

#ifdef HAVE_NCURSES

#define Uses_TKeys
#define Uses_TPoint
#define Uses_TEvent
#include <tvision/tv.h>

#include <internal/sigwinch.h>

class NcursesInput : public FdInputStrategy, SigwinchAware {

    enum : char { KEY_ESC = '\x1B' };

    TPoint lastMousePos;
    uchar buttonState;
    int buttonCount;
    bool mouseEnabled;

    void detectAlt(int keys[4], bool &Alt);
    void parsePrintableChar(TEvent &ev, int keys[4], int &num_keys);
    void setAltModifier(TEvent &ev);
    void readUtf8Char(int keys[4], int &num_keys);

    struct GetChBuf {
        enum { maxSize = 31 };
        int keys[maxSize];
        uint size {0};

        int get();
        int last();
        void unget();
        void reject();
        int getNum();
    };

    enum ParseResult { Rejected = 0, Accepted, Ignored };

    ParseResult parseEscapeSeq(TEvent&);

    static void setMouse(bool enable);
    bool parseCursesMouse(TEvent&);
    bool acceptMouseEvent(TEvent&, TPoint where, uchar buttons, uchar wheel);
    ParseResult parseX10Mouse(GetChBuf&, TEvent&);
    ParseResult parseSGRMouse(GetChBuf&, TEvent&);

public:

    NcursesInput(bool mouse = true);
    ~NcursesInput();

    bool getEvent(TEvent &ev);
    int getButtonCount();
    bool hasPendingEvents();

};

inline int NcursesInput::GetChBuf::get()
{
    if (size < maxSize)
    {
        int k = wgetch(stdscr);
        if (k != ERR)
            keys[size++] = k;
        return k;
    }
    return ERR;
}

inline int NcursesInput::GetChBuf::last()
{
    if (size)
        return keys[size - 1];
    return ERR;
}

inline void NcursesInput::GetChBuf::unget()
{
    if (size)
        ungetch(keys[--size]);
}

inline void NcursesInput::GetChBuf::reject()
{
    while (size)
        ungetch(keys[--size]);
}

inline int NcursesInput::GetChBuf::getNum()
{
    int num = 0, digits = 0;
    int k;
    while ((k = get()) != ERR && '0' <= k && k <= '9')
    {
        num = 10 * num + (k - '0');
        ++digits;
    }
    return digits ? num : ERR;
}

#else

class NcursesInput : public InputStrategy {};

#endif // HAVE_NCURSES

#endif

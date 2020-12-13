#ifndef TVISION_TERMINAL_H
#define TVISION_TERMINAL_H

#define Uses_TPoint
#define Uses_TEvent
#include <tvision/tv.h>

struct MouseState
{
    TPoint where;
    uchar buttons;
    uchar wheel;
};

class GetChBuf
{
    enum { maxSize = 31 };

    int keys[maxSize];
    uint size {0};

protected:

    virtual int do_getch() = 0;
    virtual bool do_ungetch(int) = 0;

public:

    int get();
    int last(size_t i);
    void unget();
    void reject();
    int getNum();

};

inline int GetChBuf::get()
{
    if (size < maxSize)
    {
        int k = do_getch();
        if (k != -1)
            keys[size++] = k;
        return k;
    }
    return -1;
}

inline int GetChBuf::last(size_t i=0)
{
    if (i < size)
        return keys[size - 1 - i];
    return -1;
}

inline void GetChBuf::unget()
{
    if (size)
        do_ungetch(keys[--size]);
}

inline void GetChBuf::reject()
{
    while (size)
        do_ungetch(keys[--size]);
}

inline int GetChBuf::getNum()
{
    int num = 0, digits = 0;
    int k;
    while ((k = get()) != -1 && '0' <= k && k <= '9')
    {
        num = 10 * num + (k - '0');
        ++digits;
    }
    return digits ? num : -1;
}

enum ParseResult { Rejected = 0, Accepted, Ignored };

namespace TermIO
{

    void mouseOn();
    void mouseOff();

    ParseResult parseEscapeSeq(GetChBuf&, TEvent&, MouseState&);
    ParseResult parseX10Mouse(GetChBuf&, TEvent&, MouseState&);
    ParseResult parseSGRMouse(GetChBuf&, TEvent&, MouseState&);
    ParseResult parseFKeyA(GetChBuf&, TEvent&);
    ParseResult parseFKeyB(GetChBuf&, TEvent&);
    ParseResult parseArrowKeyA(GetChBuf&, TEvent&);
    ParseResult parseHomeEndA(GetChBuf&, TEvent&);

    bool acceptMouseEvent(TEvent &ev, MouseState &oldm, const MouseState &newm);
    void setAltModifier(TEvent &ev);

    #ifdef _TV_UNIX
    namespace Unix
    {
        TPoint getSize();
    }
    #endif // _TV_UNIX

}

#endif // TVISION_TERMINAL_H

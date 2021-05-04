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
    enum { maxSize = 63 };

    uint size {0};
    int keys[maxSize];

protected:

    virtual int do_getch() = 0;
    virtual bool do_ungetch(int) = 0;

public:

    int get(bool keepErr);
    int last(size_t i);
    void unget();
    void reject();
    bool getNum(uint &);
    bool getInt(int &);

};

inline int GetChBuf::get(bool keepErr=false)
{
    if (size < maxSize)
    {
        int k = do_getch();
        if (keepErr || k != -1)
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
    int k;
    if (size && (k = keys[--size]) != -1)
        do_ungetch(k);
}

inline void GetChBuf::reject()
{
    while (size)
        unget();
}

// getNum, getInt: INVARIANT: the last non-digit read key (or -1)
// can be accessed with 'last()' and can also be ungetted.

inline bool GetChBuf::getNum(uint &result)
{
    uint num = 0, digits = 0;
    int k;
    while ((k = get(true)) != -1 && '0' <= k && k <= '9')
    {
        num = 10 * num + (k - '0');
        ++digits;
    }
    if (digits)
        return (result = num), true;
    return false;
}

inline bool GetChBuf::getInt(int &result)
{
    int num = 0, digits = 0, sign = 1;
    int k = get(true);
    if (k == '-')
    {
        sign = -1;
        k = get(true);
    }
    while (k != -1 && '0' <= k && k <= '9')
    {
        num = 10 * num + (k - '0');
        ++digits;
        k = get(true);
    }
    if (digits)
        return (result = sign*num), true;
    return false;
}

enum ParseResult { Rejected = 0, Accepted, Ignored };

struct CSIData
{
    // Represents the data stored in a CSI escape sequence:
    // \x1B [ val[0] sep[0] val[1] sep[1] ...

    // CSIs can be longer, but this is the largest we need for now.
    enum { maxLength = 3 };

    uint val[maxLength];
    uint sep[maxLength];
    uint length;

    bool readFrom(GetChBuf &buf)
    {
        length = 0;
        for (uint i = 0; i < maxLength; ++i)
        {
            if (!buf.getNum(val[i]))
                val[i] = 1;
            int k = buf.last();
            if (k == -1) return false;
            if ((sep[i] = (uint) k) != ';')
                return (length = i + 1), true;
        }
        return false;
    }

    uint terminator() const
    {
        return length ? sep[length - 1] : 0;
    }
};

namespace TermIO
{

    void mouseOn();
    void mouseOff();
    void kittyKeysOn();
    void kittyKeysOff();

    ParseResult parseEscapeSeq(GetChBuf&, TEvent&, MouseState&);
    ParseResult parseX10Mouse(GetChBuf&, TEvent&, MouseState&);
    ParseResult parseSGRMouse(GetChBuf&, TEvent&, MouseState&);
    ParseResult parseCSIKey(const CSIData &csi, TEvent&);
    ParseResult parseFKeyA(GetChBuf&, TEvent&);
    ParseResult parseSS3Key(GetChBuf&, TEvent&);
    ParseResult parseArrowKeyA(GetChBuf&, TEvent&);
    ParseResult parseFixTermKey(const CSIData &csi, TEvent&);

    bool acceptMouseEvent(TEvent &ev, MouseState &oldm, const MouseState &newm);
    void setAltModifier(KeyDownEvent &keyDown);

#ifdef _TV_UNIX
    namespace Unix
    {
        void consoleWrite(const void *data, size_t bytes);
        TPoint getSize();
    }
    namespace Impl = Unix;
#elif defined(_WIN32)
    namespace Win32
    {
        void consoleWrite(const void *data, size_t bytes);
    }
    namespace Impl = Win32;
#endif // _TV_UNIX

    bool isLinuxConsole();

    inline void consoleWrite(const void *data, size_t bytes)
    {
        Impl::consoleWrite(data, bytes);
    }

}

#endif // TVISION_TERMINAL_H

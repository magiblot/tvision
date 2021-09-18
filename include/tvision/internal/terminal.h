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
    uchar mods;
};

class GetChBuf
{
    enum { maxSize = 63 };

    uint size {0};
    int keys[maxSize];

protected:

    virtual int do_getch() noexcept = 0;
    virtual bool do_ungetch(int) noexcept = 0;

public:

    int get(bool keepErr) noexcept;
    int last(size_t i) noexcept;
    void unget() noexcept;
    void reject() noexcept;
    bool getNum(uint &) noexcept;
    bool getInt(int &) noexcept;

};

inline int GetChBuf::get(bool keepErr=false) noexcept
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

inline int GetChBuf::last(size_t i=0) noexcept
{
    if (i < size)
        return keys[size - 1 - i];
    return -1;
}

inline void GetChBuf::unget() noexcept
{
    int k;
    if (size && (k = keys[--size]) != -1)
        do_ungetch(k);
}

inline void GetChBuf::reject() noexcept
{
    while (size)
        unget();
}

// getNum, getInt: INVARIANT: the last non-digit read key (or -1)
// can be accessed with 'last()' and can also be ungetted.

inline bool GetChBuf::getNum(uint &result) noexcept
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

inline bool GetChBuf::getInt(int &result) noexcept
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

    bool readFrom(GetChBuf &buf) noexcept
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

    uint terminator() const noexcept
    {
        return length ? sep[length - 1] : 0;
    }
};

namespace TermIO
{

    void mouseOn() noexcept;
    void mouseOff() noexcept;
    void kittyKeysOn() noexcept;
    void kittyKeysOff() noexcept;

    ParseResult parseEscapeSeq(GetChBuf&, TEvent&, MouseState&) noexcept;
    ParseResult parseX10Mouse(GetChBuf&, TEvent&, MouseState&) noexcept;
    ParseResult parseSGRMouse(GetChBuf&, TEvent&, MouseState&) noexcept;
    ParseResult parseCSIKey(const CSIData &csi, TEvent&) noexcept;
    ParseResult parseFKeyA(GetChBuf&, TEvent&) noexcept;
    ParseResult parseSS3Key(GetChBuf&, TEvent&) noexcept;
    ParseResult parseArrowKeyA(GetChBuf&, TEvent&) noexcept;
    ParseResult parseFixTermKey(const CSIData &csi, TEvent&) noexcept;

    bool acceptMouseEvent(TEvent &ev, MouseState &oldm, const MouseState &newm) noexcept;
    void setAltModifier(KeyDownEvent &keyDown) noexcept;

#ifdef _TV_UNIX
    namespace Unix
    {
        TPoint getSize() noexcept;
    }
#endif // _TV_UNIX

    void consoleWrite(const char *data, size_t bytes) noexcept;
    bool isLinuxConsole() noexcept;

}

#endif // TVISION_TERMINAL_H

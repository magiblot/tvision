#ifndef TVISION_TERMIO_H
#define TVISION_TERMIO_H

#define Uses_TPoint
#define Uses_TEvent
#include <tvision/tv.h>

#include <tvision/compat/windows/windows.h>

namespace tvision
{

class ConsoleCtl;

struct Far2lState
{
    bool enabled {false};
};

struct InputState
{
    uchar buttons {0};
#ifdef _WIN32
    wchar_t surrogate {0};
#endif
    Far2lState far2l;
    bool hasFullOsc52 {false};
    bool bracketedPaste {false};
    bool gotDsrResponse {false};
    void (*putPaste)(TStringView) {nullptr};
};

class InputGetter
{
public:

    virtual int get() noexcept = 0;
    virtual void unget(int) noexcept = 0;
};

class GetChBuf
{
    enum { maxSize = 31 };

    uint size {0};
    int keys[maxSize];

public:

    InputGetter &in;

    GetChBuf(InputGetter &aIn) noexcept :
        in(aIn)
    {
    }

    inline int getUnbuffered() noexcept;
    inline int get(bool keepErr = false) noexcept;
    inline int last(size_t i) noexcept;
    inline void unget() noexcept;
    void reject() noexcept;
    bool getNum(uint &) noexcept;
    bool getInt(int &) noexcept;
    bool readStr(TStringView) noexcept;
};

inline int GetChBuf::getUnbuffered() noexcept
{
    return in.get();
}

inline int GetChBuf::get(bool keepErr) noexcept
{
    if (size < maxSize)
    {
        int k = in.get();
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
        in.unget(k);
}

struct CSIData
{
    // Represents the data stored in a CSI escape sequence. For example,
    // given the input "\x1B[123::456;789u", the CSIData would be:
    // - .length: 5
    // - .terminator: 'u'
    // - .getValue(0, <a default value>): 123
    // - .getSeparator(0): ':'
    // - .getValue(1, <a default value>): <a default value>
    // - .getSeparator(1): ':'
    // - .getValue(2, <a default value>): 456
    // - .getSeparator(2): ';'
    // - .getValue(3, <a default value>): 789

    // CSIs can be longer, but this is the largest we need for now.
    enum { maxLength = 6 };

    uint length {0};
    uint _values[maxLength];
    char _separators[maxLength - 1];
    char terminator {0};

    bool readFrom(GetChBuf &buf) noexcept;
    inline uint getValue(uint i, uint aDefault = 1) const noexcept;
    inline char getSeparator(uint i) const noexcept;
};

inline uint CSIData::getValue(uint i, uint aDefault) const noexcept
{
    if (i < length && _values[i] != UINT_MAX)
        return _values[i];
    return aDefault;
}

inline char CSIData::getSeparator(uint i) const noexcept
{
    if (length > 0 && i < length - 1)
        return _separators[i];
    return '\0';
}

enum ParseResult { Rejected = 0, Accepted, Ignored };

namespace TermIO
{
    void mouseOn(ConsoleCtl &) noexcept;
    void mouseOff(ConsoleCtl &) noexcept;
    void keyModsOn(ConsoleCtl &) noexcept;
    void keyModsOff(ConsoleCtl &) noexcept;

    void normalizeKey(KeyDownEvent &keyDown) noexcept;

    bool setClipboardText(ConsoleCtl &, TStringView, InputState &) noexcept;
    bool requestClipboardText(ConsoleCtl &, void (&)(TStringView), InputState &) noexcept;

    ParseResult parseEvent(GetChBuf&, TEvent&, InputState&) noexcept;
    ParseResult parseEscapeSeq(GetChBuf&, TEvent&, InputState&) noexcept;
    ParseResult parseX10Mouse(GetChBuf&, TEvent&, InputState&) noexcept;
    ParseResult parseSGRMouse(GetChBuf&, TEvent&, InputState&) noexcept;
    ParseResult parseCSIKey(const CSIData &csi, TEvent&, InputState&) noexcept;
    ParseResult parseSS3Key(GetChBuf&, TEvent&) noexcept;
    ParseResult parseKittyKey(const CSIData &csi, TEvent&) noexcept;
    ParseResult parseDCS(GetChBuf&, InputState&) noexcept;
    ParseResult parseOSC(GetChBuf&, InputState&) noexcept;
    ParseResult parseCPR(const CSIData &csi, InputState&) noexcept;
    ParseResult parseWin32InputModeKeyOrEscapeSeq(const CSIData &, InputGetter&, TEvent&, InputState&) noexcept;

    char *readUntilBelOrSt(GetChBuf &) noexcept;
    void consumeUnprocessedInput(ConsoleCtl &, InputGetter &, InputState &) noexcept;
}

} // namespace tvision

#endif // TVISION_TERMIO_H

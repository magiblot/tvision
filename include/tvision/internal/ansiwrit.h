#ifndef TVISION_ANSIWRIT_H
#define TVISION_ANSIWRIT_H

#define Uses_TScreenCell
#define Uses_TPoint
#include <tvision/tv.h>

#include <internal/endian.h>

namespace tvision
{

class ConsoleCtl;
class DisplayAdapter;

// TermColor represents a color that is to be printed to screen
// using certain ANSI escape sequences.

struct TermColor
{
    enum TermColorTypes : uint8_t { Default, Indexed, RGB, NoColor };

    union
    {
        uint8_t idx;
        uint8_t bgr[3];
    };
    TermColorTypes type;

    TermColor() = default;

    // GCC has issues optimizing the initialization of this struct.
    // So do bit-casting manually.

    TermColor& operator=(uint32_t val) noexcept
    {
#ifdef TV_BIG_ENDIAN
        reverseBytes(val);
#endif
        memcpy(this, &val, sizeof(*this));
        return *this;
        static_assert(sizeof(*this) == 4, "");
    }
    operator uint32_t() const noexcept
    {
        uint32_t val;
        memcpy(&val, this, sizeof(*this));
#ifdef TV_BIG_ENDIAN
        reverseBytes(val);
#endif
        return val;
    }
    TermColor(uint8_t aIdx, TermColorTypes aType) noexcept
    {
        *this = aIdx | (uint32_t(aType) << 24);
    }
    TermColor(TColorRGB c, TermColorTypes aType) noexcept
    {
        *this = uint32_t(c) | (uint32_t(aType) << 24);
    }
    TermColor(TermColorTypes aType) noexcept
    {
        *this = uint32_t(aType) << 24;
    }

};

struct TermAttr
{
    TermColor fg, bg;
    TColorAttr::Style style;
};

// Terminal quirk flags.

const ushort
    qfBoldIsBright  = 0x0001,
    qfBlinkIsBright = 0x0002,
    qfNoItalic      = 0x0004,
    qfNoUnderline   = 0x0008;

enum TermCapColors : uint8_t
{
    NoColor,
    Indexed8,
    Indexed16,
    Indexed256,
    Direct,
    TermCapColorCount,
};

struct TermCap
{
    TermCapColors colors;
    ushort quirks;

    static TermCap getDisplayCapabilities( ConsoleCtl &con,
                                           DisplayAdapter &display ) noexcept;
};

// AnsiScreenWriter allows printing characters and color attributes directly
// to screen using ANSI escape codes.

class AnsiScreenWriter
{
    class Buffer
    {
        char *head {nullptr};
        size_t capacity {0};
    public:
        char *tail {nullptr};

        ~Buffer();
        char *data() noexcept;
        size_t size() const noexcept;
        void clear() noexcept;
        void push(TStringView) noexcept;
        void push(char) noexcept;
        void reserve(size_t) noexcept;
    };

    ConsoleCtl &con;
    TermCap termcap;
    Buffer buf;
    TPoint caretPos {-1, -1};
    TermAttr lastAttr {};

    void bufWriteCSI1(int a, char F) noexcept;
    void bufWriteCSI2(int a, int b, char F) noexcept;

public:

    AnsiScreenWriter(ConsoleCtl &aCon, TermCap aTermcap) noexcept :
        con(aCon),
        termcap(aTermcap)
    {
    }

    ~AnsiScreenWriter();

    void reset() noexcept;
    void clearScreen() noexcept;

    void writeCell(TPoint, TStringView, TColorAttr, bool) noexcept;
    void setCaretPosition(TPoint) noexcept;
    void flush() noexcept;
};

} // namespace tvision

#endif // TVISION_ANSIWRIT_H

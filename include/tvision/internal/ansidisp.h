#ifndef ANSIDISP_H
#define ANSIDISP_H

#define Uses_TScreenCell
#include <tvision/tv.h>

#include <internal/termdisp.h>
#include <vector>

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

    TermColor& operator=(uint32_t val)
    {
        memcpy(this, &val, sizeof(*this));
        return *this;
        static_assert(sizeof(*this) == 4, "");
    }
    operator uint32_t() const
    {
        uint32_t val;
        memcpy(&val, this, sizeof(*this));
        return val;
    }
    TermColor(uint8_t aIdx, TermColorTypes aType)
    {
        *this = aIdx | (uint32_t(aType) << 24);
    }
    TermColor(TColorRGB c, TermColorTypes aType)
    {
        *this = uint32_t(c) | (uint32_t(aType) << 24);
    }
    TermColor(TermColorTypes aType)
    {
        *this = uint32_t(aType) << 24;
    }

};

using attrstyle_t = TColorAttr::Style;

struct TermAttr
{
    TermColor fg, bg;
    attrstyle_t style;
};

/* AnsiDisplay is a simple diplay backend which prints characters and ANSI
 * escape codes directly to stdout.
 *
 * AnsiDisplay implements only a subset of DisplayStrategy's pure virtual
 * functions, so it depends on another implementation from which it inherits,
 * which is the template parameter. In particular, AnsiDisplay implements the
 * lowlevel<*> functions from the BufferedDisplay interface. Thus, the base
 * class must also be a derived of BufferedDisplay.
 *
 * This templated inheritance also makes it possible to combine this class
 * with input strategies which depend on a certain display strategy,
 * as is the case of NcursesInput and NcursesDisplay. */

class AnsiDisplayBase {

    std::vector<char> buf;
    TermAttr lastAttr {};

    void bufWrite(TStringView s);
    void bufWriteCSI1(uint a, char F);
    void bufWriteCSI2(uint a, uint b, char F);

protected:

    ~AnsiDisplayBase();

    void clearAttributes();
    void clearScreen();

    void lowlevelWriteChars(TStringView chars, TColorAttr attr, const TermCap &);
    void lowlevelMoveCursor(uint x, uint y);
    void lowlevelMoveCursorX(uint x, uint y);
    void lowlevelFlush();

};

template<class DisplayBase>
class AnsiDisplay : public DisplayBase, public AnsiDisplayBase {

    void assertBaseClassBuffered()
    {
        static_assert(std::is_base_of<TerminalDisplay, DisplayBase>::value,
            "The base class of AnsiDisplay must be a derived of TerminalDisplay."
        );
    }

public:

    template <typename ...Args>
    AnsiDisplay(Args&& ...args) :
        DisplayBase(args...)
    {
    }

    void lowlevelWriteChars(TStringView chars, TColorAttr attr) override
        { AnsiDisplayBase::lowlevelWriteChars(chars, attr, TerminalDisplay::termcap); }
    void lowlevelMoveCursor(uint x, uint y) override { AnsiDisplayBase::lowlevelMoveCursor(x, y); }
    void lowlevelMoveCursorX(uint x, uint y) override { AnsiDisplayBase::lowlevelMoveCursorX(x, y); }
    void lowlevelFlush() override { AnsiDisplayBase::lowlevelFlush(); }

    void reloadScreenInfo() override
    {
        DisplayBase::reloadScreenInfo();
        clearAttributes();
    }

};

#endif // ANSIDISP_H

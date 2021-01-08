#ifndef ANSIDISP_H
#define ANSIDISP_H

#include <internal/buffdisp.h>
#include <internal/textattr.h>
#include <vector>

/* AnsiDisplay is a simple diplay backend which prints characters and ANSI
 * escape codes directly to stdout.
 * Character attributes are supported but assume Ncurses is being used.
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
    SGRAttribs lastAttr;
    uint sgrFlags;

    void bufWrite(TStringView s);
    void bufWriteCSI1(uint a, char F);
    void bufWriteCSI2(uint a, uint b, char F);
    void writeAttributes(TCellAttribs attr);

protected:

    AnsiDisplayBase();
    ~AnsiDisplayBase();

    void clearAttributes();
    void clearScreen();

    void lowlevelWriteChars(TStringView chars, TCellAttribs attr);
    void lowlevelMoveCursor(uint x, uint y);
    void lowlevelMoveCursorX(uint x, uint y);
    void lowlevelFlush();

};

template<class DisplayBase>
class AnsiDisplay : public DisplayBase, public AnsiDisplayBase {

    void assertBaseClassBuffered()
    {
        static_assert(std::is_base_of<BufferedDisplay, DisplayBase>::value,
            "The base class of AnsiDisplay must be a derived of BufferedDisplay."
        );
    }

public:

    template <typename ...Args>
    AnsiDisplay(Args&& ...args) :
        DisplayBase(args...)
    {
    }

    void lowlevelWriteChars(TStringView chars, TCellAttribs attr) override { AnsiDisplayBase::lowlevelWriteChars(chars, attr); }
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

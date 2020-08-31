#ifndef ANSIDISP_H
#define ANSIDISP_H

#include <internal/buffdisp.h>
#include <internal/textattr.h>
#include <vector>
#include <string_view>

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

    void bufWrite(std::string_view s);
    void bufWriteCSI1(uint a, char F);
    void bufWriteCSI2(uint a, uint b, char F);
    void writeAttributes(TCellAttribs attr);

protected:

    AnsiDisplayBase();
    ~AnsiDisplayBase();

    void clearAttributes();
    void clearScreen();
#ifdef _TV_UNIX
    void getCaretPosition(int &x, int &y);
    void getScreenSize(int &rows, int &cols);
    int getScreenRows();
    int getScreenCols();
#endif

    ushort getScreenMode();

    void lowlevelWriteChars(const uchar chars[4], TCellAttribs attr);
    void lowlevelMoveCursor(uint x, uint y);
    void lowlevelMoveCursorX(uint x, uint y);
    void lowlevelMoveCursorYby1(uint x, uint y);
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

    ushort getScreenMode() { return AnsiDisplayBase::getScreenMode(); }
#ifdef _TV_UNIX
    int getScreenRows() { return AnsiDisplayBase::getScreenRows(); }
    int getScreenCols() { return AnsiDisplayBase::getScreenCols(); }
#endif

    void lowlevelWriteChars(const uchar chars[4], TCellAttribs attr) { AnsiDisplayBase::lowlevelWriteChars(chars, attr); }
    void lowlevelMoveCursor(uint x, uint y) { AnsiDisplayBase::lowlevelMoveCursor(x, y); }
    void lowlevelMoveCursorX(uint x, uint y) { AnsiDisplayBase::lowlevelMoveCursorX(x, y); }
    void lowlevelMoveCursorYby1(uint x, uint y) { AnsiDisplayBase::lowlevelMoveCursorYby1(x, y); }
    void lowlevelFlush() { AnsiDisplayBase::lowlevelFlush(); }

    void onScreenResize() {
        BufferedDisplay::onScreenResize();
        DisplayBase::lowlevelFlush();
        clearAttributes();
    }

};

#endif // ANSIDISP_H

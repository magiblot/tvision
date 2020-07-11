#ifndef ANSIDISP_H
#define ANSIDISP_H

#include <internal/buffdisp.h>
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
    uchar lastAttr;

    void bufWrite(std::string_view s);
    void bufWriteSeq(uint a, uint b, char F);
    void writeAttributes(uchar attr);

protected:

    AnsiDisplayBase();
    ~AnsiDisplayBase();

    void clearAttributes();
    void clearScreen();
    void getCaretPosition(int &x, int &y);
    void getScreenSize(int &rows, int &cols);

    ushort getScreenMode();
    int getScreenRows();
    int getScreenCols();

    void lowlevelWriteChars(std::string_view chars, uchar attr);
    void lowlevelMoveCursor(uint x, uint y);
    void lowlevelFlush();

};

template<class DisplayBase>
class AnsiDisplay : public AnsiDisplayBase, public DisplayBase {

    void assertBaseClassBuffered()
    {
        static_assert(std::is_base_of<BufferedDisplay, DisplayBase>::value,
            "The base class of AnsiDisplay must be a derived of BufferedDisplay."
        );
    }

public:

    ushort getScreenMode() { return AnsiDisplayBase::getScreenMode(); }
    int getScreenRows() { return AnsiDisplayBase::getScreenRows(); }
    int getScreenCols() { return AnsiDisplayBase::getScreenCols(); }

    void lowlevelWriteChars(std::string_view chars, uchar attr) { AnsiDisplayBase::lowlevelWriteChars(chars, attr); }
    void lowlevelMoveCursor(uint x, uint y) { AnsiDisplayBase::lowlevelMoveCursor(x, y); }
    void lowlevelFlush() { AnsiDisplayBase::lowlevelFlush(); }

    void onScreenResize() {
        BufferedDisplay::onScreenResize();
        DisplayBase::lowlevelFlush();
        clearAttributes();
    }

};

#endif

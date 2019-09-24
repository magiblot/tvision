#ifndef ANSIDISP_H
#define ANSIDISP_H

#define Uses_TScreen
#include <tvision/tv.h>
#include <internal/codepage.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <cstdio>
#include <cstdlib>

#define CSI "\x1B["

/* AnsiDisplay is a simple diplay backend which prints characters and ANSI
 * escape codes directly to stderr (stdout doesn't work, for some reason).
 * Character attributes are not supported.
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

class BufferedDisplay;

template<class DisplayBase>
class AnsiDisplay : public DisplayBase {

public:

    AnsiDisplay()
    {
        static_assert(std::is_base_of<BufferedDisplay, DisplayBase>::value,
            "The base class of AnsiDisplay must be a derived of BufferedDisplay."
        );
    }

    void getCaretPosition(int &x, int &y)
    {
        struct termios saved, temporary;
        tcgetattr(0, &saved);
        temporary = saved;
        temporary.c_lflag &= ~ICANON;
        temporary.c_lflag &= ~ECHO;
        temporary.c_cflag &= ~CREAD;
        tcsetattr(0, TCSANOW, &temporary);
        fprintf(stderr, CSI"6n");
        fscanf(stdin, CSI"%d;%dR", &y, &x);
        tcsetattr(0, TCSANOW, &saved);
    }

    void clearScreen()
    {
        fprintf(stderr, CSI"2J");
    }

    void getScreenSize(int &rows, int &cols)
    {
        struct winsize w;
        ioctl(0, TIOCGWINSZ, &w);
        rows = w.ws_row;
        cols = w.ws_col;
    }

    int getScreenRows()
    {
        int rows, _;
        getScreenSize(rows, _);
        return rows;
    }

    int getScreenCols()
    {
        int cols, _;
        getScreenSize(_, cols);
        return cols;
    }

    ushort getScreenMode()
    {
        return TDisplay::smMono;
    }

protected:

    void lowlevelWriteChar(uchar character, ushort attr)
    {
        fprintf(stderr, "%s", CpTranslator::toUtf8(character));
    }

    void lowlevelMoveCursor(int x, int y)
    {
        fprintf(stderr, CSI"%d;%dH", y + 1, x + 1);
    }

    void lowlevelFlush() {}

};

#endif

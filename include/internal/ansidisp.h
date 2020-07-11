#ifndef ANSIDISP_H
#define ANSIDISP_H

#define Uses_TScreen
#include <tvision/tv.h>
#include <internal/codepage.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <ncurses.h> // For COLORS

#define CSI "\x1B["

/* AnsiDisplay is a simple diplay backend which prints characters and ANSI
 * escape codes directly to stderr (stdout doesn't work, for some reason).
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

class BufferedDisplay;

template<class DisplayBase>
class AnsiDisplay : public DisplayBase {

    std::stringstream buf;

    struct AnsiColor {
        union {
            uchar asChar;
            uchar
                value   : 3,
                bright  : 1;
        };
        AnsiColor(uchar c) : asChar(c) {}
    };

    struct charAttr {
        union {
            uchar asChar;
            uchar fg    : 4,
                  bg    : 4;
        };
        charAttr(uchar c) : asChar(c) {}
    } /*lastAttr = '\x07'*/;

    uchar lastAttr = '\x07';

    int cvtColor(uchar c, bool b=false) {
        return (bright(c) ? (b ? 100 : 90) : (b ? 40 : 30)) + value(c);
    }

    static uchar fg (uchar attr) {
        return attr & 0x0F;
    }

    static uchar bg (uchar attr) {
        return uchar(attr & 0xF0) >> 4;
    }

    static uchar value (uchar color) {
        return swapRedBlue(color & 0x07);
    }

    static uchar bright (uchar color) {
        return color & (COLORS < 16 ? 0 : 0x08);
    }

    void writeAttributes(uchar attr) {
        if (attr != lastAttr)
        {
            buf << CSI
                << cvtColor(fg(attr) == fg(lastAttr) ? fg(lastAttr) : fg(attr))
                << ';'
                << cvtColor(bg(attr) == bg(lastAttr) ? bg(lastAttr) : bg(attr), true)
                << 'm';
            if (COLORS < 16)
            {
                if ((attr & 0x08) && !(lastAttr & 0x08))
                    buf << CSI "1m";
                else if (!(attr & 0x08) && (lastAttr & 0x08))
                    buf << CSI "22m";
            }
        }
        lastAttr = attr;
    }

    static uchar swapRedBlue (uchar c) {
        return (c & ~0x5) | ((c & 0x4) >> 2) | ((c & 0x1) << 2);
    }

public:

    AnsiDisplay()
    {
        static_assert(std::is_base_of<BufferedDisplay, DisplayBase>::value,
            "The base class of AnsiDisplay must be a derived of BufferedDisplay."
        );
    }
    
    ~AnsiDisplay()
    {
        clearAttributes();
        clearScreen();
        lowlevelFlush();
    }

    void getCaretPosition(int &x, int &y)
    {
        lowlevelFlush();
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

    void clearAttributes()
    {
        buf << CSI"0m";
    }

    void clearScreen()
    {
        buf << CSI"2J";
    }

    void getScreenSize(int &rows, int &cols)
    {
        lowlevelFlush();
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
        return TDisplay::smCO80;
    }

protected:

    void lowlevelWriteChar(uchar character, ushort attr)
    {
        writeAttributes(attr);
        buf << CpTranslator::toUtf8(character);
    }

    void lowlevelMoveCursor(int x, int y)
    {
        buf << CSI << y+1 << ";" << x+1 << "H";
    }

    void lowlevelFlush() {
        fputs(buf.str().c_str(), stdout);
        fflush(stdout);
        std::stringstream().swap(buf);
    }

    void onScreenResize() {
        BufferedDisplay::onScreenResize();
        DisplayBase::lowlevelFlush();
        lastAttr = '\x07';
    }

};

#endif

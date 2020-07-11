#define Uses_TScreen
#include <tvision/tv.h>
#include <internal/ansidisp.h>
#include <internal/codepage.h>
#include <internal/textattr.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <ncurses.h> // For COLORS

#define CSI "\x1B["

AnsiDisplayBase::AnsiDisplayBase() :
    lastAttr('\x07')
{
}

AnsiDisplayBase::~AnsiDisplayBase()
{
    clearAttributes();
    clearScreen();
    lowlevelFlush();
}

void AnsiDisplayBase::clearAttributes()
{
    buf << CSI"0m";
    lastAttr = '\x07';
}

void AnsiDisplayBase::clearScreen()
{
    buf << CSI"2J";
}

ushort AnsiDisplayBase::getScreenMode()
{
    return TDisplay::smCO80;
}

void AnsiDisplayBase::getCaretPosition(int &x, int &y)
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

void AnsiDisplayBase::getScreenSize(int &rows, int &cols)
{
    lowlevelFlush();
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    rows = w.ws_row;
    cols = w.ws_col;
}

int AnsiDisplayBase::getScreenRows()
{
    int rows, _;
    getScreenSize(rows, _);
    return rows;
}

int AnsiDisplayBase::getScreenCols()
{
    int cols, _;
    getScreenSize(_, cols);
    return cols;
}

void AnsiDisplayBase::lowlevelWriteChar(uchar character, ushort attr)
{
    writeAttributes(attr);
    buf << CpTranslator::toUtf8(character);
}

void AnsiDisplayBase::lowlevelMoveCursor(int x, int y)
{
    buf << CSI << y+1 << ";" << x+1 << "H";
}

void AnsiDisplayBase::lowlevelFlush() {
    fputs(buf.str().c_str(), stdout);
    fflush(stdout);
    std::stringstream().swap(buf);
}

void AnsiDisplayBase::writeAttributes(uchar attr) {
    if (attr != lastAttr)
    {
        BIOSColor c = attr & (COLORS < 16 ? 0x77 : 0xFF);
        c.swapRedBlue();
        buf << CSI
            << getANSIColorCode(c.layers.fg)
            << ';'
            << getANSIColorCode(c.layers.bg, true)
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

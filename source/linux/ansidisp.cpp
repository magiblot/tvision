#define Uses_TScreen
#include <tvision/tv.h>
#include <internal/ansidisp.h>
#include <internal/codepage.h>
#include <internal/textattr.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>
#include <ncurses.h> // For COLORS

using namespace std::literals;

#define CSI "\x1B["

AnsiDisplayBase::AnsiDisplayBase() :
    lastAttr('\x07')
{
}

AnsiDisplayBase::~AnsiDisplayBase()
{
    clearAttributes();
    lowlevelFlush();
}

void AnsiDisplayBase::bufWrite(std::string_view s)
{
    buf.insert(buf.end(), s.data(), s.data()+s.size());
}

void AnsiDisplayBase::bufWriteSeq(uint a, uint b, char F)
{
    // CSI a ; b F
    char s[32] = CSI;
    char *p = s + sizeof(CSI) - 1;
    p += fast_utoa(a, p);
    *p++ = ';';
    p += fast_utoa(b, p);
    *p++ = F;
    bufWrite({s, size_t(p - s)});
}

void AnsiDisplayBase::clearAttributes()
{
    bufWrite(CSI "0m"sv);
    lastAttr = '\x07';
}

void AnsiDisplayBase::clearScreen()
{
    bufWrite(CSI "2J"sv);
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
    ::write(1, CSI "6n", sizeof(CSI "6n") - 1);
    fscanf(stdin, CSI "%d;%dR", &y, &x);
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
    bufWrite(CpTranslator::toUtf8(character));
}

void AnsiDisplayBase::lowlevelMoveCursor(uint x, uint y)
{
    bufWriteSeq(y + 1, x + 1, 'H');
}

void AnsiDisplayBase::lowlevelFlush() {
    ::write(1, buf.data(), buf.size());
    fflush(stdout);
    buf.resize(0);
}

void AnsiDisplayBase::writeAttributes(uchar attr) {
    if (attr != lastAttr)
    {
        BIOSColor c = attr & (COLORS < 16 ? 0x77 : 0xFF);
        c.swapRedBlue();
        uint fg = getANSIColorCode(c.layers.fg),
             bg = getANSIColorCode(c.layers.bg, true);
        bufWriteSeq(fg, bg, 'm');
        if (COLORS < 16)
        {
            if ((attr & 0x08) && !(lastAttr & 0x08))
                bufWrite(CSI "1m"sv);
            else if (!(attr & 0x08) && (lastAttr & 0x08))
                bufWrite(CSI "22m"sv);
        }
        lastAttr = attr;
    }
}

#define Uses_TScreen
#define Uses_THardwareInfo
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
    lastAttr(),
    sgrFlags(0)
{
    if (THardwareInfo::isLinuxConsole(0) || THardwareInfo::isLinuxConsole(1))
        sgrFlags |= sgrBrightIsBlink;
    if (COLORS < 16)
        sgrFlags |= sgrBrightIsBold;
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

void AnsiDisplayBase::bufWriteCSI1(uint a, char F)
{
    // CSI a F
    char s[32] = CSI;
    char *p = s + sizeof(CSI) - 1;
    p += fast_utoa(a, p);
    *p++ = F;
    bufWrite({s, size_t(p - s)});
}

void AnsiDisplayBase::bufWriteCSI2(uint a, uint b, char F)
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
    lastAttr = {};
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

void AnsiDisplayBase::lowlevelWriteChars(const uchar chars[4], TCellAttribs attr)
{
    writeAttributes(attr);
    uint i = 0;
    while (++i < 4 && chars[i]);
    bufWrite({(const char *) chars, i});
}

void AnsiDisplayBase::lowlevelMoveCursorX(uint x, uint)
{
    // Optimized case where the cursor only moves horizontally.
    bufWriteCSI1(x + 1, 'G');
}

void AnsiDisplayBase::lowlevelMoveCursor(uint x, uint y)
{
    bufWriteCSI2(y + 1, x + 1, 'H');
}

void AnsiDisplayBase::lowlevelFlush() {
    ::write(1, buf.data(), buf.size());
    fflush(stdout);
    buf.resize(0);
}

void AnsiDisplayBase::writeAttributes(TCellAttribs c) {
    SGRAttribs sgr {c, sgrFlags};
    SGRAttribs last = lastAttr;
    if (sgr != lastAttr)
    {
        char s[32] = CSI;
        char *p = s + sizeof(CSI) - 1;
        if (sgr.attr.fg != last.attr.fg)
        {
            p += fast_utoa(sgr.attr.fg, p);
            *p++ = ';';
        }
        if (sgr.attr.bg != last.attr.bg)
        {
            p += fast_utoa(sgr.attr.bg, p);
            *p++ = ';';
        }
        if (sgr.attr.bold != last.attr.bold)
        {
            p += fast_utoa(sgr.attr.bold, p);
            *p++ = ';';
        }
        if (sgr.attr.blink != last.attr.blink)
        {
            p += fast_utoa(sgr.attr.blink, p);
            *p++ = ';';
        }
        *(p - 1) = 'm';
        lastAttr = sgr;
        bufWrite({s, size_t(p - s)});
    }
}

#include <internal/ansidisp.h>
#include <internal/textattr.h>
#include <internal/terminal.h>
#include <internal/strings.h>

#define CSI "\x1B["

AnsiDisplayBase::AnsiDisplayBase() :
    lastAttr(SGRAttribs::defaultInit)
{
}

AnsiDisplayBase::~AnsiDisplayBase()
{
    clearAttributes();
    lowlevelFlush();
}

void AnsiDisplayBase::bufWrite(TStringView s)
{
    buf.insert(buf.end(), s.data(), s.data()+s.size());
}

void AnsiDisplayBase::bufWriteCSI1(uint a, char F)
{
    using namespace detail;
    // CSI a F
    char s[32] = CSI;
    char *p = s + sizeof(CSI) - 1;
    p += fast_utoa(a, p);
    *p++ = F;
    bufWrite({s, size_t(p - s)});
}

void AnsiDisplayBase::bufWriteCSI2(uint a, uint b, char F)
{
    using namespace detail;
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
    bufWrite(CSI "0m");
    lastAttr = {};
}

void AnsiDisplayBase::clearScreen()
{
    bufWrite(CSI "2J");
}

void AnsiDisplayBase::lowlevelWriteChars( TStringView chars, TCellAttribs attr,
                                          const TermCap &termcap )
{
    writeAttributes(attr, termcap);
    bufWrite(chars);
}

void AnsiDisplayBase::lowlevelMoveCursorX(uint x, uint)
{
    // Optimized case where the cursor only moves horizontally.
    bufWriteCSI1(x + 1, 'G');
}

void AnsiDisplayBase::lowlevelMoveCursor(uint x, uint y)
{
    // Make dumps readable.
//     bufWrite("\r");
    bufWriteCSI2(y + 1, x + 1, 'H');
}

void AnsiDisplayBase::lowlevelFlush() {
    TermIO::consoleWrite(buf.data(), buf.size());
    buf.resize(0);
}

void AnsiDisplayBase::writeAttributes(TCellAttribs c, const TermCap &termcap) {
    using namespace detail;
    SGRAttribs sgr {c, termcap.quirks};
    SGRAttribs last = lastAttr;
    if (sgr != lastAttr)
    {
        char s[64] = CSI;
        char *p = s + sizeof(CSI) - 1;
        if (sgr.fg != last.fg)
        {
            p += fast_utoa(sgr.fg, p);
            *p++ = ';';
        }
        if (sgr.bg != last.bg)
        {
            p += fast_utoa(sgr.bg, p);
            *p++ = ';';
        }
        if (sgr.bold != last.bold)
        {
            p += fast_utoa(sgr.bold, p);
            *p++ = ';';
        }
        if (sgr.blink != last.blink)
        {
            p += fast_utoa(sgr.blink, p);
            *p++ = ';';
        }
        if (sgr.italic != last.italic)
        {
            p += fast_utoa(sgr.italic, p);
            *p++ = ';';
        }
        if (sgr.underline != last.underline)
        {
            p += fast_utoa(sgr.underline, p);
            *p++ = ';';
        }
        if (sgr.reverse != last.reverse)
        {
            p += fast_utoa(sgr.reverse, p);
            *p++ = ';';
        }
        *(p - 1) = 'm';
        lastAttr = sgr;
        bufWrite({s, size_t(p - s)});
    }
}

#include <internal/ansidisp.h>
#include <internal/terminal.h>
#include <internal/strings.h>

#define CSI "\x1B["

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

namespace ansidisp
{

static size_t convertAttributes(const TColorAttr &, TermAttr &, const TermCap &, char*);

}

void AnsiDisplayBase::lowlevelWriteChars( TStringView chars, TColorAttr attr,
                                          const TermCap &termcap )
{
    using namespace ansidisp;
    char s[256];
    size_t l = convertAttributes(attr, lastAttr, termcap, s);
    bufWrite({s, l});
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

//////////////////////////////////////////////////////////////////////////
// Attribute conversion

namespace ansidisp
{

static void convertColor(TColorDesired, TermColor &, attrstyle_t &, const TermCap &, bool);
static size_t writeAttributes(const TermAttr &, const TermAttr &, char *buf);
static size_t writeColor(TermColor, bool, char * const);

static inline size_t convertAttributes( const TColorAttr &c, TermAttr &lastAttr,
                                        const TermCap &termcap, char *buf )
{
    using namespace ansidisp;
    TermAttr attr {};
    attr.style = ::getStyle(c);

    convertColor(::getFore(c), attr.fg, attr.style, termcap, true);
    convertColor(::getBack(c), attr.bg, attr.style, termcap, false);

    if (termcap.quirks & qfNoItalic)
        attr.style &= ~slItalic;
    if (termcap.quirks & qfNoUnderline)
        attr.style &= ~slUnderline;

    size_t length = writeAttributes(attr, lastAttr, buf);

    lastAttr = attr;

    return length;
}

// colorconv_r: return value of the color conversion funcions.

struct alignas(8) colorconv_r
{
    TermColor color;
    attrstyle_t extraStyle;
    uint8_t unused[2];

    colorconv_r() = default;
    colorconv_r(TermColor aColor, attrstyle_t aExtraFlags=0)
    {
        uint64_t val = aColor | (uint64_t(aExtraFlags) << 32);
        memcpy(this, &val, 8);
        static_assert(sizeof(*this) == 8, "");
    }
};

static colorconv_r convertNoColor(TColorDesired, const TermCap &, bool);
static colorconv_r convertIndexed8(TColorDesired color, const TermCap &, bool);
static colorconv_r convertIndexed16(TColorDesired, const TermCap &, bool);
static colorconv_r convertIndexed256(TColorDesired, const TermCap &, bool);
static colorconv_r convertDirect(TColorDesired, const TermCap &, bool);

static constexpr decltype(&convertNoColor) colorConverters[TermCapColorCount] =
{
    &convertNoColor,
    &convertIndexed8,
    &convertIndexed16,
    &convertIndexed256,
    &convertDirect,
};

static inline void convertColor( TColorDesired c,
                                 TermColor &resultColor, attrstyle_t &resultStyle,
                                 const TermCap &termcap, bool isFg )
{
    auto cnv = (*colorConverters[termcap.colors])(c, termcap, isFg);
    resultColor = cnv.color;
    resultStyle |= cnv.extraStyle;
}

static inline void push(char *&p, TStringView text)
{
    memcpy(p, text.data(), text.size());
    p += text.size();
}

static inline void push(char *&p, bool b, const char *s1, const char *s2)
{
    // Use the ternary on the invocation and not on the argument because
    // some compilers are unable to optimize out strlen and memcpy otherwise.
    b ? push(p, s1) : push(p, s2);
}

static inline void writeFlag( char *&p, TermAttr attr, TermAttr lastAttr,
                              ushort mask, const char * const OnOff[2] )
{
    if ((attr.style & mask) != (lastAttr.style & mask))
    {
        push(p, attr.style & mask, OnOff[0], OnOff[1]);
        push(p, ";");
    }
}

typedef const char *c_str;

static constexpr c_str
    boldOnOff[2] =      { "1", "22"},
    italicOnOff[2] =    { "3", "23"},
    underlineOnOff[2] = { "4", "24"},
    blinkOnOff[2] =     { "5", "25"},
    reverseOnOff[2] =   { "7", "27"},
    strikeOnOff[2] =    { "9", "29"};

static inline size_t writeAttributes( const TermAttr &attr,
                                      const TermAttr &lastAttr, char *buf )
{
    TStringView header = CSI;
    char *p = buf;
    push(p, header);
    char *w = buf,
         *h = p;
    // INVARIANT:
    // h points after the last CSI.
    // w points after the last character that should be printed.
    // p points after the last character written into buf.

    writeFlag(p, attr, lastAttr, slBold, boldOnOff);
    writeFlag(p, attr, lastAttr, slItalic, italicOnOff);
    writeFlag(p, attr, lastAttr, slUnderline, underlineOnOff);
    writeFlag(p, attr, lastAttr, slBlink, blinkOnOff);
    writeFlag(p, attr, lastAttr, slReverse, reverseOnOff);
    writeFlag(p, attr, lastAttr, slStrike, strikeOnOff);

    if (attr.fg != lastAttr.fg)
        p += writeColor(attr.fg, true, p);
    if (attr.bg != lastAttr.bg)
        p += writeColor(attr.bg, false, p);

    if (p != h)
    {
        *(p - 1) = 'm';
        w = p;
    }

    return w - buf;
}

static size_t writeColor(TermColor color, bool isFg, char * const s)
{
    using namespace detail;
    char *p = s;
    switch (color.type)
    {
        case TermColor::Default:
            push(p, isFg, "39;", "49;");
            break;
        case TermColor::Indexed:
            if (color.idx >= 16)
            {
                // <38,48>;5;i;
                push(p, isFg, "38;5;", "48;5;");
                p += fast_btoa(color.idx, p);
            }
            else if (color.idx >= 8)
                // <90-97,100-107>;
                p += fast_btoa(color.idx - 8 + (isFg ? 90 : 100), p);
            else
                // <30-37,40-47>;
                p += fast_btoa(color.idx + (isFg ? 30 : 40), p);
            push(p, ";");
            break;
        case TermColor::RGB:
            // <38,48>;2;r;g;b;
            push(p, isFg, "38;2;", "48;2;");
            for (int i = 2; i >= 0; --i)
            {
                p += fast_btoa(color.bgr[i], p);
                push(p, ";");
            }
            break;
        case TermColor::NoColor:
            break;
    }
    return p - s;
}

// Color conversion functions

static colorconv_r convertNoColor(TColorDesired color, const TermCap &, bool isFg)
{
    colorconv_r cnv {{TermColor::NoColor}};
    // Mimic the mono palettes with styles.
    if (color.isBIOS())
    {
        auto bios = color.asBIOS();
        if (isFg)
        {
            if (bios & 0x8)
                cnv.extraStyle |= slBold;
            else if (bios == 0x1)
                cnv.extraStyle |= slUnderline;
        }
        else if ((bios & 0x7) == 0x7)
            cnv.extraStyle |= slReverse;
    }
    return cnv;
}

static colorconv_r convertIndexed8( TColorDesired color,
                                    const TermCap &termcap, bool isFg )
{
    auto cnv = convertIndexed16(color, termcap, isFg);
    if ( cnv.color.type == TermColor::Indexed &&
         cnv.color.idx >= 8 )
    {
        cnv.color.idx -= 8;
        if (isFg)
        {
            if (termcap.quirks & qfBoldIsBright)
                cnv.extraStyle |= slBold;
        }
        else
        {
            if (termcap.quirks & qfBlinkIsBright)
                cnv.extraStyle |= slBlink;
        }
    }
    return cnv;
}

static colorconv_r convertIndexed16( TColorDesired color,
                                     const TermCap &, bool )
{
    if (color.isBIOS())
    {
        uint8_t idx = BIOStoXTerm16(color.asBIOS());
        return {{idx, TermColor::Indexed}};
    }
    else if (color.isXTerm())
    {
        uint8_t idx = color.asXTerm();
        if (idx >= 16)
            idx = XTerm256toXTerm16(idx);
        return {{idx, TermColor::Indexed}};
    }
    else if (color.isRGB())
    {
        uint8_t idx = RGBtoXTerm16(color.asRGB());
        return {{idx, TermColor::Indexed}};
    }
    return {{TermColor::Default}};
}

static colorconv_r convertIndexed256( TColorDesired color,
                                      const TermCap &termcap, bool isFg )
{
    if (color.isXTerm())
    {
        uint8_t idx = color.asXTerm();
        return {{idx, TermColor::Indexed}};
    }
    else if (color.isRGB())
    {
        uint8_t idx = RGBtoXTerm256(color.asRGB());
        return {{idx, TermColor::Indexed}};
    }
    return convertIndexed16(color, termcap, isFg);
}

static colorconv_r convertDirect( TColorDesired color,
                                  const TermCap &termcap, bool isFg )
{
    if (color.isRGB())
    {
        auto rgb = color.asRGB();
        return {{rgb, TermColor::RGB}};
    }
    return convertIndexed256(color, termcap, isFg);
}

} // namespace ansidisp

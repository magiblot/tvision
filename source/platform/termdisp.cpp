#define Uses_TScreen
#include <tvision/tv.h>

#include <internal/termdisp.h>
#include <internal/linuxcon.h>
#include <internal/getenv.h>

TermCap TerminalDisplay::getCapabilities() noexcept
{
    TermCap termcap {};
    auto colorterm = getEnv<TStringView>("COLORTERM");
    if (colorterm == "truecolor" || colorterm == "24bit")
        termcap.colors = Direct;
    else
    {
        int colors = getColorCount();
        if (colors >= 256*256*256)
            termcap.colors = Direct;
        else if (colors >= 256)
            termcap.colors = Indexed256;
        else if (colors >= 16)
            termcap.colors = Indexed16;
        else if (colors >= 8)
        {
            termcap.colors = Indexed8;
            termcap.quirks |= qfBoldIsBright;
#ifdef __linux__
            if (io.isLinuxConsole())
                termcap.quirks |= qfBlinkIsBright | qfNoItalic | qfNoUnderline;
            else
#endif // __linux__
            if (getEnv<TStringView>("TERM") == "xterm")
                // Let's assume all terminals disguising themselves as 'xterm'
                // support at least 16 colors.
                termcap.colors = Indexed16;
        }
    }
    return termcap;
}

ushort TerminalDisplay::getScreenMode() noexcept
{
    if (termcap.colors == NoColor)
        return TDisplay::smMono;
    else
    {
        ushort mode = TDisplay::smCO80;
        if (termcap.colors == Direct)
            mode |= TDisplay::smColor256 | TDisplay::smColorHigh;
        else if (termcap.colors == Indexed256)
            mode |= TDisplay::smColor256;
        return mode;
    }
}

TPoint TerminalDisplay::actualScreenSize() noexcept
{
    return io.getSize();
}

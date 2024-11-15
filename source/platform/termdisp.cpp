#include <internal/termdisp.h>
#include <internal/conctl.h>
#include <internal/getenv.h>

namespace tvision
{

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
            if (con.isLinuxConsole())
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

} // namespace tvision

#include <internal/termdisp.h>
#include <internal/terminal.h>
#include <internal/getenv.h>

void TerminalDisplay::reloadScreenInfo()
{
    BufferedDisplay::reloadScreenInfo();
    termcap = getCapabilities();
}

int TerminalDisplay::getColorCount()
{
    return 0;
}

TermCap TerminalDisplay::getCapabilities()
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
            if (TermIO::isLinuxConsole())
                termcap.quirks |= qfBlinkIsBright | qfNoItalic | qfNoUnderline;
        }
    }
    return termcap;
}


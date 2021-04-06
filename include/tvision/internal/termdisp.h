#ifndef TVISION_TERMDISP_H
#define TVISION_TERMDISP_H

#include <internal/buffdisp.h>

// Terminal quirk flags.

const uint
    qfBoldIsBright  = 0x0001,
    qfBlinkIsBright = 0x0002,
    qfNoItalic      = 0x0004,
    qfNoUnderline   = 0x0008;

enum TermCapColors : uint8_t
{
    NoColor,
    Indexed8,
    Indexed16,
    Indexed256,
    Direct,
    TermCapColorCount,
};

struct TermCap
{
    TermCapColors colors;
    uint quirks;
};

// TerminalDisplay is a BufferedDisplay with knowledge of terminal capabilities.

class TerminalDisplay : public BufferedDisplay
{

protected:

    TermCap termcap {};

    void reloadScreenInfo() override;
    virtual int getColorCount();
    virtual TermCap getCapabilities();

    ushort getScreenMode() override;

};

#endif // TVISION_TERMDISP_H

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
public:
    const StdioCtl &io;

    // The lifetime of 'aIo' exceeds that of 'this'.
    TerminalDisplay(const StdioCtl &aIo) noexcept :
        io(aIo)
    {
    }

    TermCap termcap {};

    void reloadScreenInfo() noexcept override;
    virtual int getColorCount() noexcept;
    virtual TermCap getCapabilities() noexcept;

    ushort getScreenMode() noexcept override;
};

#endif // TVISION_TERMDISP_H

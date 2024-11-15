#ifndef TVISION_TERMDISP_H
#define TVISION_TERMDISP_H

#include <internal/platform.h>

namespace tvision
{

class ConsoleCtl;

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

// TerminalDisplay is a DisplayAdapter with knowledge of terminal capabilities.

class TerminalDisplay : public DisplayAdapter
{
    TermCap getCapabilities() noexcept;

protected:

    ConsoleCtl &con;
    TermCap termcap;

    // The subclass must invoke this in the constructor.
    void initCapabilities() noexcept
    {
        termcap = getCapabilities();
    }

public:

    // The lifetime of 'aCon' exceeds that of 'this'.
    TerminalDisplay(ConsoleCtl &aCon) noexcept :
        con(aCon)
    {
    }
};

} // namespace tvision

#endif // TVISION_TERMDISP_H

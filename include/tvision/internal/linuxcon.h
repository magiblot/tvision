#ifndef TVISION_LINUXCON_H
#define TVISION_LINUXCON_H

#ifdef __linux__

#include <internal/platform.h>

struct TEvent;

namespace tvision
{

class ScreenLifetime;
class SigwinchHandler;
class GpmInput;

struct LinuxConsoleInput final : public EventSource
{
    const StdioCtl &io;
    InputStrategy &input;

    LinuxConsoleInput(const StdioCtl &aIo, InputStrategy &aInput) noexcept :
        EventSource(aInput.handle),
        io(aIo),
        input(aInput)
    {
    }

    bool getEvent(TEvent &ev) noexcept override;
    bool hasPendingEvents() noexcept override;

    static ushort getKeyboardModifiers(const StdioCtl &io) noexcept;
};

class LinuxConsoleStrategy : public ConsoleStrategy
{
    ScreenLifetime &scrl;
    SigwinchHandler *sigwinch;
    LinuxConsoleInput &wrapper;
    GpmInput *gpm;

    LinuxConsoleStrategy( DisplayStrategy &aDisplay, ScreenLifetime &aScrl,
                          SigwinchHandler *aSigwinch, LinuxConsoleInput &aWrapper,
                          GpmInput *aGpm ) noexcept;

public:

    // Pre: 'io.isLinuxConsole()' returns 'true'.
    // The lifetime of 'io' must exceed that of the returned object.
    // Takes ownership over 'scrl', 'display' and 'input'.
    static LinuxConsoleStrategy &create( const StdioCtl &io, ScreenLifetime &scrl,
                                         DisplayStrategy &display,
                                         InputStrategy &input ) noexcept;
    ~LinuxConsoleStrategy();

    static int charWidth(uint32_t) noexcept;
};

} // namespace tvision

#endif // __linux__

#endif // TVISION_LINUXCON_H

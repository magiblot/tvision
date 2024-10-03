#ifndef TVISION_LINUXCON_H
#define TVISION_LINUXCON_H

#ifdef __linux__

#include <internal/platform.h>
#include <internal/errredir.h>

struct TEvent;

namespace tvision
{

class SigwinchHandler;
class GpmInput;
struct InputState;

struct LinuxConsoleInput final : public EventSource
{
    StdioCtl &io;
    InputStrategy &input;

    LinuxConsoleInput(StdioCtl &aIo, InputStrategy &aInput) noexcept :
        EventSource(aInput.handle),
        io(aIo),
        input(aInput)
    {
    }

    bool getEvent(TEvent &ev) noexcept override;
    bool hasPendingEvents() noexcept override;

    ushort getKeyboardModifiers() noexcept;

    static ushort convertLinuxKeyModifiers(ushort linuxShiftState) noexcept;
};

class LinuxConsoleStrategy : public ConsoleStrategy
{
    StderrRedirector errRedir;

    InputState &inputState;
    SigwinchHandler *sigwinch;
    LinuxConsoleInput &wrapper;
    GpmInput *gpm;

    LinuxConsoleStrategy( DisplayStrategy &, LinuxConsoleInput &,
                          InputState &, SigwinchHandler *,
                          GpmInput * ) noexcept;

public:

    // Pre: 'io.isLinuxConsole()' returns 'true'.
    // The lifetime of 'io' and 'displayBuf' must exceed that of the returned object.
    // Takes ownership over 'inputState', 'display' and 'input'.
    static LinuxConsoleStrategy &create( StdioCtl &io,
                                         DisplayBuffer &displayBuf,
                                         InputState &inputState,
                                         DisplayStrategy &display,
                                         InputStrategy &input ) noexcept;
    ~LinuxConsoleStrategy();

    static int charWidth(uint32_t) noexcept;
};

} // namespace tvision

#endif // __linux__

#endif // TVISION_LINUXCON_H

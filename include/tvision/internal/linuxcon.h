#ifndef TVISION_LINUXCON_H
#define TVISION_LINUXCON_H

#ifdef __linux__

#include <internal/platform.h>
#include <internal/errredir.h>

struct TEvent;

namespace tvision
{

class ConsoleCtl;
class SigwinchHandler;
class GpmInput;
struct InputState;

struct LinuxConsoleInput final : public InputAdapter
{
    ConsoleCtl &con;
    InputAdapter &input;

    LinuxConsoleInput(ConsoleCtl &aCon, InputAdapter &aInput) noexcept :
        InputAdapter(aInput.handle),
        con(aCon),
        input(aInput)
    {
    }

    bool getEvent(TEvent &ev) noexcept override;
    bool hasPendingEvents() noexcept override;

    ushort getKeyboardModifiers() noexcept;

    static ushort convertLinuxKeyModifiers(ushort linuxShiftState) noexcept;
};

class LinuxConsoleAdapter final : public ConsoleAdapter
{
    StderrRedirector errRedir;

    InputState &inputState;
    SigwinchHandler *sigwinch;
    LinuxConsoleInput &wrapper;
    GpmInput *gpm;

    LinuxConsoleAdapter( DisplayAdapter &, LinuxConsoleInput &,
                         InputState &, SigwinchHandler *,
                         GpmInput * ) noexcept;

public:

    // Pre: 'io.isLinuxConsole()' returns 'true'.
    // The lifetime of 'con' and 'displayBuf' must exceed that of the returned object.
    // Takes ownership over 'inputState', 'display' and 'input'.
    static LinuxConsoleAdapter &create( ConsoleCtl &con,
                                        DisplayBuffer &displayBuf,
                                        InputState &inputState,
                                        DisplayAdapter &display,
                                        InputAdapter &input ) noexcept;
    ~LinuxConsoleAdapter();

    static int charWidth(uint32_t) noexcept;
};

} // namespace tvision

#endif // __linux__

#endif // TVISION_LINUXCON_H

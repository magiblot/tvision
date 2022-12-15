#ifndef TVISION_UNIXCON_H
#define TVISION_UNIXCON_H

#include <internal/platform.h>
#ifdef _TV_UNIX

namespace tvision
{

class ScreenLifetime;
class SigwinchHandler;

class UnixConsoleStrategy : public ConsoleStrategy
{
    ScreenLifetime &scrl;
    SigwinchHandler *sigwinch;

    UnixConsoleStrategy( DisplayStrategy &, InputStrategy &,
                         ScreenLifetime &, SigwinchHandler * ) noexcept;

public:

    // Takes ownership over 'scrl', 'display' and 'input'.
    static UnixConsoleStrategy &create( ScreenLifetime &scrl,
                                        DisplayStrategy &display,
                                        InputStrategy &input ) noexcept;

    ~UnixConsoleStrategy();

    static int charWidth(uint32_t) noexcept;
};

} // namespace tvision

#endif // _TV_UNIX

#endif // TVISION_UNIXCON_H

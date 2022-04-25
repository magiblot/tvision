#ifndef TVISION_UNIXCON_H
#define TVISION_UNIXCON_H

#include <internal/platform.h>
#ifdef _TV_UNIX

#include <internal/sigwinch.h>

namespace tvision
{

class ScreenLifetime;

class UnixConsoleStrategy : public ConsoleStrategy
{
    ScreenLifetime &scrl;
    SigwinchHandler *sigwinch;

    void forEachSource(void *args, void (&action)(void *, EventSource &)) noexcept override;

protected:

    void forEachPrivateSource(void *args, void (&action)(void *, EventSource &)) noexcept
    {
        if (sigwinch)
            action(args, sigwinch->getEventSource());
    }

public:

    // Takes ownership over 'aDisplay' and 'aInput'.
    UnixConsoleStrategy(ScreenLifetime &aScrl, DisplayStrategy &aDisplay, InputStrategy &aInput) noexcept :
        ConsoleStrategy(aDisplay, aInput),
        scrl(aScrl),
        sigwinch(SigwinchHandler::create())
    {
    }

    // Deletes 'display' and 'input'.
    ~UnixConsoleStrategy();

    static int charWidth(uint32_t) noexcept;
};

} // namespace tvision

#endif // _TV_UNIX

#endif // TVISION_UNIXCON_H

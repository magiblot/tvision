#ifndef TVISION_UNIXCON_H
#define TVISION_UNIXCON_H

#include <internal/platform.h>
#ifdef _TV_UNIX

#include <internal/sigwinch.h>

class UnixConsoleStrategy : public ConsoleStrategy
{
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
    UnixConsoleStrategy(DisplayStrategy &aDisplay, InputStrategy &aInput) noexcept :
        ConsoleStrategy(aDisplay, aInput),
        sigwinch(SigwinchHandler::create())
    {
    }

    // Deletes 'display' and 'input'.
    ~UnixConsoleStrategy();

    static int charWidth(uint32_t) noexcept;
};

#endif // _TV_UNIX

#endif // TVISION_UNIXCON_H

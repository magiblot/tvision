#include <internal/unixcon.h>

#ifdef _TV_UNIX

#include <internal/scrlife.h>
#include <internal/sigwinch.h>

namespace tvision
{

inline UnixConsoleStrategy::UnixConsoleStrategy( DisplayStrategy &aDisplay,
                                                 InputStrategy &aInput,
                                                 ScreenLifetime &aScrl,
                                                 SigwinchHandler *aSigwinch ) noexcept :
    ConsoleStrategy(aDisplay, aInput, {&aInput, aSigwinch}),
    scrl(aScrl),
    sigwinch(aSigwinch)
{
}

UnixConsoleStrategy &UnixConsoleStrategy::create( ScreenLifetime &scrl,
                                                  DisplayStrategy &display,
                                                  InputStrategy &input ) noexcept
{
    auto *sigwinch = SigwinchHandler::create();
    return *new UnixConsoleStrategy(display, input, scrl, sigwinch);
}

UnixConsoleStrategy::~UnixConsoleStrategy()
{
    delete sigwinch;
    delete &input;
    delete &display;
    delete &scrl;
}

} // namespace tvision

#endif // _TV_UNIX

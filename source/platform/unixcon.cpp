#include <internal/unixcon.h>

#ifdef _TV_UNIX

#include <internal/sigwinch.h>
#include <internal/termio.h>
#include <internal/dispbuff.h>
#include <internal/unixclip.h>

namespace tvision
{

inline UnixConsoleAdapter::UnixConsoleAdapter( DisplayAdapter &aDisplay,
                                               InputAdapter &aInput,
                                               ConsoleCtl &aCon,
                                               DisplayBuffer &aDisplayBuf,
                                               InputState &aInputState,
                                               SigwinchHandler *aSigwinch ) noexcept :
    ConsoleAdapter(aDisplay, {&aInput, aSigwinch}),
    con(aCon),
    displayBuf(aDisplayBuf),
    inputState(aInputState),
    sigwinch(aSigwinch),
    input(aInput)
{
    // Ensure we don't miss a possible undetected screen size change (e.g. after
    // recovering from SIGTSTP).
    if (sigwinch)
        sigwinch->signal();
}

UnixConsoleAdapter &UnixConsoleAdapter::create( ConsoleCtl &con,
                                                DisplayBuffer &displayBuf,
                                                InputState &inputState,
                                                DisplayAdapter &display,
                                                InputAdapter &input ) noexcept
{
    auto *sigwinch = SigwinchHandler::create();
    return *new UnixConsoleAdapter(display, input, con, displayBuf, inputState, sigwinch);
}

UnixConsoleAdapter::~UnixConsoleAdapter()
{
    delete sigwinch;
    delete &input;
    delete &display;
    delete &inputState;
}

bool UnixConsoleAdapter::setClipboardText(TStringView text) noexcept
{
    if (UnixClipboard::setClipboardText(text))
        return true;
    if (TermIO::setClipboardText(con, text, inputState))
        return true;
    // On non-success, 'setClipboardText' prints an OSC sequence not all
    // terminals can handle; redraw the screen in case it has been messed up.
    displayBuf.redrawScreen(display);
    return false;
}

bool UnixConsoleAdapter::requestClipboardText(void (&accept)(TStringView)) noexcept
{
    if (UnixClipboard::requestClipboardText(accept))
        return true;
    if (TermIO::requestClipboardText(con, accept, inputState))
        return true;
    return false;
}

} // namespace tvision

#endif // _TV_UNIX

#ifndef TVISION_UNIXCON_H
#define TVISION_UNIXCON_H

#include <internal/platform.h>
#ifdef _TV_UNIX

namespace tvision
{

class ScreenLifetime;
class SigwinchHandler;
struct InputState;
class BufferedDisplay;

class UnixConsoleStrategy : public ConsoleStrategy
{
    const StdioCtl &io;
    BufferedDisplay &displayBuf;
    ScreenLifetime &scrl;
    InputState &inputState;
    SigwinchHandler *sigwinch;

    UnixConsoleStrategy( DisplayStrategy &, InputStrategy &, const StdioCtl &,
                         BufferedDisplay &, ScreenLifetime &, InputState &,
                         SigwinchHandler * ) noexcept;

public:

    // The lifetime of 'io' and 'displayBuf' must exceed that of the returned object.
    // Takes ownership over 'scrl', 'inputState', 'display' and 'input'.
    static UnixConsoleStrategy &create( const StdioCtl &io,
                                        BufferedDisplay &displayBuf,
                                        ScreenLifetime &scrl,
                                        InputState &inputState,
                                        DisplayStrategy &display,
                                        InputStrategy &input ) noexcept;

    ~UnixConsoleStrategy();

    bool setClipboardText(TStringView) noexcept override;
    bool requestClipboardText(void (&)(TStringView)) noexcept override;

    static int charWidth(uint32_t) noexcept;
};

} // namespace tvision

#endif // _TV_UNIX

#endif // TVISION_UNIXCON_H

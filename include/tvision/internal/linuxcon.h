#ifndef TVISION_LINUXCON_H
#define TVISION_LINUXCON_H

#include <internal/unixcon.h>

#ifdef __linux__

#define Uses_TKeys
#define Uses_TEvent
#include <tvision/tv.h>

class GpmInput;

struct LinuxConsoleInput : public EventSource
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

    static ushort keyCodeWithModifiers(ulong, ushort) noexcept;
    static void applyKeyboardModifiers(const StdioCtl &io, KeyDownEvent &key) noexcept;
};

class LinuxConsoleStrategy : public UnixConsoleStrategy
{
    LinuxConsoleInput wrapper;

    LinuxConsoleStrategy( const StdioCtl &aIo, DisplayStrategy &aDisplay,
                          InputStrategy &aInput, GpmInput *gpm ) noexcept :
        UnixConsoleStrategy(aDisplay, gpm ? (InputStrategy &) *gpm : aInput),
        wrapper(aIo, aInput)
    {
    }

public:

    // Pre: 'isLinuxConsole(io)' returns 'true'.
    // The lifetime of 'io' must exceed that of the returned object.
    // Takes ownership over 'display' and 'input'.
    static LinuxConsoleStrategy &create( const StdioCtl &io,
                                         DisplayStrategy &display,
                                         InputStrategy &input ) noexcept;
    ~LinuxConsoleStrategy();

    void forEachSource(void *, void (&)(void *, EventSource &)) noexcept override;

    static int charWidth(uint32_t) noexcept;
};

#endif // __linux__

#endif // TVISION_LINUXCON_H

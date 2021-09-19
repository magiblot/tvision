#ifndef TVISION_LINUXCON_H
#define TVISION_LINUXCON_H

#include <internal/platform.h>

#ifdef __linux__

#define Uses_TKeys
#define Uses_TEvent
#include <tvision/tv.h>

#include <memory>

class GpmInput;

class LinuxConsoleInput : public EventSource
{
    EventSource &wrapped;

public:

    LinuxConsoleInput(EventSource &src) noexcept :
        EventSource(src.handle),
        wrapped(src)
    {
    }

    bool getEvent(TEvent &ev) noexcept override;
    bool hasPendingEvents() noexcept override;

    static ushort keyCodeWithModifiers(ulong, ushort) noexcept;
    static void applyKeyboardModifiers(KeyDownEvent &key) noexcept;
};

class LinuxConsoleStrategy : public UnixPlatformStrategy
{
    LinuxConsoleInput inputWrap;
    std::unique_ptr<GpmInput> gpm;

public:

    LinuxConsoleStrategy(DisplayStrategy &, InputStrategy &) noexcept;
    ~LinuxConsoleStrategy();

    int getButtonCount() noexcept override;
    int charWidth(TStringView mbc, char32_t wc) noexcept override; // ttext.cpp
};

#endif // __linux__

#endif // TVISION_LINUXCON_H

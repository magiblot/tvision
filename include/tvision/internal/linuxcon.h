#ifndef LINUXCON_H
#define LINUXCON_H

#include <internal/platform.h>

#ifdef __linux__

#define Uses_TKeys
#define Uses_TEvent
#include <tvision/tv.h>

#include <internal/gpminput.h>
#include <memory>

class LinuxConsoleStrategy : public UnixPlatformStrategy {

    std::unique_ptr<GpmInput> gpm;

    bool patchKeyEvent(TEvent &ev) noexcept;
    static ushort keyCodeWithModifiers(ulong, ushort) noexcept;
    static void applyKeyboardModifiers(KeyDownEvent &key) noexcept;

public:

    LinuxConsoleStrategy(DisplayStrategy *, FdInputStrategy *) noexcept;

    int getButtonCount() noexcept override;
    int charWidth(TStringView mbc, char32_t wc) noexcept override; // ttext.cpp

};

#else

class LinuxConsoleStrategy : public PlatformStrategy {
    using PlatformStrategy::PlatformStrategy;
};

#endif // __linux__

#endif // LINUXCON_H

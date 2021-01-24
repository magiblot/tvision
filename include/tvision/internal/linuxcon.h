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

    bool patchKeyEvent(TEvent &ev);
    static ushort keyCodeWithModifiers(ulong, ushort);
    static void applyKeyboardModifiers(KeyDownEvent &key);

public:

    LinuxConsoleStrategy(DisplayStrategy *, FdInputStrategy *);

    int getButtonCount() override;
    int charWidth(TStringView mbc, char32_t wc) override; // ttext.cpp

};

#else

class LinuxConsoleStrategy : public PlatformStrategy {
    using PlatformStrategy::PlatformStrategy;
};

#endif // __linux__

#endif // LINUXCON_H

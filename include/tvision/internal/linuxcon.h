#ifndef LINUXCON_H
#define LINUXCON_H

#include <internal/platform.h>

#ifdef __linux__

#define Uses_TKeys
#define Uses_TEvent
#include <tvision/tv.h>

#include <internal/gpminput.h>
#include <memory>

class LinuxConsoleStrategy : public PlatformStrategy {

    std::unique_ptr<GpmInput> gpm;

    bool patchKeyEvent(TEvent &ev);
    static void applyKeyboardModifiers(KeyDownEvent &key);

public:

    LinuxConsoleStrategy(DisplayStrategy*, FdInputStrategy*);

    int getButtonCount();
    void flushScreen();

};

#else

class LinuxConsoleStrategy : public PlatformStrategy {};

#endif // __linux__

#endif

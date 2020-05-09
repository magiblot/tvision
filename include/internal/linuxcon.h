#ifndef LINUXCON_H
#define LINUXCON_H

#define Uses_TKeys
#define Uses_TEvent
#include <tvision/tv.h>

#include <internal/platform.h>
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

#endif

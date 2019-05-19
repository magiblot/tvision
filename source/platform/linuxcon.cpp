#include <platform.h>
LinuxConsoleStrategy::LinuxConsoleStrategy(DisplayStrategy *d, AsyncInputStrategy *i) :
    PlatformStrategy(d, new GpmInput()), keyboard(i)
{
    if (keyboard) keyboard->startInputThread();
}

LinuxConsoleStrategy::~LinuxConsoleStrategy()
{
    if (keyboard) keyboard->endInputThread();
}


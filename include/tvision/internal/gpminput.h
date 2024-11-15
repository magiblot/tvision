#ifndef TVISION_GPMINPUT_H
#define TVISION_GPMINPUT_H

#include <internal/platform.h>

#ifdef HAVE_GPM

#define Uses_TPoint
#define Uses_TEvent
#include <tvision/tv.h>

#include <gpm.h>

namespace tvision
{

class DisplayBuffer;

class GpmInput final : public InputAdapter
{
    uchar buttonState {0};
    DisplayBuffer &displayBuf;

    GpmInput(DisplayBuffer &) noexcept;
    void fitEvent(Gpm_Event &) noexcept;

public:

    // Pre: The lifetime of 'displayBuf' must exceed that of the returned object.
    static GpmInput *create(DisplayBuffer &displayBuf) noexcept;
    ~GpmInput();

    bool getEvent(TEvent &ev) noexcept override;
};

} // namespace tvision

#else

namespace tvision
{

class GpmInput : public InputAdapter
{
public:
    static GpmInput *create(DisplayBuffer &) noexcept { return nullptr; }
};

} // namespace tvision

#endif // HAVE_GPM

#endif // TVISION_GPMINPUT_H

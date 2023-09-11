#ifndef TVISION_TERMINAL_TEST_H
#define TVISION_TERMINAL_TEST_H

#include <internal/terminal.h>

namespace tvision
{

class StrInputGetter : public InputGetter
{
    TStringView str;
    size_t i {0};

public:

    StrInputGetter(TStringView aStr) noexcept :
        str(aStr)
    {
    }

    int get() noexcept override
    {
        return i < str.size() ? str[i++] : -1;
    }

    void unget(int) noexcept override
    {
        if (i > 0)
            --i;
    }

    int bytesLeft() noexcept
    {
        return str.size() - i;
    }
};

struct ParseResultEvent
{
    ParseResult parseResult;
    TEvent ev;
};

static bool operator==(const ParseResultEvent &a, const ParseResultEvent &b)
{
    if (a.parseResult != b.parseResult)
        return false;
    if (a.parseResult == Ignored)
        return true;
    if (a.ev.what != b.ev.what)
        return false;
    if (a.ev.what == evNothing)
        return true;
    if (a.ev.what == evKeyDown)
        return
            a.ev.keyDown.keyCode == b.ev.keyDown.keyCode &&
            a.ev.keyDown.controlKeyState == b.ev.keyDown.controlKeyState &&
            a.ev.keyDown.getText() == b.ev.keyDown.getText();
    if (a.ev.what == evMouse)
        return
            a.ev.mouse.where == b.ev.mouse.where &&
            a.ev.mouse.eventFlags == b.ev.mouse.eventFlags &&
            a.ev.mouse.controlKeyState == b.ev.mouse.controlKeyState &&
            a.ev.mouse.buttons == b.ev.mouse.buttons &&
            a.ev.mouse.wheel == b.ev.mouse.wheel;
    abort();
}

inline std::ostream &operator<<(std::ostream &os, const ParseResultEvent &p)
{
    os << "{";
    switch (p.parseResult)
    {
        case Rejected: os << "Rejected"; break;
        case Ignored: os << "Ignored"; break;
        case Accepted:
        {
            os << "Accepted, {";
            printEventCode(os, p.ev.what);
            os << ", {";
            if (p.ev.what == evKeyDown)
            {
                os << "{";
                printKeyCode(os, p.ev.keyDown.keyCode);
                os << "}, {";
                printControlKeyState(os, p.ev.keyDown.controlKeyState);
                os << "}, '" << p.ev.keyDown.getText() << "'";
            }
            else if (p.ev.what == evMouse)
            {
                os << "(" << p.ev.mouse.where.x << "," << p.ev.mouse.where.y << ")";
                os << ", ";
                printMouseEventFlags(os, p.ev.mouse.eventFlags);
                os << ", ";
                printControlKeyState(os, p.ev.mouse.controlKeyState);
                os << ", ";
                printMouseButtonState(os, p.ev.mouse.buttons);
                os << ", ";
                printMouseWheelState(os, p.ev.mouse.wheel);
            }
            os << "}}";
        }
    }
    os << "}";
    return os;
}

constexpr TEvent keyDownEv(ushort keyCode, ushort controlKeyState, TStringView text)
{
    TEvent ev {};
    ev.what = evKeyDown;
    ev.keyDown.keyCode = keyCode;
    ev.keyDown.controlKeyState = controlKeyState;
    while (ev.keyDown.textLength <= sizeof(ev.keyDown.text) && ev.keyDown.textLength < text.size())
    {
        ev.keyDown.text[ev.keyDown.textLength] = text[ev.keyDown.textLength];
        ++ev.keyDown.textLength;
    }
    return ev;
}

constexpr TEvent mouseEv(TPoint where, ushort eventFlags, ushort controlKeyState, uchar buttons, uchar wheel)
{
    TEvent ev {};
    ev.what = evMouse;
    ev.mouse.where = where;
    ev.mouse.eventFlags = eventFlags;
    ev.mouse.controlKeyState = controlKeyState;
    ev.mouse.buttons = buttons;
    ev.mouse.wheel = wheel;
    return ev;
}

} // namespace tvision

#endif // TVISION_TERMINAL_TEST_H

#ifndef TVISION_EVENTS_TEST_H
#define TVISION_EVENTS_TEST_H

#define Uses_TEvent
#define Uses_TKeys
#include <tvision/tv.h>

#include <ostream>

inline bool operator==(const TEvent &a, const TEvent &b)
{
    if (a.what != b.what)
        return false;
    if (a.what == evNothing)
        return true;
    if (a.what & evKeyboard)
        return
            a.keyDown.keyCode == b.keyDown.keyCode &&
            a.keyDown.controlKeyState == b.keyDown.controlKeyState &&
            a.keyDown.getText() == b.keyDown.getText();
    if (a.what & evMouse)
        return
            a.mouse.where == b.mouse.where &&
            a.mouse.eventFlags == b.mouse.eventFlags &&
            a.mouse.controlKeyState == b.mouse.controlKeyState &&
            a.mouse.buttons == b.mouse.buttons &&
            a.mouse.wheel == b.mouse.wheel;
    if (a.what & evMessage)
        return
            a.message.command == b.message.command &&
            a.message.infoPtr == b.message.infoPtr;
    abort();
}

inline std::ostream &operator<<(std::ostream &os, const TEvent &ev)
{
    os << "{";
    printEventCode(os, ev.what);
    os << ", ";
    if (ev.what & evKeyboard)
    {
        os << "{";
        printKeyCode(os, ev.keyDown.keyCode);
        os << "}, {";
        printControlKeyState(os, ev.keyDown.controlKeyState);
        os << "}, '" << ev.keyDown.getText() << "'";
    }
    else if (ev.what & evMouse)
    {
        os << "(" << ev.mouse.where.x << "," << ev.mouse.where.y << ")"
           << ", ";
        printMouseEventFlags(os, ev.mouse.eventFlags);
        os << ", ";
        printControlKeyState(os, ev.mouse.controlKeyState);
        os << ", ";
        printMouseButtonState(os, ev.mouse.buttons);
        os << ", ";
        printMouseWheelState(os, ev.mouse.wheel);
    }
    else if (ev.what & evMessage)
    {
        os << ev.message.command
           << ", "
           << ev.message.infoPtr;
    }
    os << "}";
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const TKey &key)
{
    os << "{{";
    printKeyCode(os, key.code);
    os << "}, {";
    printControlKeyState(os, key.mods);
    os << "}}";
    return os;
}

inline TEvent keyDownEv(ushort keyCode, ushort controlKeyState, TStringView text = {})
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

inline TEvent mouseEv(TPoint where, ushort eventFlags, ushort controlKeyState, uchar buttons, uchar wheel)
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

inline TEvent messageEv(ushort what, ushort command, void *infoPtr = nullptr)
{
    TEvent ev {};
    ev.what = what;
    ev.message.command = command;
    ev.message.infoPtr = infoPtr;
    return ev;
}

#endif // TVISION_EVENTS_H

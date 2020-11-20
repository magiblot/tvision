#define Uses_THardwareInfo
#include <tvision/tv.h>

#include <internal/terminal.h>

// Mouse support in Ncurses is not always good. To work around some issues,
// we request and parse mouse events manually.

void TermIO::mouseOn()
{
    TStringView seq = "\x1B[?1001s" // Save old highlight mouse reporting.
                      "\x1B[?1000h" // Enable mouse reporting.
                      "\x1B[?1002h" // Enable mouse drag reporting.
                      "\x1B[?1006h" // Enable SGR extended mouse reporting.
                    ;
    THardwareInfo::consoleWrite(seq.data(), seq.size());
}

void TermIO::mouseOff()
{
    TStringView seq = "\x1B[?1006l" // Disable SGR extended mouse reporting.
                      "\x1B[?1002l" // Disable mouse drag reporting.
                      "\x1B[?1000l" // Disable mouse reporting.
                      "\x1B[?1001r" // Restore old highlight mouse reporting.
                    ;
    THardwareInfo::consoleWrite(seq.data(), seq.size());
}

bool TermIO::acceptMouseEvent(TEvent &ev, MouseState &oldm, const MouseState &newm)
{
    // Some terminal emulators send a mouse event every pixel the graphical
    // mouse cursor moves over the window. Filter out those unnecessary
    // events.
    if (newm.buttons != oldm.buttons || newm.wheel || newm.where != oldm.where)
    {
        ev.what = evMouse;
        ev.mouse = {};
        ev.mouse.buttons = newm.buttons;
        ev.mouse.where = newm.where;
        ev.mouse.wheel = newm.wheel;
        oldm = newm;
        return true;
    }
    return false;
}

ParseResult TermIO::parseEscapeSeq(GetChBuf &buf, TEvent &ev, MouseState &oldm)
// Pre: "\x1B" has just been read.
{
    if (buf.get() == '[')
    {
        switch (buf.get())
        {
            // Mouse events are usually detected in 'parseCursesMouse'.
            case 'M':
                return parseX10Mouse(buf, ev, oldm) == Accepted ? Accepted : Ignored;
            case '<':
                return parseSGRMouse(buf, ev, oldm) == Accepted ? Accepted : Ignored;
        }
    }
    buf.reject();
    return Rejected;
}

ParseResult TermIO::parseX10Mouse(GetChBuf &buf, TEvent &ev, MouseState &oldm)
// Pre: "\x1B[M" has just been read.
// The complete sequence looks like "\x1B[Mabc", where:
// * 'a' is the button number plus 32.
// * 'b' is the column number (one-based) plus 32.
// * 'c' is the row number (one-based) plus 32.
{
    int but, col, row;
    but = buf.get();
    if (but < 32 || 255 < but) return Rejected;
    but -= 32;
    for (int *i : {&col, &row})
    {
        *i = buf.get();
        if (*i < 0 || 255 < *i)
            return Rejected;
        // In theory, this encoding only supports coordinates in the range [0, 222].
        // However, some terminal emulators (e.g. urxvt) keep increasing the
        // counters, causing an overflow. We can take advantage of this to support
        // more coordinates, but we definitely don't want to reject the sequence,
        // as that will cause Ctrl+key events to be generated.
        if (*i > 32)
            *i -= 32;
        else
            *i += (256 - 32);
        // Make it zero-based.
        --*i;
    }

    MouseState newm = {};
    newm.where = {col, row};
    newm.buttons = oldm.buttons;
    switch (but)
    {
        case 0: // Press.
        case 32: // Drag.
            newm.buttons |= mbLeftButton; break;
        case 1:
        case 33:
            newm.buttons |= mbMiddleButton; break;
        case 2:
        case 34:
            newm.buttons |= mbRightButton; break;
        case 3: newm.buttons = 0; break; // Release.
        case 64: newm.wheel = mwUp; break;
        case 65: newm.wheel = mwDown; break;
    }
    return acceptMouseEvent(ev, oldm, newm) ? Accepted : Ignored;
}

ParseResult TermIO::parseSGRMouse(GetChBuf &buf, TEvent &ev, MouseState &oldm)
// https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Extended-coordinates
// Pre: "\x1B[<" has just been read.
// The complete sequence looks like "\x1B[<a;b;cM" or "\x1B[<a;b;cm", where:
// * 'a' is a sequence of digits representing the button number in decimal.
// * 'b' is a sequence of digits representing the column number (one-based) in decimal.
// * 'c' is a sequence of digits representing the row number (one-based) in decimal.
// The sequence ends with 'M' on button press and on 'm' on button release.
{
    int but, col, row, state;
    for (int *i : {&but, &col, &row})
        if ((*i = buf.getNum()) == -1)
            return Rejected;
    // Make the coordinates zero-based.
    --row, --col;
    // Finally, the press/release state.
    state = buf.last();
    if (!(state == 'M' || state == 'm')) return Rejected;

    MouseState newm = {};
    newm.where = {col, row};
    newm.buttons = oldm.buttons;
    if (state == 'M') // Press, wheel or drag.
    {
        switch (but)
        {
            case 0:
            case 32:
                newm.buttons |= mbLeftButton; break;
            case 1:
            case 33:
                newm.buttons |= mbMiddleButton; break;
            case 2:
            case 34:
                newm.buttons |= mbRightButton; break;
            case 64: newm.wheel = mwUp; break;
            case 65: newm.wheel = mwDown; break;
        }
    }
    else // Release.
    {
        switch (but)
        {
            case 0: newm.buttons &= ~mbLeftButton; break;
            case 1: newm.buttons &= ~mbMiddleButton; break;
            case 2: newm.buttons &= ~mbRightButton; break;
        }
    }
    return acceptMouseEvent(ev, oldm, newm) ? Accepted : Ignored;
}

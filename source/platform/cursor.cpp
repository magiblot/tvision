#include <internal/cursor.h>

// When the 'default color' attribute is set, we cannot expect the foreground
// or background colors to match what's actually shown on screen. So this case
// has to be handled separately.

void ReverseScreenCursor::draw(TCellAttribs &attrib) const
{
    TCellAttribs attr = attrib;
    if (attr.fgDefault || attr.bgDefault)
    {
        // Not all terminals deal with the 'reverse' attribute in the same way.
        // So fall back on it only when the 'default' attribute is used.
        attr.reverse = !attr.reverse;
    }
    else
    {
        auto fg = attr.fgGet(),
             bg = attr.bgGet();
        attr.fgSet(bg);
        attr.bgSet(fg);
    }
    attrib = attr;
}

void NegativeScreenCursor::draw(TCellAttribs &attrib) const
{
    TCellAttribs attr = attrib;
    if (attr.fgDefault)
    {
        attr.fgDefault = 0;
        attr.fgSet(0x7); // Assume white foregound.
    }
    if (attr.bgDefault)
    {
        attr.bgDefault = 0;
        attr.bgSet(0x0); // Assume black background.
    }
    auto fg = attr.fgGet(),
         bg = attr.bgGet();
    attr.fgSet(fg ^ 0x7);
    attr.bgSet(bg ^ 0x7);
    attrib = attr;
}

#include <internal/cursor.h>

void ReverseScreenCursor::draw(TCellAttribs &attrib) const {
    TCellAttribs attr = attrib;
    attr.fgDefault = attr.bgDefault = 0;
    auto fg = attr.fgGet(),
         bg = attr.bgGet();
    attr.fgSet(bg);
    attr.bgSet(fg);
    attrib = attr;
}

void NegativeScreenCursor::draw(TCellAttribs &attrib) const {
    TCellAttribs attr = attrib;
    attr.fgDefault = attr.bgDefault = 0;
    auto fg = attr.fgGet(),
         bg = attr.bgGet();
    attr.fgSet(fg ^ 0x7);
    attr.bgSet(bg ^ 0x7);
    attrib = attr;
}

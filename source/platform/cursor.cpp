#include <internal/cursor.h>

void ReverseScreenCursor::draw(TColorAttr &attr) const
{
    attr = reverseAttribute(attr);
}

void NegativeScreenCursor::draw(TColorAttr &attr) const
{
    auto fg = ::getFore(attr),
         bg = ::getBack(attr);
    ::setFore(attr, TColorBIOS(fg.toBIOS(true) ^ 0x7));
    ::setBack(attr, TColorBIOS(bg.toBIOS(false) ^ 0x7));
}

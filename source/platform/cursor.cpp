#include <internal/cursor.h>

void ScreenCursor::draw(TCellAttribs &attr) const {
    attr.fgDefault = attr.bgDefault = 0;
    switch (style) {
        case Reverse: {
            auto fg = attr & 0x0F;
            uchar bg = attr & 0xF0;
            attr = (attr & 0xFF00) | (fg << 4) | (bg >> 4);
            break;
        }
        case Negative:
            attr = (attr & 0xFF00) | (attr & ~0x77) | (~attr & 0x77);
            break;
    }
}

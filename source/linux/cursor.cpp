#include <internal/cursor.h>

void ScreenCursor::draw(uchar &attr) const {
    switch(style) {
        case Reverse: {
            uchar fg = attr & 0x0F;
            uchar bg = attr & 0xF0;
            attr = (fg << 4) | (bg >> 4);
            break;
        }
        case Negative:
            attr = (attr & ~0x77) | (~attr & 0x77);
            break;
    }
}

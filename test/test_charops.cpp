#include <tvision/internal/platform.h>
#include "test_charops.h"

// Test-only implementation of Platform::CharOps that does not depend on the
// system locale.

void TestCharOps::init()
{
    tvision::Platform::charOps = {
        mockCharWidth,
        mockCharToLower,
    };
};

int TestCharOps::mockCharWidth(uint32_t ch) noexcept
{
    switch (ch)
    {
        case *COMBINING_ZIGZAG_UTF32:
        case *DEVANAGARI_VIRAMA_UTF32:
        case *ZERO_WIDTH_JOINER_UTF32:
        case U'\0':
            return 0;
        case *SMILING_FACE_WITH_SMILING_EYES_UTF32:
            return 2;
        default:
            if (ch <= U'\x1F' || (U'\x7F' <= ch && ch <= U'\x9F'))
                return -1;
            return 1;
    }
}

uint32_t TestCharOps::mockCharToLower(uint32_t ch) noexcept
{
    switch (ch)
    {
        case U'F': return U'f';
        case U'Ф': return U'ф';
        case U'Ы': return U'ы';
        default: return ch;
    }
}

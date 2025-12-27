#ifndef TVISION_TEST_CHAROPS_H
#define TVISION_TEST_CHAROPS_H

#include <stdint.h>

#define COMBINING_ZIGZAG_UTF8 "\xCD\x9B"
#define COMBINING_ZIGZAG_UTF32 U"\u035B"
#define DEVANAGARI_VIRAMA_UTF8 "\xE0\xA5\x8D"
#define DEVANAGARI_VIRAMA_UTF32 U"\u094D"
#define ZERO_WIDTH_JOINER_UTF8 "\xE2\x80\x8D"
#define ZERO_WIDTH_JOINER_UTF32 U"\u200D"
#define SMILING_FACE_WITH_SMILING_EYES_UTF8 "\xF0\x9F\x98\x8A"
#define SMILING_FACE_WITH_SMILING_EYES_UTF32 U"\U0001F60A"

class TestCharOps
{
public:

    static void init();

private:

    static int mockCharWidth(uint32_t) noexcept;
    static uint32_t mockCharToLower(uint32_t) noexcept;
};

#endif // TVISION_TEST_CHAROPS_H

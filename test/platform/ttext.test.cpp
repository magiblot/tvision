#define Uses_TText
#include <tvision/tv.h>

#include <test.h>

#define COMBINING_ZIGZAG_UTF8 "\xCD\x9B"
#define COMBINING_ZIGZAG_UTF32 U"\u035B"
#define ZERO_WIDTH_JOINER_UTF8 "\xE2\x80\x8D"

static std::ostream &operator<<(std::ostream &os, TSpan<const char32_t> span)
{
    for (char32_t ch : span)
    {
        char s[11];
        if (ch <= 0xFF)
            sprintf(s, "\\x%02X", ch);
        else if (ch <= 0xFFFF)
            sprintf(s, "\\u%04X", ch);
        else
            sprintf(s, "\\U%08X", ch);
        os << s;
    }
    return os;
}

TEST(TText, ShouldConvertUtf8ControlCharacters)
{
    static const TestCase<TStringView> testCases[] =
    {
        {{"\0", 1}, {"\0", 1}},
        {"\x01", "☺"},
        {"\x1F", "▼"},
        {" ", " "},
        {"~", "~"},
        {"\x7F", "⌂"},
        {"\x80", "Ç"},
        {"\xFE", "■"},
        {"\x03" COMBINING_ZIGZAG_UTF8, "♥" COMBINING_ZIGZAG_UTF8},
        {"ñ" COMBINING_ZIGZAG_UTF8, "ñ" COMBINING_ZIGZAG_UTF8},
    };

    for (auto &testCase : testCases)
    {
        TScreenCell cells[1] {};
        size_t i = 0, j = 0;
        while(TText::drawOne(cells, i, testCase.input, j));
        TStringView actual = cells[0]._ch.getText();
        expectResultMatches(actual, testCase);
    }
}

TEST(TText, ShouldConvertUtf32ControlCharacters)
{
    static const TestCase<TSpan<const char32_t>, TStringView> testCases[] =
    {
        {{U"\0", 1}, {"\0", 1}},
        {{U"\x01", 1}, "�"},
        {{U"\x1F", 1}, "�"},
        {{U" ", 1}, " "},
        {{U"~", 1}, "~"},
        {{U"\x7F", 1}, "�"},
        {{U"\x80", 1}, "�"},
        {{U"\xFE", 1}, "þ"},
        {{U"\x03" COMBINING_ZIGZAG_UTF32, 2}, "�" COMBINING_ZIGZAG_UTF8},
        {{U"ñ" COMBINING_ZIGZAG_UTF32, 2}, "ñ" COMBINING_ZIGZAG_UTF8},
    };

    for (auto &testCase : testCases)
    {
        TScreenCell cells[1] {};
        TSpan<const uint32_t> input {(const uint32_t *) testCase.input.data(), testCase.input.size()};
        size_t i = 0, j = 0;
        while(TText::drawOne(cells, i, input, j));
        TStringView actual = cells[0]._ch.getText();
        expectResultMatches(actual, testCase);
    }
}

TEST(TText, ShouldDrawTextInScreenCells)
{
    enum { nCells = 2 };
    static const TestCase<TStringView, std::vector<TStringView>> testCases[] =
    {
        {"a", {"a", {"\0", 1}}},
        {"aa", {"a", "a"}},
        {{"\0", 1}, {{"\0", 1}, {"\0", 1}}},
        {{"\0\0", 2}, {{"\0", 1}, {"\0", 1}}},
        {{"\0a", 2}, {{"\0", 1}, "a"}},
        {"a" COMBINING_ZIGZAG_UTF8, {"a" COMBINING_ZIGZAG_UTF8, {"\0", 1}}},
        {{"\0" COMBINING_ZIGZAG_UTF8, 3}, {" " COMBINING_ZIGZAG_UTF8, {"\0", 1}}},
        {"क्"  ZERO_WIDTH_JOINER_UTF8 "ष", {"क्", "ष"}},
    };

    for (auto &testCase : testCases)
    {
        TScreenCell cells[nCells] {};
        TText::drawStr(cells, 0, testCase.input, 0);

        std::vector<TStringView> actual(nCells);
        for (int i = 0; i < nCells; ++i)
            actual[i] = cells[i]._ch.getText();

        expectResultMatches(actual, testCase);
    }
}

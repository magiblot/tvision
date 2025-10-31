#define Uses_TText
#include <tvision/tv.h>

#include <test.h>
#include <test_charops.h>
#include <stdlib.h>

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

TEST(TText, ShouldConvertUtf8ToCodePage)
{
    static const TestCase<TStringView, char> testCases[] =
    {
        {{}, '\0'},
        {{"\0", 1}, '\0'},
        {"a", 'a'},
        {"○", '\t'},
        {"\t", '\t'},
        {"≡", '\xF0'},
        {"€", '\0'},
    };

    for (auto &testCase : testCases)
    {
        auto actual = TText::toCodePage(testCase.input);
        expectResultMatches(actual, testCase);
    }
}

TEST(TText, ShouldConvertCodePageToUtf8)
{
    static const TestCase<char, TStringView> testCases[] =
    {
        {'\0', {"\0", 1}},
        {'a', "a"},
        {'\t', "○"},
        {'\xF0', "≡"},
    };

    for (auto &testCase : testCases)
    {
        auto actual = TText::fromCodePage(testCase.input);
        expectResultMatches(actual, testCase);
    }
}

TEST(TText, ShouldOverrideCodePageTranslation)
{
    char customCodepageToUtf8[256][4] = {};
    memcpy(customCodepageToUtf8[0x10], "¥", 2); // ASCII position.
    memcpy(customCodepageToUtf8[0x80], "\U00010000", 4); // non-ASCII position, 4 bytes in UTF-8.

    TText::setCodePageTranslation(&customCodepageToUtf8);

    static const TestCase<TStringView, char> toCodePageTestCases[] =
    {
        {"¥", '\x10'},
        {"\x10", '\x10'},
        {"\x20", '\x20'},
        {"\U00010000", '\x80'},
    };

    for (auto &testCase : toCodePageTestCases)
    {
        auto actual = TText::toCodePage(testCase.input);
        expectResultMatches(actual, testCase);
    }

    static const TestCase<char, TStringView> fromCodePageTestCases[] =
    {
        {'\x10', "¥"},
        {'\x20', {"\0", 1}},
        {'\x80', "\U00010000"},
    };

    for (auto &testCase : fromCodePageTestCases)
    {
        auto actual = TText::fromCodePage(testCase.input);
        expectResultMatches(actual, testCase);
    }

    TText::setCodePageTranslation(nullptr);

    static const TestCase<char, TStringView> backToDefaultTestCases[] =
    {
        {'\x10', "►"},
        {'\x20', " "},
        {'\x80', "Ç"},
    };

    for (auto &testCase : backToDefaultTestCases)
    {
        auto actual = TText::fromCodePage(testCase.input);
        expectResultMatches(actual, testCase);
    }
}

TEST(TText, ShouldConvertUtf8ControlCharacters)
{
    TestCharOps::init();
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
        TText::drawStr(cells, testCase.input);
        TStringView actual = cells[0]._ch.getText();
        expectResultMatches(actual, testCase);
    }
}

TEST(TText, ShouldConvertUtf32ControlCharacters)
{
    TestCharOps::init();
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
        TText::drawStr(cells, 0, input, 0);
        TStringView actual = cells[0]._ch.getText();
        expectResultMatches(actual, testCase);
    }
}

TEST(TText, ShouldDrawTextInScreenCells)
{
    enum { nCells = 2 };
    TestCharOps::init();
    static const TestCase<TStringView, std::vector<TStringView>> testCases[] =
    {
        {"a", {"a", {"\0", 1}}},
        {"aa", {"a", "a"}},
        {{"\0", 1}, {{"\0", 1}, {"\0", 1}}},
        {{"\0\0", 2}, {{"\0", 1}, {"\0", 1}}},
        {{"\0a", 2}, {{"\0", 1}, "a"}},
        {"a" COMBINING_ZIGZAG_UTF8, {"a" COMBINING_ZIGZAG_UTF8, {"\0", 1}}},
        {{"\0" COMBINING_ZIGZAG_UTF8, 3}, {" " COMBINING_ZIGZAG_UTF8, {"\0", 1}}},
        {"क" DEVANAGARI_VIRAMA_UTF8 ZERO_WIDTH_JOINER_UTF8 "ष", {"क" DEVANAGARI_VIRAMA_UTF8, "ष"}},
    };

    for (auto &testCase : testCases)
    {
        TScreenCell cells[nCells] {};
        TText::drawStr(cells, testCase.input);

        std::vector<TStringView> actual(nCells);
        for (int i = 0; i < nCells; ++i)
            actual[i] = cells[i]._ch.getText();

        expectResultMatches(actual, testCase);
    }
}

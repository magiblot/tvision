#define Uses_TText
#include <tvision/tv.h>
#include <tvision/internal/platform.h>
#include <tvision/internal/unixcon.h>

#include <test.h>
#include <stdlib.h>

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

static bool mustSkipTTextTests = false;

#if !defined(_WIN32)

// On Unix, the 'wcwidth' function might be used to determine character widths.
// The behaviour of this function depends on the current locale, so we need
// to check if a UTF-8 locale is being used.
TEST(TText, Utf8LocaleIsBeingUsed)
{
    // 'Platform::charWidth' initializes the locale when first invoked.
    tvision::Platform::charWidth(L'\0');
    // Then, ensure the UnixConsoleAdapter implementation is used, which
    // relies on 'wcwidth'.
    tvision::Platform::charWidth = &tvision::UnixConsoleAdapter::charWidth;

    TStringView input = "☺";
    wchar_t actual = L'\0';

    // If the locale is right, this should convert from UTF-8 into UTF-32.
    mbtowc(&actual, &input[0], input.size());

    wchar_t expected = L'☺';
    bool localeIsUtf8 = (actual == expected);
    EXPECT_TRUE(localeIsUtf8) <<
        "\nIMPORTANT!\n"
        "A UTF-8 locale is required to run these tests. Please specify a valid "
        "UTF-8 locale using the LC_CTYPE or LC_ALL environment variable. The "
        "command 'locale -a' may provide you with a list of available locales.";

    // If the locale is wrong, there is no point in running the other tests.
    mustSkipTTextTests = !localeIsUtf8;
}

#endif // _WIN32

TEST(TText, ShouldConvertUtf8ControlCharacters)
{
    if (mustSkipTTextTests)
        GTEST_SKIP();

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
    if (mustSkipTTextTests)
        GTEST_SKIP();

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
    if (mustSkipTTextTests)
        GTEST_SKIP();

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

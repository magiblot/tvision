#include <internal/utf8.h>

#include <test.h>

static std::ostream &operator<<(std::ostream &os, TSpan<const char16_t> span)
{
    for (char16_t ch : span)
    {
        char s[4 + 3];
        if (ch <= 0xFF)
            sprintf(s, "\\x%02X", ch);
        else
            sprintf(s, "\\u%04X", ch);
        os << s;
    }
    return os;
}

namespace tvision
{

TEST(Utf8, ShouldConvertUtf16StringToUtf8)
{
    static const TestCase<TSpan<const char16_t>, TStringView> testCases[] =
    {
        {{}, ""},
        {{u"abcd", 4}, "abcd"},
        {{u"o\U0001F495W", 4}, "o💕W"},
    };

    for (auto &testCase : testCases)
    {
        TSpan<const uint16_t> input {(const uint16_t *) testCase.input.data(), testCase.input.size()};
        char *buf = new char[input.size()*3];
        size_t length = utf16To8(input, buf);
        TStringView actual {buf, length};
        expectResultMatches(actual, testCase);
        delete[] buf;
    }
}

} // namespace tvision

#include <internal/utf8.h>

#include <test.h>

namespace tvision
{

TEST(Utf8, ShouldConvertUtf16StringToUtf8)
{
    static const TestCase<TSpan<const char>, TStringView> testCases[] =
    {
        {"", ""},
        {"\x61\x00\x62\x00\x63\x00\x64\x00", "abcd"},
        {"\x6F\x00\x3D\xD8\x95\xDC\x57\x00", "o💕W"},
    };

    for (auto &testCase : testCases)
    {
        TSpan<const uint16_t> input {
            (const uint16_t *) testCase.input.data(),
            testCase.input.size()/2,
        };
        char *buf = new char[input.size()*3];
        size_t length = utf16To8(input, buf);
        TStringView actual {buf, length};
        expectResultMatches(actual, testCase);
        delete[] buf;
    }
}

} // namespace tvision

#define Uses_TText
#include <tvision/tv.h>

#include <internal/dispbuff.h>
#include <internal/platform.h>

#include <test.h>
#include <test_charops.h>
#include <test_operators.h>
#include <sstream>
#include <string>
#include <vector>

namespace tvision
{

// Records the calls made on it into 'calls', so that tests can check the
// exact sequence of operations.
class TestDisplayAdapter : public DisplayAdapter
{
public:

    TPoint size;
    std::vector<std::string> calls;

    TestDisplayAdapter(TPoint aSize) noexcept :
        size(aSize)
    {
    }

    TPoint reloadScreenInfo() noexcept override
    {
        return size;
    }

    void writeCell( TPoint pos, TStringView text,
                    TColorAttr attr, bool doubleWidth ) noexcept override
    {
        std::ostringstream oss;
        oss << "writeCell(" << pos << ", \"" << text << "\", "
            << attr << ", " << (doubleWidth ? "true" : "false") << ")";
        calls.push_back(oss.str());
    }

    void setCaretPosition(TPoint pos) noexcept override
    {
        std::ostringstream oss;
        oss << "setCaretPosition(" << pos << ")";
        calls.push_back(oss.str());
    }

    void setCaretSize(int aSize) noexcept override
    {
        calls.push_back("setCaretSize(" + std::to_string(aSize) + ")");
    }

    void clearScreen() noexcept override
    {
        calls.push_back("clearScreen()");
    }

    void flush() noexcept override
    {
        calls.push_back("flush()");
    }
};

struct DisplayBufferFlushTestInput
{
    TPoint size;
    TStringView text;
};

using DisplayBufferFlushTestOutput = std::vector<std::string>;

static std::ostream &operator<<(std::ostream &os, const DisplayBufferFlushTestInput &self)
{
    os << "Size: " << self.size << std::endl
       << "Text: '" << self.text << '\'';
    return os;
}

TEST(DisplayBuffer, ShouldRenderProperlyOnFlushScreen)
{
    TestCharOps::init();
    static const TestCase<DisplayBufferFlushTestInput, DisplayBufferFlushTestOutput> testCases[] =
    {
        // The cursor should advance by 2 when drawing a double-width character.
        {   {   TPoint {4, 1},
                "[" SMILING_FACE_WITH_SMILING_EYES_UTF8 "]",
            },
            {   "writeCell({0, 0}, \"[\", {Default, Default}, false)",
                "writeCell({1, 0}, \"" SMILING_FACE_WITH_SMILING_EYES_UTF8 "\", {Default, Default}, true)",
                "writeCell({3, 0}, \"]\", {Default, Default}, false)",
                "flush()",
            },
        },
    };

    for (const auto &testCase : testCases)
    {
        auto &input = testCase.input;

        TestDisplayAdapter display(input.size);
        DisplayBuffer buf;
        buf.reloadScreenInfo(display);
        // Flush once so that the caret size is settled and does not show up
        // as a call below.
        buf.flushScreen(display);

        std::vector<TScreenCell> cells(input.size.x);
        TText::drawStr({cells.data(), cells.size()}, input.text);
        buf.screenWrite(0, 0, cells.data(), (int) cells.size());

        // Only the calls made by the second 'flushScreen' are relevant to this test.
        display.calls.clear();
        buf.flushScreen(display);

        expectResultMatches(display.calls, testCase);
    }
}

} // namespace tvision

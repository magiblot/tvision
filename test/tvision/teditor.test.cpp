#define Uses_TGroup
#define Uses_TEditor
#define Uses_TText
#include <tvision/tv.h>

#include <test.h>
#include <test_operators.h>
#include <test_charops.h>

class DrawableTestGroup : public TGroup
{
public:
    DrawableTestGroup(const TRect &bounds) noexcept :
        TGroup(bounds)
    {
    }

    TColorAttr mapColor(uchar i) noexcept
    {
        return TColorAttr(i); // Use palette index as attribute.
    }

    std::vector<std::vector<TStringView>> getBufferText()
    {
        std::vector<std::vector<TStringView>> text(
            size.y,
            std::vector<TStringView>(size.x)
        );
        for (int y = 0; y < size.y; ++y)
            for (int x = 0; x < size.x; ++x)
                text[y][x] = buffer[y*size.x + x]._ch.getText();
        return text;
    }

    std::vector<std::vector<int>> getBufferAttributes()
    {
        std::vector<std::vector<int>> attributes(
            size.y,
            std::vector<int>(size.x)
        );
        for (int y = 0; y < size.y; ++y)
            for (int x = 0; x < size.x; ++x)
                // Extract palette index from attribute.
                attributes[y][x] = (uchar) buffer[y*size.x + x].attr;
        return attributes;
    }
};

struct TEditorTestInput
{
    TPoint size;
    TStringView text;
    TPoint cursor;
    std::vector<TEvent> events;
};

struct TEditorTestOutput
{
    std::vector<std::vector<TStringView>> text;
    std::vector<std::vector<int>> attributes;
    TPoint cursor;
};

static bool operator==(const TEditorTestOutput &a, const TEditorTestOutput &b)
{
    return
        a.text == b.text &&
        a.attributes == b.attributes &&
        a.cursor == b.cursor;
}

static std::ostream &operator<<(std::ostream &os, const TEditorTestInput &self)
{
    os << "Size: " << self.size << std::endl
       << "Text: '" << self.text << '\'' << std::endl
       << "Cursor: " << self.cursor << std::endl
       << "Events: " << testing::PrintToString(self.events);
    return os;
}

static std::ostream &operator<<(std::ostream &os, const TEditorTestOutput &self)
{
    os << std::endl
       << "Text: " << testing::PrintToString(self.text) << std::endl
       << "Attributes: " << testing::PrintToString(self.attributes) << std::endl
       << "Cursor: " << self.cursor;
    return os;
}

TEST(TEditor, ShouldDrawTextAndPlaceCursorCorrectly)
{
    TestCharOps::init();
    static const TestCase<TEditorTestInput, TEditorTestOutput> testCases[] =
    {
        {   {   TPoint { 3, 1 },
                "€",
                TPoint { 1, 0 },
                {   messageEv(evCommand, cmEncoding),
                    keyDownEv(kbLeft, 0x0000),
                    keyDownEv(kbBack, 0x0000),
                    keyDownEv(kbBack, 0x0000),
                    messageEv(evCommand, cmEncoding),
                    messageEv(evCommand, cmUndo),
                },
            },
            {   {{ "€", " ", " " }},
                {{   7,   6,   6 }},
                TPoint { 1, 0 },
            },
        },
    };

    for (const auto &testCase : testCases)
    {
        TRect bounds({0, 0}, testCase.input.size);
        auto cursor = testCase.input.cursor;
        auto text = testCase.input.text;
        auto *editor = new TEditor(bounds, nullptr, nullptr, nullptr, 256);
        editor->setCursor(cursor.x, cursor.y);
        editor->insertMultilineText(text.data(), text.size());
        auto *group = new DrawableTestGroup(bounds);
        group->setState(sfExposed, true);
        group->getBuffer();
        group->insert(editor);

        for (const auto &event : testCase.input.events)
        {
            TEvent inputEvent = event;
            editor->handleEvent(inputEvent);
        }

        TEditorTestOutput actual {
            group->getBufferText(),
            group->getBufferAttributes(),
            editor->cursor,
        };
        expectResultMatches(actual, testCase);

        TObject::destroy(group);
    }
}

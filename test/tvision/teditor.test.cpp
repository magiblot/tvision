#define Uses_TGroup
#define Uses_TEditor
#define Uses_TText
#include <tvision/tv.h>

#include <test.h>
#include <test_operators.h>
#include <test_charops.h>

struct TEditorBufferTestInput
{
    TStringView initialText;
    TStringView textToInsert;
};

struct TEditorBufferTestOutput
{
    TStringView bufferText;
    int lineEndingType;
};

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
            {
                auto &ch = buffer[y*size.x + x]._ch;
                if (ch._flags & TCellChar::fTrail)
                    text[y][x] = "(wide char trail)";
                else
                    text[y][x] = ch.getText();
            }
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

struct TEditorDisplayTestInput
{
    TPoint size;
    TStringView text;
    std::vector<TEvent> events;
    TPoint delta;
};

struct TEditorDisplayTestOutput
{
    std::vector<std::vector<TStringView>> text;
    std::vector<std::vector<int>> attributes;
    TPoint curPos;
    TPoint delta;
};

static bool operator==(const TEditorBufferTestOutput &a, const TEditorBufferTestOutput &b)
{
    return
        a.bufferText == b.bufferText &&
        a.lineEndingType == b.lineEndingType;
}

static bool operator==(const TEditorDisplayTestOutput &a, const TEditorDisplayTestOutput &b)
{
    return
        a.text == b.text &&
        a.attributes == b.attributes &&
        a.curPos == b.curPos &&
        a.delta == b.delta;
}

static std::ostream &operator<<(std::ostream &os, const TEditorBufferTestInput &self)
{
    os << "Initial Text: '" << self.initialText << '\'' << std::endl
       << "Text To Insert: '" << self.textToInsert << '\'' << std::endl;
    return os;
}

static std::ostream &operator<<(std::ostream &os, const TEditorBufferTestOutput &self)
{
    os << "Buffer Text: '" << self.bufferText << '\'' << std::endl
       << "Line Ending Type: " << self.lineEndingType << std::endl;
    return os;
}

static std::ostream &operator<<(std::ostream &os, const TEditorDisplayTestInput &self)
{
    os << "Size: " << self.size << std::endl
       << "Text: '" << self.text << '\'' << std::endl
       << "Events: " << testing::PrintToString(self.events) << std::endl
       << "Delta: " << self.delta;
    return os;
}

static std::ostream &operator<<(std::ostream &os, const TEditorDisplayTestOutput &self)
{
    os << std::endl
       << "Text: " << testing::PrintToString(self.text) << std::endl
       << "Attributes: " << testing::PrintToString(self.attributes) << std::endl
       << "CurPos: " << self.curPos << std::endl
       << "Delta: " << self.delta;
    return os;
}

TEST(TEditor, ShouldDetectAndConvertLineEndingsWhenInsertingText)
{
    static const TestCase<TEditorBufferTestInput, TEditorBufferTestOutput> testCases[] =
    {
        {   {   "a\nb\n",
                "c\n",
            },
            {   "a\nb\nc\n",
                TEditor::eolLf,
            },
        },
        {   {   "a\r\nb\r\n",
                "c\n",
            },
            {   "a\r\nb\r\nc\r\n",
                TEditor::eolCrLf,
            },
        },
        {   {   "a\nb\n",
                "c\r\n",
            },
            {   "a\nb\nc\n",
                TEditor::eolLf,
            },
        },
        {   {   "a\rb\r",
                "c\n",
            },
            {   "a\rb\rc\r",
                TEditor::eolCr,
            },
        },
        {   {   "a",
                "\nb",
            },
            {   "a\r\nb",
                TEditor::eolCrLf,
            },
        },
    };

    for (const auto &testCase : testCases)
    {
        auto initialText = testCase.input.initialText;
        auto *editor = new TEditor(TRect(), nullptr, nullptr, nullptr, 256);
        // The initial text must be placed at the end of the buffer, since the
        // gap will be located between the cursor and the rest of the text.
        memcpy(
            &editor->buffer[editor->bufSize - initialText.size()],
            initialText.data(),
            initialText.size()
        );
        editor->setBufLen(initialText.size());
        editor->setCurPtr(editor->bufLen, 0);

        auto textToInsert = testCase.input.textToInsert;
        editor->insertText(textToInsert.data(), textToInsert.size(), false);

        // Since the cursor should now be at the end, we can read the inserted
        // text from the beginning of the buffer.
        TEditorBufferTestOutput actual {
            TStringView(editor->buffer, editor->bufLen),
            editor->lineEndingType,
        };
        expectResultMatches(actual, testCase);

        TObject::destroy(editor);
    }
}

TEST(TEditor, ShouldInsertNewLinesProperly)
{
    static const TestCase<TEditor::LineEndingType, TStringView> testCases[] =
    {
        {   TEditor::eolCrLf,
            "\r\n",
        },
        {   TEditor::eolLf,
            "\n",
        },
        {   TEditor::eolCr,
            "\r",
        },
    };

    for (const auto &testCase : testCases)
    {
        auto *editor = new TEditor(TRect(), nullptr, nullptr, nullptr, 256);
        editor->lineEndingType = testCase.input;
        editor->newLine();

        // Since the cursor should now be at the end, we can read the inserted
        // text from the beginning of the buffer.
        TStringView actual = TStringView(editor->buffer, editor->bufLen);
        expectResultMatches(actual, testCase);

        TObject::destroy(editor);
    }
}

TEST(TEditor, ShouldDrawTextAndPlaceCursorCorrectly)
{
    TestCharOps::init();
    static const TestCase<TEditorDisplayTestInput, TEditorDisplayTestOutput> testCases[] =
    {
        // Remove part of a multi-byte character, then undo the removal.
        {   {   TPoint { 3, 1 },
                "€",
                {   messageEv(evCommand, cmEncoding),
                    keyDownEv(kbLeft, 0x0000),
                    keyDownEv(kbBack, 0x0000),
                    keyDownEv(kbBack, 0x0000),
                    messageEv(evCommand, cmEncoding),
                    messageEv(evCommand, cmUndo),
                },
                TPoint { 0, 0 },
            },
            {   {{ "€", " ", " " }},
                {{   7,   6,   6 }},
                TPoint { 1, 0 },
                TPoint { 0, 0 },
            },
        },
        // It should display multi-byte characters properly when switching to single-byte mode.
        {   {   TPoint { 4, 1 },
                "€",
                {   messageEv(evCommand, cmEncoding),
                },
                TPoint { 0, 0 },
            },
            {   {{ "Γ", "é", "¼", " " }},
                {{   6,   6,   6,   6, }},
                TPoint { 3, 0 },
                TPoint { 0, 0 },
            },
        },
        // Tabulators should be properly drawn when there is horizontal scroll.
        {   {   TPoint { 3, 1 },
                "\ta",
                {},
                TPoint { 6, 0 },
            },
            {   {{ " ", " ", "a" }},
                {{   6,   6,   6 }},
                TPoint { 9, 0 },
                TPoint { 6, 0 },
            },
        },
        // A partially visible tabulator should be drawn properly when selected.
        {   {   TPoint { 3, 1 },
                "\ta",
                {   keyDownEv(kbLeft, 0x0000),
                    keyDownEv(kbLeft, kbShift),
                },
                TPoint { 6, 0 },
            },
            {   {{ " ", " ", "a" }},
                {{   7,   7,   6 }},
                TPoint { 0, 0 },
                TPoint { 6, 0 },
            },
        },
        // A selected line break should be drawn properly.
        {   {   TPoint { 3, 2 },
                "ab\ncd",
                {   keyDownEv(kbLeft, 0x0000),
                    keyDownEv(kbLeft, kbShift),
                    keyDownEv(kbLeft, kbShift),
                    keyDownEv(kbLeft, kbShift),
                },
                TPoint { 0, 0 },
            },
            {   {{ "a", "b", " " }, { "c", "d", " " }},
                {{   6,   7,   7 }, {   7,   6,   6 }},
                TPoint { 1, 0 },
                TPoint { 0, 0 },
            },
        },
        // A double-width character should be drawn properly.
        {   {   TPoint { 3, 1 },
                SMILING_FACE_WITH_SMILING_EYES_UTF8 "a",
                {},
                TPoint { 0, 0 },
            },
            {   {{ "😊", "(wide char trail)", "a" }},
                {{   6,   6,   6 }},
                TPoint { 3, 0 },
                TPoint { 0, 0 },
            },
        },
        // A partially visible and selected double-width character should be drawn properly.
        {   {   TPoint { 2, 1 },
                SMILING_FACE_WITH_SMILING_EYES_UTF8 "a",
                {   keyDownEv(kbLeft, 0x0000),
                    keyDownEv(kbLeft, kbShift),
                },
                TPoint { 1, 0 },
            },
            {   {{ " ", "a" }},
                {{   7,   6 }},
                TPoint { 0, 0 },
                TPoint { 1, 0 },
            },
        },
        // Combining characters should be drawn properly.
        {   {   TPoint { 2, 1 },
                "a" COMBINING_ZIGZAG_UTF8 "b" COMBINING_ZIGZAG_UTF8 "c" COMBINING_ZIGZAG_UTF8 "d" COMBINING_ZIGZAG_UTF8,
                {},
                TPoint { 1, 0 },
            },
            {   {{ "b" COMBINING_ZIGZAG_UTF8, "c" COMBINING_ZIGZAG_UTF8 }},
                {{   6,   6 }},
                TPoint { 4, 0 },
                TPoint { 1, 0 },
            },
        },
    };

    for (const auto &testCase : testCases)
    {
        TRect bounds({0, 0}, testCase.input.size);
        auto text = testCase.input.text;
        auto delta = testCase.input.delta;
        auto *editor = new TEditor(bounds, nullptr, nullptr, nullptr, 256);
        auto *group = new DrawableTestGroup(bounds);
        group->setState(sfExposed, true);
        group->getBuffer();
        group->insert(editor);

        editor->insertText(text.data(), text.size(), false);
        for (const auto &event : testCase.input.events)
        {
            TEvent inputEvent = event;
            editor->handleEvent(inputEvent);
        }
        editor->scrollTo(delta.x, delta.y);

        TEditorDisplayTestOutput actual {
            group->getBufferText(),
            group->getBufferAttributes(),
            editor->curPos,
            editor->delta,
        };
        expectResultMatches(actual, testCase);

        TObject::destroy(group);
    }
}

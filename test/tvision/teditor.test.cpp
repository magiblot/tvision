#define Uses_TGroup
#define Uses_TEditor
#define Uses_TText
#include <tvision/tv.h>

#include <forward_list>
#include <test.h>
#include <test_operators.h>
#include <test_charops.h>
#include <test_group.h>

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

class DrawableTestGroup : public TestGroup
{
public:
    DrawableTestGroup(const TRect &bounds, std::forward_list<TEvent> eventQueue = {}) noexcept :
        TestGroup(std::move(eventQueue))
    {
        changeBounds(bounds);
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
                auto &ch = buffer[y*size.x + x].character;
                if (ch.isWideCharTrail())
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
                attributes[y][x] = (uchar) buffer[y*size.x + x].attribute;
        return attributes;
    }
};

struct TEditorDisplayTestInput
{
    TPoint size;
    TStringView text;
    std::forward_list<TEvent> events;
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
                {   messageEv(evCommand, cmTextEnd),
                    messageEv(evCommand, cmEncoding),
                    keyDownEv(kbLeft),
                    keyDownEv(kbBack),
                    keyDownEv(kbBack),
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
                    messageEv(evCommand, cmTextEnd),
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
                {   messageEv(evCommand, cmTextEnd),
                },
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
                {   messageEv(evCommand, cmTextEnd),
                    keyDownEv(kbLeft),
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
                {   keyDownEv(kbRight),
                    keyDownEv(kbRight, kbShift),
                    keyDownEv(kbRight, kbShift),
                    keyDownEv(kbRight, kbShift),
                },
                TPoint { 0, 0 },
            },
            {   {{ "a", "b", " " }, { "c", "d", " " }},
                {{   6,   7,   7 }, {   7,   6,   6 }},
                TPoint { 1, 1 },
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
                TPoint { 0, 0 },
                TPoint { 0, 0 },
            },
        },
        // A partially visible and selected double-width character should be drawn properly.
        {   {   TPoint { 2, 1 },
                SMILING_FACE_WITH_SMILING_EYES_UTF8 "a",
                {   messageEv(evCommand, cmTextEnd),
                    keyDownEv(kbLeft),
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
                TPoint { 0, 0 },
                TPoint { 1, 0 },
            },
        },
        // Ctrl+Right should skip trailing whitespace instead of stopping on it.
        {   {   TPoint { 7, 1 },
                "foo bar",
                {   messageEv(evCommand, cmWordRight),
                },
                TPoint { 0, 0 },
            },
            {   {{ "f", "o", "o", " ", "b", "a", "r" }},
                {{   6,   6,   6,   6,   6,   6,   6 }},
                TPoint { 4, 0 },
                TPoint { 0, 0 },
            },
        },
        // Ctrl+Left should do the same in reverse.
        {   {   TPoint { 7, 1 },
                "foo bar",
                {   messageEv(evCommand, cmTextEnd),
                    messageEv(evCommand, cmWordLeft),
                    messageEv(evCommand, cmWordLeft),
                },
                TPoint { 0, 0 },
            },
            {   {{ "f", "o", "o", " ", "b", "a", "r" }},
                {{   6,   6,   6,   6,   6,   6,   6 }},
                TPoint { 0, 0 },
                TPoint { 0, 0 },
            },
        },
        // Ctrl+Right should reach the punctuation before the next word.
        {   {   TPoint { 10, 1 },
                "foo(); bar",
                {   messageEv(evCommand, cmWordRight),
                },
                TPoint { 0, 0 },
            },
            {   {{ "f", "o", "o", "(", ")", ";", " ", "b", "a", "r" }},
                {{   6,   6,   6,   6,   6,   6,   6,   6,   6,   6 }},
                TPoint { 3, 0 },
                TPoint { 0, 0 },
            },
        },
        // A second Ctrl+Right should then skip the punctuation and the whitespace after it together.
        {   {   TPoint { 10, 1 },
                "foo(); bar",
                {   messageEv(evCommand, cmWordRight),
                    messageEv(evCommand, cmWordRight),
                },
                TPoint { 0, 0 },
            },
            {   {{ "f", "o", "o", "(", ")", ";", " ", "b", "a", "r" }},
                {{   6,   6,   6,   6,   6,   6,   6,   6,   6,   6 }},
                TPoint { 7, 0 },
                TPoint { 0, 0 },
            },
        },
        // Ctrl+Left should reach the same two points in reverse.
        {   {   TPoint { 10, 1 },
                "foo(); bar",
                {   messageEv(evCommand, cmTextEnd),
                    messageEv(evCommand, cmWordLeft),
                    messageEv(evCommand, cmWordLeft),
                },
                TPoint { 0, 0 },
            },
            {   {{ "f", "o", "o", "(", ")", ";", " ", "b", "a", "r" }},
                {{   6,   6,   6,   6,   6,   6,   6,   6,   6,   6 }},
                TPoint { 3, 0 },
                TPoint { 0, 0 },
            },
        },
        {   {   TPoint { 10, 1 },
                "foo(); bar",
                {   messageEv(evCommand, cmTextEnd),
                    messageEv(evCommand, cmWordLeft),
                    messageEv(evCommand, cmWordLeft),
                    messageEv(evCommand, cmWordLeft),
                },
                TPoint { 0, 0 },
            },
            {   {{ "f", "o", "o", "(", ")", ";", " ", "b", "a", "r" }},
                {{   6,   6,   6,   6,   6,   6,   6,   6,   6,   6 }},
                TPoint { 0, 0 },
                TPoint { 0, 0 },
            },
        },
        // Ctrl+Right should skip consecutive blank lines in a single jump.
        {   {   TPoint { 3, 4 },
                "foo\n\n\nbar",
                {   messageEv(evCommand, cmWordRight),
                },
                TPoint { 0, 0 },
            },
            {   {{ "f", "o", "o" }, { " ", " ", " " }, { " ", " ", " " }, { "b", "a", "r" }},
                {{   6,   6,   6 }, {   6,   6,   6 }, {   6,   6,   6 }, {   6,   6,   6 }},
                TPoint { 0, 3 },
                TPoint { 0, 0 },
            },
        },
        // Ctrl+Left should do the same in reverse.
        {   {   TPoint { 3, 4 },
                "foo\n\n\nbar",
                {   messageEv(evCommand, cmTextEnd),
                    messageEv(evCommand, cmWordLeft),
                    messageEv(evCommand, cmWordLeft),
                },
                TPoint { 0, 0 },
            },
            {   {{ "f", "o", "o" }, { " ", " ", " " }, { " ", " ", " " }, { "b", "a", "r" }},
                {{   6,   6,   6 }, {   6,   6,   6 }, {   6,   6,   6 }, {   6,   6,   6 }},
                TPoint { 0, 0 },
                TPoint { 0, 0 },
            },
        },
        // Ctrl+Del should delete consecutive punctuation characters and the whitespace after them in one step.
        {   {   TPoint { 10, 1 },
                "foo(); bar",
                {   messageEv(evCommand, cmWordRight),
                    messageEv(evCommand, cmDelWord),
                },
                TPoint { 0, 0 },
            },
            {   {{ "f", "o", "o", "b", "a", "r", " ", " ", " ", " " }},
                {{   6,   6,   6,   6,   6,   6,   6,   6,   6,   6 }},
                TPoint { 3, 0 },
                TPoint { 0, 0 },
            },
        },
        // Ctrl+Backspace should mirror that going left.
        {   {   TPoint { 7, 1 },
                "foo bar",
                {   messageEv(evCommand, cmTextEnd),
                    messageEv(evCommand, cmDelWordLeft),
                },
                TPoint { 0, 0 },
            },
            {   {{ "f", "o", "o", " ", " ", " ", " " }},
                {{   6,   6,   6,   6,   6,   6,   6 }},
                TPoint { 4, 0 },
                TPoint { 0, 0 },
            },
        },
        // Double-clicking inside a word selects exactly that word.
        {   {   TPoint { 9, 1 },
                "foo   bar",
                {   mouseDownEv({ 1, 0 }, meDoubleClick),
                    mouseUpEv({ 1, 0 }),
                },
                TPoint { 0, 0 },
            },
            {   {{ "f", "o", "o", " ", " ", " ", "b", "a", "r" }},
                {{   7,   7,   7,   6,   6,   6,   6,   6,   6 }},
                TPoint { 3, 0 },
                TPoint { 0, 0 },
            },
        },
        // Double-clicking inside a run of whitespace selects just that run.
        {   {   TPoint { 9, 1 },
                "foo   bar",
                {   mouseDownEv({ 4, 0 }, meDoubleClick),
                    mouseUpEv({ 4, 0 }),
                },
                TPoint { 0, 0 },
            },
            {   {{ "f", "o", "o", " ", " ", " ", "b", "a", "r" }},
                {{   6,   6,   6,   7,   7,   7,   6,   6,   6 }},
                TPoint { 6, 0 },
                TPoint { 0, 0 },
            },
        },
        // Double-clicking on punctuation selects just the consecutive punctuation characters.
        {   {   TPoint { 9, 1 },
                "foo();bar",
                {   mouseDownEv({ 4, 0 }, meDoubleClick),
                    mouseUpEv({ 4, 0 }),
                },
                TPoint { 0, 0 },
            },
            {   {{ "f", "o", "o", "(", ")", ";", "b", "a", "r" }},
                {{   6,   6,   6,   7,   7,   7,   6,   6,   6 }},
                TPoint { 6, 0 },
                TPoint { 0, 0 },
            },
        },
        // Double-clicking on trailing whitespace must not select across a line break.
        {   {   TPoint { 4, 2 },
                "foo \n bar",
                {   mouseDownEv({ 3, 0 }, meDoubleClick),
                    mouseUpEv({ 3, 0 }),
                },
                TPoint { 0, 0 },
            },
            {   {{ "f", "o", "o", " " }, { " ", "b", "a", "r" }},
                {{   6,   6,   6,   7 }, {   6,   6,   6,   6 }},
                TPoint { 4, 0 },
                TPoint { 0, 0 },
            },
        },
    };

    for (const auto &testCase : testCases)
    {
        TRect bounds({0, 0}, testCase.input.size);
        auto text = testCase.input.text;
        auto delta = testCase.input.delta;
        auto *editor = new TEditor(bounds, nullptr, nullptr, nullptr, 256);
        auto *group = new DrawableTestGroup(bounds, testCase.input.events);
        group->setState(sfExposed, true);
        group->getBuffer();
        group->insert(editor);

        editor->insertText(text.data(), text.size(), false);
        editor->setCurPtr(0, 0);
        TEvent inputEvent;
        group->getEvent(inputEvent);
        while (inputEvent.what != evNothing)
        {
            editor->handleEvent(inputEvent);
            group->getEvent(inputEvent);
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

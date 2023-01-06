#define Uses_TKeys
#include <tvision/tv.h>

#include <internal/far2l.h>

#include <test.h>

namespace tvision
{

class StrInputGetter : public InputGetter
{
    TStringView str;
    size_t i {0};

public:

    StrInputGetter(TStringView aStr) noexcept :
        str(aStr)
    {
    }

    int get() noexcept override
    {
        return i < str.size() ? str[i++] : -1;
    }

    void unget(int) noexcept override
    {
        if (i > 0)
            --i;
    }
};

struct ParseResultEvent
{
    ParseResult parseResult;
    TEvent ev;
};

static bool operator==(const ParseResultEvent &a, const ParseResultEvent &b)
{
    if (a.parseResult != b.parseResult)
        return false;
    if (a.ev.what != b.ev.what)
        return false;
    if (a.ev.what == evNothing)
        return true;
    if (a.ev.what == evKeyDown)
        return
            a.ev.keyDown.keyCode == b.ev.keyDown.keyCode &&
            a.ev.keyDown.controlKeyState == b.ev.keyDown.controlKeyState &&
            a.ev.keyDown.getText() == b.ev.keyDown.getText();
    abort();
}

static std::ostream &operator<<(std::ostream &os, const ParseResultEvent &p)
{
    os << "{";
    switch (p.parseResult)
    {
        case Rejected: os << "Rejected"; break;
        case Accepted: os << "Accepted"; break;
        case Ignored: os << "Ignored"; break;
    }
    os << ", {";
    printEventCode(os, p.ev.what);
    os << ", {{";
    printKeyCode(os, p.ev.keyDown.keyCode);
    os << "}, {";
    printControlKeyState(os, p.ev.keyDown.controlKeyState);
    os << "}, '" << p.ev.keyDown.getText() << "'}}";
    return os;
}

constexpr static TEvent keyDownEv(ushort keyCode, ushort controlKeyState, TStringView text)
{
    TEvent ev {};
    ev.what = evKeyDown;
    ev.keyDown.keyCode = keyCode;
    ev.keyDown.controlKeyState = controlKeyState;
    while (ev.keyDown.textLength <= sizeof(ev.keyDown.text) && ev.keyDown.textLength < text.size())
    {
        ev.keyDown.text[ev.keyDown.textLength] = text[ev.keyDown.textLength];
        ++ev.keyDown.textLength;
    }
    return ev;
}

TEST(TermIO, ShouldReadFar2lKeys)
{
    static constexpr char longString[1024*1024] = {0};
    static const TestCase<TStringView, ParseResultEvent> testCases[] =
    {
        {"", {Ignored}},
        {"\x07", {Ignored}},
        {"çśdfç32rç€v\x07", {Ignored}},
        {{longString, sizeof(longString)}, {Ignored}},
        {"AQBTAAAAAAAAAHMAAAB=", {Ignored}},
        {"AQBTAAAAAAAAAHMAAABLaa==", {Ignored}},
        {"AQBTAAAAAAAAAHMAAHMAAABL", {Ignored}},
        {"AQBTAAAAAAAAAHMAAABL", {Accepted, keyDownEv(0x0073, 0x0000, "s")}},
        {"AQBTAAAAAAAAAHMAAABL\x07", {Accepted, keyDownEv(0x0073, 0x0000, "s")}},
        {"AQC+AAAAAAAAAKwgAABL\x07", {Accepted, keyDownEv(kbNoKey, 0x0000, "€")}},
        {"AQBWAAAACAAAAAAAAABL\x07", {Accepted, keyDownEv(kbCtrlV, kbLeftCtrl, "")}},
        {"AQA5AAAACAAAADkAAABL\x07", {Accepted, keyDownEv(0x0039, kbLeftCtrl, "9")}},
        {"AQBWAAAACgAAAFYAAABL\x07", {Accepted, keyDownEv(kbAltV, kbLeftCtrl | kbLeftAlt, "")}},
        {"AQBWAAAACgAAAAAAAABL\x07", {Accepted, keyDownEv(kbAltV, kbLeftCtrl | kbLeftAlt, "")}},
    };

    for (auto &testCase : testCases)
    {
        StrInputGetter in(testCase.input);
        GetChBuf buf(in);
        ParseResultEvent actual {};
        InputState state {};
        actual.parseResult = parseFar2lInput(buf, actual.ev, state);
        expectResultMatches(actual, testCase);
    }
}

} // namespace tvision

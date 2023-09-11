#define Uses_TKeys
#include <tvision/tv.h>

#include <internal/terminal.h>

#include <test.h>
#include "terminal.test.h"

static bool operator==(const KeyDownEvent &a, const KeyDownEvent &b)
{
    return a.keyCode == b.keyCode && a.controlKeyState == b.controlKeyState
        && a.getText() == b.getText();
}

static std::ostream &operator<<(std::ostream &os, const KeyDownEvent &keyDown)
{
    os << "{{";
    printKeyCode(os, keyDown.keyCode);
    os << "}, {";
    printControlKeyState(os, keyDown.controlKeyState);
    os << "}, {";
    os << "\"" << keyDown.getText() << "\"";
    os << "}}";
    return os;
}

namespace tvision
{

TEST(TermIO, ShouldNormalizeKeys)
{
    static constexpr TestCase<KeyDownEvent> testCases[] =
    {
        {{'a', 0, {'a'}, 1}, {'a', 0, {'a'}, 1}},
        {{'a', kbShift, {'a'}, 1}, {'a', kbShift, {'a'}, 1}},
        {{'a', kbCtrlShift, {'a'}, 1}, {kbCtrlA, kbCtrlShift}},
        {{'a', kbLeftAlt, {'a'}, 1}, {kbAltA, kbLeftAlt}},
        {{kbIns, kbShift}, {kbShiftIns, kbShift}},
        {{kbBack, kbLeftCtrl | kbLeftAlt}, {kbAltBack, kbLeftCtrl | kbLeftAlt}},
        {{kbCtrlBack, kbLeftCtrl}, {kbCtrlBack, kbLeftCtrl}},
        {{kbCtrlBack}, {kbCtrlBack, kbCtrlShift}},
        {{kbIns, kbLeftCtrl | kbEnhanced}, {kbCtrlIns, kbLeftCtrl | kbEnhanced}},
        {{kbCtrlDel, kbLeftAlt}, {kbAltDel, kbCtrlShift | kbLeftAlt}},
    };

    for (auto &testCase : testCases)
    {
        KeyDownEvent actual = testCase.input;
        TermIO::normalizeKey(actual);
        expectResultMatches(actual, testCase);
    }
}

TEST(TermIO, ShouldReadWin32InputModeKeys)
{
    static const TestCase<TStringView, ParseResultEvent> testCases[] =
    {
        {"[65;30;65;1;16;1_", {Accepted, keyDownEv(0x1e41, kbShift, "A")}},
        {"[65;30;65;1;16_", {Accepted, keyDownEv(0x1e41, kbShift, "A")}},
        {"[16;42;0;0;0;1_", {Ignored}},
        {"[65;30;97;1;0;1_", {Accepted, keyDownEv(0x1e61, 0x0000, "a")}},
        {"[65;30;97;1_", {Accepted, keyDownEv(0x1e61, 0x0000, "a")}},
        {"[112;59;0;1;8;1_", {Accepted, keyDownEv(kbCtrlF1, kbLeftCtrl, "")}},
        {"[112;59;;1;8_", {Accepted, keyDownEv(kbCtrlF1, kbLeftCtrl, "")}},
        {"[112;59;0;0;8;1_", {Ignored}},
        // https://github.com/microsoft/terminal/issues/15083
        // SGR mouse event
        {"[0;0;27;1;0;1_"
         "\x1B[0;0;91;1;0;1_"
         "\x1B[0;0;60;1;0;1_"
         "\x1B[0;0;48;1;0;1_"
         "\x1B[0;0;59;1;0;1_"
         "\x1B[0;0;53;1;0;1_"
         "\x1B[0;0;50;1;0;1_"
         "\x1B[0;0;59;1;0;1_"
         "\x1B[0;0;49;1;0;1_"
         "\x1B[0;0;50;1;0;1_"
         "\x1B[0;0;77;1;0;1_", {Accepted, mouseEv({51, 11}, 0x0000, 0x0000, mbLeftButton, 0x0000)}},
    };

    for (auto &testCase : testCases)
    {
        StrInputGetter in(testCase.input);
        GetChBuf buf(in);
        ParseResultEvent actual {};
        InputState state {};
        actual.parseResult = TermIO::parseEscapeSeq(buf, actual.ev, state);
        expectResultMatches(actual, testCase);
        EXPECT_EQ(in.bytesLeft(), 0);
    }
}

} // namespace tvision

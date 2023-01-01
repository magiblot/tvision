#define Uses_TKeys
#include <tvision/tv.h>

#include <internal/terminal.h>

#include <test.h>

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

} // namespace tvision

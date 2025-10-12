#define Uses_TKeys
#include <tvision/tv.h>

#include <internal/termio.h>

#include <test.h>
#include "termio.test.h"
#include <vector>

namespace tvision
{

TEST(TermIO, ShouldNormalizeKeys)
{
    static const TestCase<TEvent> testCases[] =
    {
        {keyDownEv('a', 0x0000, "a"), keyDownEv('a', 0x0000, "a")},
        {keyDownEv('a', kbShift, "a"), keyDownEv('a', kbShift, "a")},
        {keyDownEv('a', kbCtrlShift, "a"), keyDownEv(kbCtrlA, kbCtrlShift)},
        {keyDownEv('a', kbLeftAlt, "a"), keyDownEv(kbAltA, kbLeftAlt)},
        {keyDownEv(kbIns, kbShift), keyDownEv(kbShiftIns, kbShift)},
        {keyDownEv(kbBack, kbLeftCtrl | kbLeftAlt), keyDownEv(kbAltBack, kbLeftCtrl | kbLeftAlt)},
        {keyDownEv(kbCtrlBack, kbRightCtrl), keyDownEv(kbCtrlBack, kbRightCtrl)},
        {keyDownEv(kbCtrlBack, 0x0000), keyDownEv(kbCtrlBack, kbLeftCtrl)},
        {keyDownEv(kbIns, kbLeftCtrl | kbEnhanced), keyDownEv(kbCtrlIns, kbLeftCtrl | kbEnhanced)},
        {keyDownEv(kbCtrlDel, kbLeftAlt), keyDownEv(kbAltDel, kbLeftCtrl | kbLeftAlt)},
    };

    for (auto &testCase : testCases)
    {
        TEvent actual = testCase.input;
        TermIO::normalizeKey(actual.keyDown);
        expectResultMatches(actual, testCase);
    }
}

TEST(TermIO, ShouldReadWin32InputModeKeys)
{
    static const TestCase<TStringView, std::vector<TEvent>> testCases[] =
    {
        {"\x1B[65;30;65;1;16;1_", {keyDownEv(0x1e41, kbShift, "A")}},
        {"\x1B[65;30;65;1;16_", {keyDownEv(0x1e41, kbShift, "A")}},
        {"\x1B[16;42;0;0;0;1_", {}},
        {"\x1B[65;30;97;1;0;1_", {keyDownEv(0x1e61, 0x0000, "a")}},
        {"\x1B[65;30;97;1_", {keyDownEv(0x1e61, 0x0000, "a")}},
        {"\x1B[112;59;0;1;8;1_", {keyDownEv(kbCtrlF1, kbLeftCtrl)}},
        {"\x1B[112;59;;1;8_", {keyDownEv(kbCtrlF1, kbLeftCtrl)}},
        {"\x1B[112;59;0;0;8;1_", {}},
        // https://github.com/microsoft/terminal/issues/15083
        { // SGR mouse event
            "\x1B[0;0;27;1;0;1_" // \x1B[<0;52;12M
            "\x1B[0;0;91;1;0;1_"
            "\x1B[0;0;60;1;0;1_"
            "\x1B[0;0;48;1;0;1_"
            "\x1B[0;0;59;1;0;1_"
            "\x1B[0;0;53;1;0;1_"
            "\x1B[0;0;50;1;0;1_"
            "\x1B[0;0;59;1;0;1_"
            "\x1B[0;0;49;1;0;1_"
            "\x1B[0;0;50;1;0;1_"
            "\x1B[0;0;77;1;0;1_",
            {mouseEv({51, 11}, 0x0000, 0x0000, mbLeftButton, 0x0000)},
        },
        { // Paste event
            "\x1B[17;29;0;1;8;1_" // Ctrl (press)
            "\x1B[16;42;0;1;24;1_" // Shift (press)
            "\x1B[0;0;27;1;0;1_" // \x1B[200~ (bracketed paste begin)
            "\x1B[0;0;91;1;0;1_"
            "\x1B[0;0;50;1;0;1_"
            "\x1B[0;0;48;1;0;1_"
            "\x1B[0;0;48;1;0;1_"
            "\x1B[0;0;126;1;0;1_"
            "\x1B[65;30;97;1;0;1_" // 'a' (press)
            "\x1B[65;30;97;0;0;1_" // 'a' (release)
            "\x1B[0;0;27;1;0;1_" // \x1B[201~ (bracketed paste end)
            "\x1B[0;0;91;1;0;1_"
            "\x1B[0;0;50;1;0;1_"
            "\x1B[0;0;48;1;0;1_"
            "\x1B[0;0;49;1;0;1_"
            "\x1B[0;0;126;1;0;1_"
            "\x1B[86;47;22;0;24;1_" // Ctrl+Shift+V (release)
            "\x1B[17;29;0;0;16;1_" // Shift (release)
            "\x1B[16;42;0;0;0;1_", // Ctrl (release)
            {keyDownEv(0x1e61, kbPaste, "a")},
        },
    };

    for (auto &testCase : testCases)
    {
        StrInputGetter in(testCase.input);
        std::vector<TEvent> actual {};
        InputState state {};
        while (true)
        {
            GetChBuf buf(in);
            TEvent ev {};
            ParseResult result = TermIO::parseEvent(buf, ev, state);
            if (state.bracketedPaste && ev.what == evKeyDown)
                ev.keyDown.controlKeyState |= kbPaste;

            if (result == Accepted)
                actual.push_back(ev);
            else if (result == Rejected)
                break;
        }
        expectResultMatches(actual, testCase);
    }
}

TEST(TermIO, ShouldReadKittyKeys)
{
    static const TestCase<TStringView, std::vector<TEvent>> testCases[] =
    {
        // a
        {"\x1B[97;;97u", {keyDownEv(0x0061, 0x0000, "a")}},
        // a + Caps
        {"\x1B[97;65;65u", {keyDownEv(0x0041, 0x0000, "A")}},
        // a + Shift
        {"\x1B[97:65;2;65u", {keyDownEv(0x0041, kbShift, "A")}},
        // a + Shift + Caps
        {"\x1B[97;66;97u", {keyDownEv(0x0061, kbShift, "a")}},
        // e + AltGr (ES)
        {"\x1B[8364::101;;8364u", {keyDownEv(kbNoKey, 0x0000, "€")}},
        // e + AltGr + Caps (ES)
        {"\x1B[8364::101;65;8364u", {keyDownEv(kbNoKey, 0x0000, "€")}},
        // ñ (ES)
        {"\x1B[241::59;;241u", {keyDownEv(0x00a4, 0x0000, "ñ")}},
        // ñ + Caps (ES)
        {"\x1B[241::59;65;209u", {keyDownEv(0x00a5, 0x0000, "Ñ")}},
        // ñ + Shift (ES)
        {"\x1B[241:209:59;2;209u", {keyDownEv(0x00a5, kbShift, "Ñ")}},
        // ñ + Shift + Caps (ES)
        {"\x1B[241::59;66;241u", {keyDownEv(0x00a4, kbShift, "ñ")}},
        // a + Alt
        {"\x1B[97;3u", {keyDownEv(kbAltA, kbLeftAlt)}},
        // a + Shift + Alt
        {"\x1B[97:65;4u", {keyDownEv(kbAltA, kbShift | kbLeftAlt)}},
        // a + Alt + Caps
        {"\x1B[97;67u", {keyDownEv(kbAltA, kbLeftAlt)}},
        // a + Shift + Alt + Caps
        {"\x1B[97;68u", {keyDownEv(kbAltA, kbShift | kbLeftAlt)}},
        // ñ + Alt (ES)
        {"\x1B[241::59;3u", {keyDownEv(0x00a4, kbLeftAlt, "ñ")}},
        // ñ + Shift + Alt (ES)
        {"\x1B[241:209:59;4u", {keyDownEv(0x00a5, kbShift | kbLeftAlt, "Ñ")}},
        // ñ + Shift + Ctrl + Alt + Caps (ES)
        {"\x1B[241::59;72u", {keyDownEv(0x00a4, kbShift | kbLeftCtrl | kbLeftAlt, "ñ")}},
        // - + Alt (ES)
        {"\x1B[45::47;3u", {keyDownEv(kbAltMinus, kbLeftAlt)}},
        // 0 + Shift + Alt (ES: 0 + Shift → =)
        {"\x1B[48:61;4u", {keyDownEv(kbAltEqual, kbShift | kbLeftAlt)}},
        // c + Ctrl
        {"\x1B[99;5u", {keyDownEv(kbCtrlC, kbLeftCtrl)}},
        // ф (RU)
        {"\x1B[1092::97;;1092u", {keyDownEv(0x0000, 0x0000, "ф")}},
        // ф + Caps (RU)
        {"\x1B[1092::97;65;1060u", {keyDownEv(0x0000, 0x0000, "Ф")}},
        // ф + Shift (RU)
        {"\x1B[1092:1060:97;2;1060u", {keyDownEv(0x0000, kbShift, "Ф")}},
        // ф + Shift + Caps (RU)
        {"\x1B[1092::97;66;1092u", {keyDownEv(0x0000, kbShift, "ф")}},
        // ф + Ctrl (RU)
        {"\x1B[1092::97;5u", {keyDownEv(kbCtrlA, kbLeftCtrl, "ф")}},
        // ф + Alt (RU)
        {"\x1B[1092::97;3u", {keyDownEv(kbAltA, kbLeftAlt, "ф")}},
        // ф + Shift + Alt (RU)
        {"\x1B[1092:1060:97;4u", {keyDownEv(kbAltA, kbShift | kbLeftAlt, "Ф")}},
        // Right
        {"\x1B[C", {keyDownEv(kbRight, 0x0000)}},
        // F2
        {"\x1B[Q", {keyDownEv(kbF2, 0x0000)}},
        // F3
        {"\x1B[13~", {keyDownEv(kbF3, 0x0000)}},
        // Del
        {"\x1B[3~", {keyDownEv(kbDel, 0x0000)}},
        // Tab
        {"\x1B[9u", {keyDownEv(kbTab, 0x0000)}},
        // Enter
        {"\x1B[13u", {keyDownEv(kbEnter, 0x0000)}},
        // Back
        {"\x1B[127u", {keyDownEv(kbBack, 0x0000)}},
        // Space + Ctrl
        {"\x1B[32;5u", {keyDownEv(0x0020, kbLeftCtrl, " ")}},
        // Enter + Ctrl
        {"\x1B[13;5u", {keyDownEv(kbCtrlEnter, kbLeftCtrl)}},
        // Left Shift
        {"\x1B[57441;2u", {}},
        // Left Ctrl
        {"\x1B[57442;5u", {}},
        // Left Alt
        {"\x1B[57443;3u", {}},
        // AltGr
        {"\x1B[57453u", {}},
    };

    for (auto &testCase : testCases)
    {
        StrInputGetter in(testCase.input);
        std::vector<TEvent> actual {};
        InputState state {};
        while (true)
        {
            GetChBuf buf(in);
            TEvent ev {};
            ParseResult result = TermIO::parseEvent(buf, ev, state);
            if (state.bracketedPaste && ev.what == evKeyDown)
                ev.keyDown.controlKeyState |= kbPaste;

            if (result == Accepted)
                actual.push_back(ev);
            else if (result == Rejected)
                break;
        }
        expectResultMatches(actual, testCase);
    }
}

} // namespace tvision

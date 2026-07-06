#define Uses_TColorAttr
#include <tvision/tv.h>

#include <type_traits>

#include <test.h>

#define check_trivial_copy_size_align(T, size, align) \
    EXPECT_EQ(std::is_trivially_copyable<T>(), true); \
    EXPECT_EQ(sizeof(T), size); \
    EXPECT_EQ(alignof(T), align);

#define check_convertible(T, U) \
    EXPECT_EQ((std::is_constructible<T, U>()), true); \
    EXPECT_EQ((std::is_convertible<T, U>()), true); \
    EXPECT_EQ((std::is_convertible<U, T>()), true);

TEST(Colors, StructsShouldHaveTheExpectedTrivialityAlignmentAndSize)
{
    // Use 'alignof(type)' rather than hardcoding the alignment values,
    // given that they may vary among systems.
    check_trivial_copy_size_align(TColor, 4, alignof(uint32_t));
    check_trivial_copy_size_align(TColorBIOS, 1, alignof(uint8_t));
    check_trivial_copy_size_align(TColorRGB, 4, alignof(uint32_t));
    check_trivial_copy_size_align(TColorXTerm, 1, alignof(uint8_t));
    check_trivial_copy_size_align(TAttrPair, 16, alignof(uint64_t));
    check_convertible(TColorBIOS, uint8_t);
    check_convertible(TColorRGB, uint32_t);
    check_convertible(TColorXTerm, uint8_t);
}

// TColorRGB

TEST(TColorRGB, DefaultConstructor) {
    constexpr TColorRGB rgb;

    EXPECT_EQ(rgb.getRed(), 0);
    EXPECT_EQ(rgb.getGreen(), 0);
    EXPECT_EQ(rgb.getBlue(), 0);
    EXPECT_EQ(static_cast<uint32_t>(rgb), 0u);
}

TEST(TColorRGB, ConstructorWithComponents) {
    constexpr TColorRGB rgb(127, 0, 187);

    EXPECT_EQ(rgb.getRed(), 127);
    EXPECT_EQ(rgb.getGreen(), 0);
    EXPECT_EQ(rgb.getBlue(), 187);
}

TEST(TColorRGB, ConstructorWithInteger) {
    constexpr TColorRGB rgb(0x7F00BB);

    EXPECT_EQ(rgb.getRed(), 0x7F);
    EXPECT_EQ(rgb.getGreen(), 0x00);
    EXPECT_EQ(rgb.getBlue(), 0xBB);
}

TEST(TColorRGB, IntegerConversionTruncatesHighBits) {
    constexpr TColorRGB rgb(0xAABBCCDD);

    EXPECT_EQ(rgb.getRed(), 0xBB);
    EXPECT_EQ(rgb.getGreen(), 0xCC);
    EXPECT_EQ(rgb.getBlue(), 0xDD);
    // Only the lower 24 bits are used.
    EXPECT_EQ(static_cast<uint32_t>(rgb), 0xBBCCDDU);
}

TEST(TColorRGB, SettersAndGetters) {
    TColorRGB rgb;
    rgb.setRed(255);
    rgb.setGreen(128);
    rgb.setBlue(64);

    EXPECT_EQ(rgb.getRed(), 255);
    EXPECT_EQ(rgb.getGreen(), 128);
    EXPECT_EQ(rgb.getBlue(), 64);
    EXPECT_EQ(static_cast<uint32_t>(rgb), 0xFF8040U);
}

// TColorBIOS

TEST(TColorBIOS, DefaultConstructor) {
    constexpr TColorBIOS bios;

    EXPECT_EQ(bios.getRed(), false);
    EXPECT_EQ(bios.getGreen(), false);
    EXPECT_EQ(bios.getBlue(), false);
    EXPECT_EQ(bios.getIntensity(), false);
    EXPECT_EQ(static_cast<uint8_t>(bios), 0U);
}

TEST(TColorBIOS, ConstructorWithValues) {
    constexpr TColorBIOS bios(0b1101); // i: 1, r: 1, g: 0, b: 1

    EXPECT_EQ(bios.getBlue(), true);
    EXPECT_EQ(bios.getGreen(), false);
    EXPECT_EQ(bios.getRed(), true);
    EXPECT_EQ(bios.getIntensity(), true);
}

TEST(TColorBIOS, IntegerConversionTruncatesHighBits) {
    constexpr TColorBIOS bios(0xAB);
    // Only the lower 4 bits are used.
    EXPECT_EQ(static_cast<uint8_t>(bios), 0x0BU);
}

TEST(TColorBIOS, SettersAndGetters) {
    TColorBIOS bios;
    bios.setRed(true);
    bios.setGreen(true);
    bios.setBlue(false);
    bios.setIntensity(true);

    EXPECT_EQ(bios.getRed(), true);
    EXPECT_EQ(bios.getGreen(), true);
    EXPECT_EQ(bios.getBlue(), false);
    EXPECT_EQ(bios.getIntensity(), true);
    EXPECT_EQ(static_cast<uint8_t>(bios), 0b1110U);
}

// TColorXTerm

TEST(TColorXTerm, DefaultConstructor) {
    constexpr TColorXTerm xterm;

    EXPECT_EQ(static_cast<uint8_t>(xterm), 0U);
}

TEST(TColorXTerm, ConstructorWithIndex) {
    constexpr TColorXTerm xterm(0xFE);

    EXPECT_EQ(static_cast<uint8_t>(xterm), 0xFEU);
}

// TColorConversion

TEST(TColorConversion, BIOStoXTerm16) {
    static constexpr TestCase<TColorBIOS, TColorXTerm> testCases[] =
    {
        {0x0, 0},  // Black
        {0x1, 4},  // Blue
        {0x4, 1},  // Red
        {0x8, 8},  // Bright black (dark gray)
        {0xF, 15}, // White
    };

    for (auto &testCase : testCases)
    {
        uint actual = TColorConversion::BIOStoXTerm16(testCase.input);
        expectResultMatches(actual, testCase);
    }
}

TEST(TColorConversion, XTerm16toBIOS) {
    static constexpr TestCase<TColorXTerm, TColorBIOS> testCases[] =
    {
        {0,  0x0}, // Black
        {1,  0x4}, // Blue
        {4,  0x1}, // Red
        {8,  0x8}, // Bright black (dark gray)
        {15, 0xF}, // White
    };

    for (auto &testCase : testCases)
    {
        uint actual = TColorConversion::XTerm16toBIOS(testCase.input);
        expectResultMatches(actual, testCase);
    }
}

TEST(TColorConversion, XTerm256toRGB) {
    static constexpr TestCase<TColorXTerm, TColorRGB> testCases[] =
    {
        {16,  0x000000}, // "Grey0" (first entry of the 6×6×6 cube)
        {17,  0x00005F}, // "NavyBlue" (second cube entry)
        {21,  0x0000FF}, // "Blue1"
        {46,  0x00FF00}, // "Green1"
        {59,  0x5F5F5F}, // "Grey37"
        {110, 0x87AFD7}, // "LightSkyBlue3"
        {196, 0xFF0000}, // "Red1"
        {208, 0xFF8700}, // "DarkOrange"
        {231, 0xFFFFFF}, // "Grey100" (last entry of the cube)
        {232, 0x080808}, // "Grey3" (first entry of the grayscale ramp)
        {240, 0x585858}, // "Grey35" (mid-point of the grayscale ramp)
        {255, 0xEEEEEE}, // "Grey93" (last entry of the grayscale ramp)
    };

    for (auto &testCase : testCases)
    {
        TColorRGB actual = TColorConversion::XTerm256toRGB(testCase.input);
        expectResultMatches(actual, testCase);
    }
}

TEST(TColorConversion, XTerm256toXTerm16) {
    static constexpr TestCase<TColorXTerm, TColorXTerm> testCases[] =
    {
        {21,  12}, // "Grey93"   (#0000FF)
        {46,  10}, // "Green1"   (#00FF00)
        {51,  14}, // "Cyan1"    (#00FFFF)
        {59,   8}, // "Grey37"   (#5F5F5F)
        {196,  9}, // "Red1"     (#FF0000)
        {201, 13}, // "Magenta1" (#FF00FF)
        {226, 11}, // "Yellow1"  (#FFFF00)
        {231, 15}, // "Grey100"  (#FFFFFF)
        {232,  0}, // "Grey3"    (#080808)
        {244,  8}, // "Grey50"   (#808080)
    };

    for (auto &testCase : testCases)
    {
        uint actual = TColorConversion::XTerm256toXTerm16(testCase.input);
        expectResultMatches(actual, testCase);
    }
}

TEST(TColorConversion, RGBtoXTerm256) {
    static constexpr TestCase<TColorRGB, TColorXTerm> testCases[] =
    {
        {0x000000,  16}, // "Grey0" (first entry of the 6×6×6 cube)
        {0x00005F,  17}, // "NavyBlue" (second cube entry)
        {0x0000FF,  21}, // "Blue1"
        {0x00FF00,  46}, // "Green1"
        {0x5F5F5F,  59}, // "Grey37"
        {0x87AFD7, 110}, // "LightSkyBlue3"
        {0xFF0000, 196}, // "Red1"
        {0xFF8700, 208}, // "DarkOrange"
        {0xFFFFFF, 231}, // "Grey100" (last entry of the cube)
        {0x080808, 232}, // "Grey3" (first entry of the grayscale ramp)
        {0x585858, 240}, // "Grey35" (mid-point of the grayscale ramp)
        {0xEEEEEE, 255}, // "Grey93" (last entry of the grayscale ramp)
    };

    for (auto &testCase : testCases)
    {
        uint actual = TColorConversion::RGBtoXTerm256(testCase.input);
        expectResultMatches(actual, testCase);
    }
}

TEST(TColorConversion, RGBtoXTerm16) {
    static constexpr TestCase<TColorRGB, TColorXTerm> testCases[] =
    {
        {0x800000,  1}, // Red
        {0x008000,  2}, // Green
        {0x808000,  3}, // Olive → Dark yellow
        {0x000080,  4}, // Blue
        {0x800080,  5}, // Magenta
        {0x008080,  6}, // Cyan
        {0x804000,  3}, // Brown → Dark yellow
        {0xFF0000,  9}, // Bright red
        {0x00FF00, 10}, // Bright green
        {0xFFFF00, 11}, // Bright yellow
        {0x0000FF, 12}, // Bright blue
        {0xFF00FF, 13}, // Bright magenta
        {0x00FFFF, 14}, // Bright cyan
        {0x000000,  0}, // Black
        {0x404040,  8}, // Bright black (dark gray)
        {0xC0C0C0,  7}, // Gray
        {0xFFFFFF, 15}, // White
    };

    for (auto &testCase : testCases)
    {
        uint actual = TColorConversion::RGBtoXTerm16(testCase.input);
        expectResultMatches(actual, testCase);
    }
}

// TColor

TEST(TColor, DefaultConstructorIsDefaultType) {
    constexpr TColor def;

    EXPECT_EQ(def.isDefault(), true);
    EXPECT_EQ(def.isBIOS(), false);
    EXPECT_EQ(def.isRGB(), false);
    EXPECT_EQ(def.isXTerm(), false);
}

TEST(TColor, DefaultColorEquality) {
    constexpr TColor def1;
    constexpr TColor def2 = TColorDefault();

    EXPECT_EQ(def1 == def2, true);
}

TEST(TColor, ConstructorFromCharOrUcharCreatesBIOS) {
    constexpr TColor bios('\xF');

    EXPECT_EQ(bios.isBIOS(), true);
    EXPECT_EQ(bios.asBIOS(), TColorBIOS(0xF));
}

TEST(TColor, ConstructorFromIntCreatesRGB) {
    constexpr TColor rgb(0x7F00BB);
    constexpr TColorRGB extracted = rgb.asRGB();

    EXPECT_EQ(rgb.isRGB(), true);
    EXPECT_EQ(extracted, TColorRGB(0x7F00BB));
}

TEST(TColor, ConstructorFromTColorBIOS) {
    constexpr TColorBIOS bios(0xD);
    constexpr TColor color(bios);

    EXPECT_EQ(color.isBIOS(), true);
    EXPECT_EQ(color.asBIOS(), bios);
}

TEST(TColor, ConstructorFromTColorRGB) {
    constexpr TColorRGB rgb(0x12, 0x34, 0x56);
    constexpr TColor color(rgb);

    EXPECT_EQ(color.isRGB(), true);
    EXPECT_EQ(color.asRGB(), rgb);
}

TEST(TColor, ConstructorFromTColorXTerm) {
    constexpr TColorXTerm xterm(0xAB);
    constexpr TColor color(xterm);

    EXPECT_EQ(color.isXTerm(), true);
    EXPECT_EQ(color.asXTerm(), xterm);
}

TEST(TColor, ToBIOSConversion) {
    static constexpr TestCase<TColor, TColorBIOS> testCases[] =
    {
        {TColorBIOS(0x7), TColorBIOS(0x7)}, // Gray
        {TColorRGB(0xEBDB34), TColorBIOS(0xE)}, // Yellow
        {TColorXTerm(201), TColorBIOS(0xD)}, // Magenta
    };

    for (auto &testCase : testCases)
    {
        uint actual = testCase.input.toBIOS(true);
        expectResultMatches(actual, testCase);
    }
}

TEST(TColor, DefaultToBIOSConversion) {
    constexpr TColor def;

    EXPECT_EQ(def.toBIOS(true), TColorBIOS(0x7)); // Foreground: gray.
    EXPECT_EQ(def.toBIOS(false), TColorBIOS(0x0)); // Blackground: black.
}

TEST(TColor, EqualityOperator) {
    constexpr TColor c1(0x7F00BB);
    constexpr TColor c2(0x7F00BB);
    constexpr TColor c3(0x7F00BC);

    EXPECT_EQ(c1, c2);
    EXPECT_NE(c1, c3);
}

TEST(TColor, DifferentTypesNotEqual) {
    constexpr TColor bios('\x0');
    constexpr TColor rgb(0x000000);
    // Different internal representation even if visually similar.
    EXPECT_NE(bios, rgb);
}

// TColorAttr

TEST(TColorAttr, DefaultInitialization) {
    // Zero-initialized should have default colors and no style.
    TColorAttr attr {};

    EXPECT_EQ(attr.getForeground().isDefault(), true);
    EXPECT_EQ(attr.getBackground().isDefault(), true);
    EXPECT_EQ(attr.getStyle(), 0);
}

TEST(TColorAttr, ZeroInitializationEqualsDefaultInitialization) {
    constexpr TColorAttr attr1 {};
    TColorAttr attr2;
    memset(&attr2, 0, sizeof(TColorAttr));

    EXPECT_EQ(attr2, attr1);
}

TEST(TColorAttr, ConstructorFromBIOSInt) {
    constexpr TColorAttr attr(0x3D); // foreground = 0xD, background = 0x3

    EXPECT_EQ(attr.getForeground().isBIOS(), true);
    EXPECT_EQ(attr.getForeground().asBIOS(), TColorBIOS(0xD));
    EXPECT_EQ(attr.getBackground().isBIOS(), true);
    EXPECT_EQ(attr.getBackground().asBIOS(), TColorBIOS(0x3));
    EXPECT_EQ(attr.getStyle(), 0);
}

TEST(TColorAttr, ConstructorWithColorsAndStyle) {
    constexpr TColor fg(0xFF0000);
    constexpr TColor bg('\x1');
    constexpr TColorAttr attr(fg, bg, slBold | slItalic);

    EXPECT_EQ(attr.getForeground().isRGB(), true);
    EXPECT_EQ(attr.getForeground().asRGB(), TColorRGB(0xFF0000));
    EXPECT_EQ(attr.getBackground().isBIOS(), true);
    EXPECT_EQ(attr.getBackground().asBIOS(), TColorBIOS(0x1));
    EXPECT_EQ(attr.getStyle(), slBold | slItalic);
}

TEST(TColorAttr, GettersAndSetters) {
    TColorAttr attr;
    constexpr TColor newFg(0x123456);
    constexpr TColor newBg('\x7');
    attr.setForeground(newFg);
    attr.setBackground(newBg);
    attr.setStyle(slUnderline);

    EXPECT_EQ(attr.getForeground().isRGB(), true);
    EXPECT_EQ(attr.getForeground().asRGB(), TColorRGB(0x123456));
    EXPECT_EQ(attr.getBackground().isBIOS(), true);
    EXPECT_EQ(attr.getBackground().asBIOS(), TColorBIOS(0x07));
    EXPECT_EQ(attr.getStyle(), slUnderline);
}

TEST(TColorAttr, EqualityOperator) {
    constexpr TColorAttr a1('\x7', '\xF', slBold);
    constexpr TColorAttr a2('\x7', '\xF', slBold);
    constexpr TColorAttr a3('\x7', '\xF', slItalic);

    EXPECT_EQ(a2, a1);
    EXPECT_NE(a3, a1);
}

TEST(TColorAttr, ComparisonWithInt) {
    constexpr TColorAttr attr(0x3D);

    EXPECT_EQ(attr, 0x3D);
    EXPECT_NE(attr, 0x3E);
}

TEST(TColorAttr, ReversedMethod) {
    constexpr TColorAttr attrWithDefaultColors {};
    constexpr TColorAttr attrWithStyle('\x7', '\xF', slBold);

    EXPECT_EQ(attrWithDefaultColors.reversed(), TColorAttr({}, {}, slReverse));
    EXPECT_EQ(attrWithStyle.reversed(), TColorAttr('\xF', '\x7', slBold));
}

TEST(TColorAttr, UCharOperator) {
    constexpr TColorAttr biosAttr('\x7', '\xF');
    constexpr TColorAttr nonBiosAttr(0xFF0000, '\xF');

    EXPECT_EQ(static_cast<uchar>(biosAttr), 0xF7);
    EXPECT_EQ(static_cast<uchar>(nonBiosAttr), 0x5F);
}

TEST(TColorAttr, ConstructorFromAttrPair) {
    constexpr TColorAttr low(0xFF0000, '\x1', slBold);
    constexpr TColorAttr high('\x7');
    constexpr TAttrPair pair(low, high);
    constexpr TColorAttr fromPair(pair);

    EXPECT_EQ(fromPair, low);
}

TEST(TColorAttr, ToBIOS) {
    constexpr TColorAttr biosAttr('\x7', '\xF');
    constexpr TColorAttr rgbAttr(0xFF0000, 0x000080);
    constexpr TColorAttr attrWithStyle('\x7', '\xF', slBold);

    EXPECT_EQ(biosAttr.toBIOS(), 0xF7); // Same as operator uchar() for pure BIOS attrs.
    EXPECT_EQ(rgbAttr.toBIOS(), 0x1C); // Quantized: bright red fg, blue bg.
    EXPECT_EQ(attrWithStyle.toBIOS(), 0xF7); // Style flags are ignored.
}

TEST(TColorAttr, LeftShiftOperator) {
    constexpr TColorAttr attr('\x7');
    constexpr TAttrPair pair = attr << 8;
    constexpr TAttrPair shifted = attr << 4;

    EXPECT_EQ(pair[0], TColorAttr(0));
    EXPECT_EQ(pair[1], attr);
    EXPECT_EQ(static_cast<ushort>(shifted), 0x70);
}

// TAttrPair

TEST(TAttrPair, ConstructorFromInt) {
    constexpr TAttrPair pair(0x3D21);

    EXPECT_EQ(pair[0], TColorAttr(0x21));
    EXPECT_EQ(pair[1], TColorAttr(0x3D));
}

TEST(TAttrPair, ConstructorFromColorAttrs) {
    constexpr TColorAttr low('\x7');
    constexpr TColorAttr high('\xF');
    constexpr TAttrPair pair(low, high);

    EXPECT_EQ(pair[0], low);
    EXPECT_EQ(pair[1], high);
}

TEST(TAttrPair, ConstructorWithDefaultHigh) {
    constexpr TColorAttr low('\x7');
    constexpr TAttrPair pair(low);

    EXPECT_EQ(pair[0], low);
    EXPECT_EQ(pair[1], TColorBIOS(0x00)); // high defaults to TColorAttr(0) = BIOS color attribute 0x00
}

TEST(TAttrPair, UShortConversion) {
    constexpr TColorAttr low('\x1');
    constexpr TColorAttr high('\x2');
    constexpr TAttrPair pair(low, high);

    EXPECT_EQ(static_cast<ushort>(pair), 0x0201);
}

TEST(TAttrPair, RightShiftOperator) {
    constexpr TColorAttr low('\x1');
    constexpr TColorAttr high('\x2');
    constexpr TAttrPair pair(low, high);
    constexpr TAttrPair shifted = pair >> 8;

    EXPECT_EQ(shifted[0], high);
    EXPECT_EQ(shifted[1], TColorAttr(0));
}

TEST(TAttrPair, OrEqualsOperator) {
    constexpr TColorAttr highAttr('\x5');
    constexpr TColorAttr newLowAttr('\x7');
    TAttrPair pair('\x0', highAttr);
    pair |= newLowAttr;

    EXPECT_EQ(pair[0], newLowAttr);
    EXPECT_EQ(pair[1], highAttr);
}

#define Uses_TScreenCell
#include <tvision/tv.h>

#include <type_traits>

#include <test.h>

#define check_trivial_construct_copy_size_align(T, size, align) \
    EXPECT_EQ(std::is_trivially_default_constructible<T>(), true); \
    EXPECT_EQ(std::is_trivially_copyable<T>(), true); \
    EXPECT_EQ(sizeof(T), size); \
    EXPECT_EQ(alignof(T), align);

TEST(Scrncell, StructsShouldHaveTheExpectedTrivialityAlignmentAndSize)
{
    // Use 'alignof(type)' rather than hardcoding the alignment values,
    // given that they may vary among systems.
    check_trivial_construct_copy_size_align(TColorAttr, 8, alignof(uint64_t));
    check_trivial_construct_copy_size_align(TScreenCharacter, 16, alignof(uint8_t));
    check_trivial_construct_copy_size_align(TScreenCell, 24, alignof(uint64_t));
}

// TScreenCharacter

TEST(TScreenCharacter, CharConstructor) {
    constexpr TScreenCharacter ch('A');

    EXPECT_EQ(ch.getText(), TStringView("A", 1));
    EXPECT_EQ(ch.isWide(), false);
    EXPECT_EQ(ch.isWideCharTrail(), false);
}

TEST(TScreenCharacter, InitWithCharResetsState) {
    TScreenCharacter ch;
    ch.initAsWideCharTrail();
    ch.initWithChar('B');

    EXPECT_EQ(ch.getText(), TStringView("B", 1));
    EXPECT_EQ(ch.isWide(), false);
    EXPECT_EQ(ch.isWideCharTrail(), false);
}

TEST(TScreenCharacter, InitWithMultiByteCharStringView) {
    TScreenCharacter ch;
    ch.initWithMultiByteChar("\xC3\xA9"); // 'é'

    EXPECT_EQ(ch.getText(), TStringView("\xC3\xA9", 2));
    EXPECT_EQ(ch.isWide(), false);
    EXPECT_EQ(ch.isWideCharTrail(), false);
}

TEST(TScreenCharacter, InitWithMultiByteCharStringViewWide) {
    TScreenCharacter ch;
    ch.initWithMultiByteChar("\xE4\xB8\xAD", true); // '中', double-width

    EXPECT_EQ(ch.getText(), TStringView("\xE4\xB8\xAD", 3));
    EXPECT_EQ(ch.isWide(), true);
    EXPECT_EQ(ch.isWideCharTrail(), false);
}

TEST(TScreenCharacter, InitWithMultiByteCharStringViewResetsState) {
    TScreenCharacter ch;
    ch.initAsWideCharTrail();
    ch.initWithMultiByteChar("\xC3\xA9");

    EXPECT_EQ(ch.getText(), TStringView("\xC3\xA9", 2));
    EXPECT_EQ(ch.isWideCharTrail(), false);
}

TEST(TScreenCharacter, InitWithMultiByteCharUInt32) {
    TScreenCharacter ch;
    ch.initWithMultiByteChar(*(const uint32_t *) "\xE4\xB8\xAD"); // '中'

    EXPECT_EQ(ch.getText(), TStringView("\xE4\xB8\xAD", 3));
    EXPECT_EQ(ch.isWide(), false);
}

TEST(TScreenCharacter, InitAsWideCharTrailResetsState) {
    TScreenCharacter ch;
    ch.initWithMultiByteChar("\xE4\xB8\xAD", true); // Wide char
    ch.initAsWideCharTrail();

    EXPECT_EQ(ch.isWideCharTrail(), true);
    EXPECT_EQ(ch.isWide(), false);
}

TEST(TScreenCharacter, AppendZeroWidthChar) {
    TScreenCharacter ch('a');
    ch.appendZeroWidthChar("\xCC\x80"); // Combining grave U+0300

    EXPECT_EQ(ch.getText(), TStringView("a\xCC\x80", 3));
    EXPECT_EQ(ch.isWide(), false);
}

TEST(TScreenCharacter, AppendZeroWidthCharPromotesNullToSpace) {
    TScreenCharacter ch {};
    ch.appendZeroWidthChar("\xCC\x80");

    EXPECT_EQ(ch.getText()[0], ' ');
}

TEST(TScreenCharacter, AppendZeroWidthCharNoEffectAfterOverflow) {
    TScreenCharacter ch;
    ch.initWithMultiByteChar("\xF0\x9F\x98\x80"); // '😀', 4 bytes
    ch.appendZeroWidthChar("\xF3\xA0\x84\x80");   // +4 = 8 bytes
    ch.appendZeroWidthChar("\xF3\xA0\x84\x80");   // +4 = 12 bytes
    ch.appendZeroWidthChar("\xCC\x80");            // +2 = 14 bytes; next would overflow
    ch.appendZeroWidthChar("\xCC\x80");            // overflow: sets fOverflow, ignored
    ch.appendZeroWidthChar("\x2E");                // also ignored: fOverflow is set

    EXPECT_EQ(ch.getText().size(), 14U);
}

// TScreenCell

TEST(TScreenCell, ConstructorWithCharAndAttr) {
    constexpr TScreenCharacter ch('X');
    constexpr TColorAttr attr('\x7', '\xF');
    constexpr TScreenCell cell(ch, attr);

    EXPECT_EQ(cell.character.getText(), TStringView("X", 1));
    EXPECT_EQ(cell.attribute, attr);
}

TEST(TScreenCell, ConstructorFromBIOS) {
    constexpr TScreenCell cell(0x1F41); // char = 0x41 ('A'), attr = 0x1F

    EXPECT_EQ(cell.character.getText(), TStringView("A", 1));
    EXPECT_EQ(cell.attribute, TColorAttr(0x1F));
}

TEST(TScreenCell, EqualityOperator) {
    constexpr TScreenCell a('A', 0x07);
    constexpr TScreenCell b(TScreenCharacter('A'), TColorAttr(0x07));
    constexpr TScreenCell diffChar(TScreenCharacter('B'), TColorAttr(0x07));
    constexpr TScreenCell diffAttr(TScreenCharacter('A'), TColorAttr(0x70));

    EXPECT_EQ(a, b);
    EXPECT_NE(a, diffChar);
    EXPECT_NE(a, diffAttr);
}

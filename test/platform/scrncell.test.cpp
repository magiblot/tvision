#define Uses_TScreenCell
#include <tvision/tv.h>

#include <type_traits>

#include <test.h>

#define check_trivial_size_align(T, size, align) \
    EXPECT_EQ(std::is_trivial<T>(), true); \
    EXPECT_EQ(sizeof(T), size); \
    EXPECT_EQ(alignof(T), align);

TEST(Scrncell, StructsShouldBeTrivialAndHaveTheExpectedAlignmentAndSize)
{
    check_trivial_size_align(TColorDesired, 4, 4);
    check_trivial_size_align(TColorAttr, 8, 8);
    check_trivial_size_align(TAttrPair, 16, 8);
    check_trivial_size_align(TCellChar, 16, 1);
    check_trivial_size_align(TScreenCell, 24, 8);
    check_trivial_size_align(TColorBIOS, 1, 1);
    check_trivial_size_align(TColorRGB, 4, 4);
    check_trivial_size_align(TColorXTerm, 1, 1);
}

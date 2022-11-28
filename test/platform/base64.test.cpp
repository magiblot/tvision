#include <internal/base64.h>

#include <test.h>

namespace tvision
{

TEST(Base64, ShouldDecodeProperly)
{
    static constexpr TestCase<TStringView> testCases[] =
    {
        {""        , ""      },
        {"Zg=="    , "f"     },
        {"Zm8="    , "fo"    },
        {"Zm9v"    , "foo"   },
        {"Zm9vYg==", "foob"  },
        {"Zm9vYmE=", "fooba" },
        {"Zm9vYmFy", "foobar"},
        {"Zg"      , "f"     },
        {"Zm8"     , "fo"    },
        {"Zm9vYg"  , "foob"  },
        {"Zm9vYmE" , "fooba" },
    };
    for (auto &testCase : testCases)
    {
        auto &&actual = from_base64(testCase.input);
        expectResultMatches(actual, testCase);
    }
}

} // namespace tvision

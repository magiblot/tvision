#ifndef TVISION_TERMIO_TEST_H
#define TVISION_TERMIO_TEST_H

#include <internal/termio.h>
#include <test_operators.h>

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

    int bytesLeft() noexcept
    {
        return str.size() - i;
    }
};

struct ParseResultEvent
{
    ParseResult parseResult;
    TEvent ev;
};

inline bool operator==(const ParseResultEvent &a, const ParseResultEvent &b)
{
    if (a.parseResult != b.parseResult)
        return false;
    if (a.parseResult == Ignored)
        return true;
    return a.ev == b.ev;
}

inline std::ostream &operator<<(std::ostream &os, const ParseResultEvent &p)
{
    os << "{";
    switch (p.parseResult)
    {
        case Rejected: os << "Rejected"; break;
        case Ignored: os << "Ignored"; break;
        case Accepted: os << "Accepted, " << p.ev; break;
    }
    os << "}";
    return os;
}

} // namespace tvision

#endif // TVISION_TERMIO_TEST_H

#ifndef TVISION_BASE64_H
#define TVISION_BASE64_H

#include <tvision/tv.h>
#include <string>

namespace tvision
{

// Returns the number of bytes written into 'output'.
// Pre: 'output.size()' is no less than 3/4 of 'input.size()'.
size_t from_base64(TStringView input, TSpan<uint8_t> output) noexcept;

inline std::string from_base64(TStringView input)
{
    std::string result;
    enum { k = 128 };
    uint8_t buf[3*k];
    if (!input.empty())
    {
        size_t i = 0;
        do
        {
            result.append((char *) buf, from_base64(input.substr(i*4*k, 4*k), buf));
        } while (i++ < (input.size() - 1)/(4*k));
    }
    return result;
}

} // namespace tvision

#endif // TVISION_BASE64_H

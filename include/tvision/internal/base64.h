#ifndef TVISION_BASE64_H
#define TVISION_BASE64_H

#include <tvision/tv.h>
#include <string>

namespace tvision
{

std::string from_base64(TSpan<const uint8_t> input);

} // namespace tvision

#endif // TVISION_BASE64_H

#ifndef GETENV_H
#define GETENV_H

#include <cstdlib>
#include <type_traits>
#include <string>

template<typename T>
static inline T getEnv(const char* name, T def = T{});

template<>
int getEnv<int>(const char* name, int def)
{
    const char* body = std::getenv(name);
    if (body) {
        char* end;
        auto i = strtol(body, &end, 0);
        if (body != end)
            return i;
    }
    return def;
}

template<>
std::string getEnv<std::string>(const char* name, std::string def)
{
    const char* body = std::getenv(name);
    return body ? body : def;
}

#endif

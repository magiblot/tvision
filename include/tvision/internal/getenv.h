#ifndef GETENV_H
#define GETENV_H

#include <cstdlib>

template<typename T>
inline T getEnv(const char* name, T def = T{})
{
    const char* body = std::getenv(name);
    return body ? body : def;
}

template<>
inline int getEnv<int>(const char* name, int def)
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

#endif

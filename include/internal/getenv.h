#ifndef GETENV_H
#define GETENV_H

#include <cstdlib>
#include <type_traits>
#include <string>

template<typename T>
static inline T getEnv(const char* name);

template<>
int getEnv<int>(const char* name)
{
    const char* body = std::getenv(name);
    return body ? atoi(body) : 0;
}

template<>
std::string getEnv<std::string>(const char* name)
{
    const char* body = std::getenv(name);
    return body ? body : "";
}

#endif

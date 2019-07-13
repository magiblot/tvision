#ifndef FILESYS_H
#define FILESYS_H

#include <filesystem>

namespace fs = std::filesystem;

fs::path getCurDir(); // tdircoll.cpp
fs::path fexpand(const fs::path&); // tfillist.cpp

/* Quick workaround for C++'s inhability to translate std::filesystem::file_time_type
 * into an easily readable format such as std::time_t. */
#include <chrono>
#include <ctime>

template<typename src_time_point>
std::time_t to_time_t(const src_time_point &tp)
{
    const auto src_now = src_time_point::clock::now();
    const auto sys_now = std::chrono::system_clock::now();
    return std::chrono::system_clock::to_time_t(sys_now + (tp - src_now));
}

#endif

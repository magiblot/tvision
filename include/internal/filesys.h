#ifndef FILESYS_H
#define FILESYS_H

#include <filesystem>

namespace fs = std::filesystem;

fs::path getCurDir(); // tdircoll.cpp
fs::path fexpand(const fs::path&); // tfillist.cpp

#endif

cmake_minimum_required (VERSION 3.12)
project (tvision)

include_directories(
include
include/windows
)

set(CMAKE_CXX_FLAGS "-m32 -O0 -g -gstabs+ --std=c++11 -fpermissive")

add_compile_definitions(__FLAT__ __cdecl= __stdcall=)

file(GLOB_RECURSE SOURCES "source/tvision/*.cpp")

add_executable(hello
hello.cpp
${SOURCES}
)
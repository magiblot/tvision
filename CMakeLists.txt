cmake_minimum_required (VERSION 3.12)
project (tvision)

include_directories(
include
include/windows
)

set(CMAKE_CXX_FLAGS "-m32 -O0 -g -gstabs+ --std=c++11 -fpermissive")

add_compile_definitions(__FLAT__ __cdecl= __stdcall=)

file(GLOB_RECURSE TVSOURCE "${CMAKE_CURRENT_SOURCE_DIR}/source/tvision/*.cpp")
list(REMOVE_ITEM TVSOURCE "${CMAKE_CURRENT_SOURCE_DIR}/source/tvision/geninc.cpp")

add_executable(hello
hello.cpp
${TVSOURCE}
)

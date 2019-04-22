cmake_minimum_required (VERSION 3.12)
project (tvision)

include_directories(
include
include/override
include/windows
)

set(CMAKE_CXX_FLAGS "-O0 -g -gstabs+ --std=c++11 -fpermissive -lncurses")

add_compile_definitions(__FLAT__ __cdecl= __stdcall=)

file(GLOB_RECURSE TVSOURCE "${CMAKE_CURRENT_SOURCE_DIR}/source/*/*.cpp")
list(REMOVE_ITEM TVSOURCE "${CMAKE_CURRENT_SOURCE_DIR}/source/tvision/geninc.cpp")

add_executable(hello
hello.cpp
${TVSOURCE}
)

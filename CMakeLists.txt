cmake_minimum_required (VERSION 3.12)
project (tvision)

include_directories(
include
include/override
include/windows
)

set(CMAKE_CXX_FLAGS "-O0 -g -gstabs+ --std=c++11 -fpermissive -lncurses -Wno-missing-braces -Wno-unknown-pragmas -Wno-unused-function -Wno-parentheses -Wall")

add_compile_definitions(__FLAT__ __cdecl= __stdcall=)

file(GLOB_RECURSE TVSOURCE "${CMAKE_CURRENT_SOURCE_DIR}/source/*/*.cpp")
list(REMOVE_ITEM TVSOURCE "${CMAKE_CURRENT_SOURCE_DIR}/source/tvision/geninc.cpp")
file(GLOB_RECURSE HELLOSRC "${CMAKE_CURRENT_SOURCE_DIR}/hello.cpp")
file(GLOB_RECURSE TVEDITSRC "${CMAKE_CURRENT_SOURCE_DIR}/examples/tvedit/*.cpp")
file(GLOB_RECURSE TVDEMOSRC "${CMAKE_CURRENT_SOURCE_DIR}/examples/tvdemo/*.cpp")


add_library(tvision STATIC ${TVSOURCE})
add_executable(hello ${HELLOSRC})
add_executable(tvedit ${TVEDITSRC})
add_executable(tvdemo ${TVDEMOSRC})

target_link_libraries(hello tvision)
target_link_libraries(tvedit tvision)
target_link_libraries(tvdemo tvision)

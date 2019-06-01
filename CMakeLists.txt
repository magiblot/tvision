cmake_minimum_required (VERSION 3.5)
project (tvision)

include_directories(
include
include/override
include/windows
)

set(CMAKE_CXX_FLAGS "-O0 -g --std=c++11 -fpermissive -Wno-missing-braces -Wno-unknown-pragmas -Wno-unused-function -Wno-parentheses -Wall -D__FLAT__ -D__cdecl= -D__stdcall=")

file(GLOB_RECURSE TVSOURCE "${CMAKE_CURRENT_SOURCE_DIR}/source/*/*.cpp")
list(REMOVE_ITEM TVSOURCE "${CMAKE_CURRENT_SOURCE_DIR}/source/tvision/geninc.cpp")
file(GLOB_RECURSE HELLOSRC "${CMAKE_CURRENT_SOURCE_DIR}/hello.cpp")
file(GLOB_RECURSE TVEDITSRC "${CMAKE_CURRENT_SOURCE_DIR}/examples/tvedit/*.cpp")
file(GLOB_RECURSE TVDEMOSRC "${CMAKE_CURRENT_SOURCE_DIR}/examples/tvdemo/*.cpp")

find_library(NCURSESW ncursesw)
find_library(PTHREAD pthread)
set(LIBS tvision ${NCURSESW} ${PTHREAD})

# Optional dependencies
find_library(GPM gpm)
if (GPM)
    list(APPEND LIBS ${GPM})
    string(APPEND CMAKE_CXX_FLAGS " -DHAVE_GPM")
endif()

add_library(tvision STATIC ${TVSOURCE})
add_executable(hello ${HELLOSRC})
add_executable(tvedit ${TVEDITSRC})
add_executable(tvdemo ${TVDEMOSRC})

target_link_libraries(hello ${LIBS})
target_link_libraries(tvedit ${LIBS})
target_link_libraries(tvdemo ${LIBS})

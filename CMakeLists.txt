cmake_minimum_required (VERSION 3.5)
project (tvision)

add_compile_options(
    --std=c++17 -O2 -fpermissive
#     -g
    -Wall -Wno-missing-braces -Wno-unknown-pragmas -Wno-unused-function -Wno-pragmas -Wno-reorder -Wno-deprecated
    # Enable the following if your system supports it:
#     -flto
)

include_directories(
include
include/override
)

file(GLOB_RECURSE TVSOURCE "${CMAKE_CURRENT_SOURCE_DIR}/source/*/*.cpp")
list(REMOVE_ITEM TVSOURCE "${CMAKE_CURRENT_SOURCE_DIR}/source/tvision/geninc.cpp")
file(GLOB_RECURSE HELLOSRC "${CMAKE_CURRENT_SOURCE_DIR}/hello.cpp")
file(GLOB_RECURSE TVEDITSRC "${CMAKE_CURRENT_SOURCE_DIR}/examples/tvedit/*.cpp")
file(GLOB_RECURSE TVDEMOSRC "${CMAKE_CURRENT_SOURCE_DIR}/examples/tvdemo/*.cpp")
file(GLOB_RECURSE TVDIRSRC "${CMAKE_CURRENT_SOURCE_DIR}/examples/tvdir/*.cpp")
file(GLOB_RECURSE TVHELPSRC "${CMAKE_CURRENT_SOURCE_DIR}/examples/tvhelp/*.cpp")

add_library(tvision STATIC ${TVSOURCE})
add_executable(hello ${HELLOSRC})
add_executable(tvedit ${TVEDITSRC})
add_executable(tvdemo ${TVDEMOSRC})
add_executable(tvdir ${TVDIRSRC})
add_executable(tvhc ${TVHELPSRC})

# Libraries

find_library(NCURSESW ncursesw)
set(LIBS tvision ${NCURSESW})

# Optional dependencies
find_library(GPM gpm)
if (GPM)
    list(APPEND LIBS ${GPM})
    target_compile_definitions(tvision PRIVATE HAVE_GPM)
endif()

target_link_libraries(hello ${LIBS})
target_link_libraries(tvedit ${LIBS})
target_link_libraries(tvdemo ${LIBS})
target_link_libraries(tvdir ${LIBS})
target_link_libraries(tvhc ${LIBS})

target_compile_options(tvdir PRIVATE -Wno-stringop-truncation)

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16.0")
    target_precompile_headers(tvision PRIVATE "include/tvision/tv.h")
endif()

cmake_minimum_required (VERSION 3.5)
cmake_policy(SET CMP0091 NEW)

project(tvision)

# Platform disambiguation and global compilation options

set(TV_PLATF_UNIX 1)
set(TV_PLATF_WIN32 2)

if (WIN32)
    set(TV_PLATF ${TV_PLATF_WIN32})
else()
    set(TV_PLATF ${TV_PLATF_UNIX})
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # Static RTL linkage for MSVC.
#     set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    add_compile_options(
        /wd4068 /wd4166 /wd4250 /wd4267 /wd4996
        /std:c++17 /permissive- /Zc:__cplusplus
    )
    add_definitions(
        -D_CRT_SECURE_NO_WARNINGS
        # Why do I get syntax errors unless I set this?
        -D_HAS_EXCEPTIONS=0
    )
else()
    add_compile_options(
    #     -g
        --std=c++17 -O2
        -Wall -Wno-unknown-pragmas -Wno-pragmas -Wno-reorder -Wno-deprecated
        # You may enable the following if your system supports it:
    #     -flto
    )
endif()

include_directories(
    include
    include/tvision
    include/tvision/borland
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

set(LIBS tvision)

if (TV_PLATF EQUAL TV_PLATF_UNIX)
    find_library(NCURSESW ncursesw REQUIRED)
    if (NCURSESW)
        list(APPEND LIBS ${NCURSESW})
        target_compile_definitions(tvision PRIVATE HAVE_NCURSES)
    endif()
    # Optional dependencies
    find_library(GPM gpm)
    if (GPM)
        list(APPEND LIBS ${GPM})
        target_compile_definitions(tvision PRIVATE HAVE_GPM)
    endif()
endif()

target_link_libraries(hello ${LIBS})
target_link_libraries(tvedit ${LIBS})
target_link_libraries(tvdemo ${LIBS})
target_link_libraries(tvdir ${LIBS})
target_link_libraries(tvhc ${LIBS})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(tvdir PRIVATE -Wno-stringop-truncation)
endif()

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16.0")
    target_precompile_headers(tvision PRIVATE "include/tvision/tv.h")
    target_precompile_headers(hello PRIVATE "include/tvision/tv.h")
    target_precompile_headers(tvedit REUSE_FROM hello)
    target_precompile_headers(tvdemo REUSE_FROM hello)
    target_precompile_headers(tvdir REUSE_FROM hello)
    target_precompile_headers(tvhc REUSE_FROM hello)
endif()

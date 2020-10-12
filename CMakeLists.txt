# Options summary:
#
# * TV_BUILD_EXAMPLES (default ON) to enable building examples.
# * TV_BUILD_USING_GPM (default ON) (only on linux) to enable linking to libgpm
#   (warning if not found).

cmake_minimum_required (VERSION 3.5)
if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.15.0")
    cmake_policy(SET CMP0091 NEW)
endif()

project(tvision)

# Platform disambiguation and global compilation options

set(TV_PLATF_UNIX 1)
set(TV_PLATF_WIN32 2)

if (WIN32)
    set(TV_PLATF ${TV_PLATF_WIN32})
else()
    set(TV_PLATF ${TV_PLATF_UNIX})
    option(TV_BUILD_USING_GPM "Use GPM" ON)
endif()

option(TV_BUILD_EXAMPLES "Build example apps" ON)

# tvision_BINARY_DIR derived from project() name
#
set(EXECUTABLE_OUTPUT_PATH ${tvision_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")
set(LIBRARY_OUTPUT_PATH    ${tvision_BINARY_DIR}/lib CACHE INTERNAL "Single output directory for building all libraries.")

message(STATUS "Library output: ${LIBRARY_OUTPUT_PATH}")
message(STATUS "Binary output : ${EXECUTABLE_OUTPUT_PATH}")
message(STATUS "Install path  : ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Build Examples: ${TV_BUILD_EXAMPLES}")
message(STATUS "Build w/GPM   : ${TV_BUILD_USING_GPM}")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # Static RTL linkage for MSVC (requires CMake >= 3.15).
#     set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    add_compile_options(
        /wd4068 /wd4146 /wd4166 /wd4244 /wd4250 /wd4267 /wd4996
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
    include/tvision/compat
)

if (NOT WIN32 AND NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Linux"))
    include_directories(
        include/tvision/compat/malloc
    )
endif()

file(GLOB_RECURSE TVSOURCE "${CMAKE_CURRENT_SOURCE_DIR}/source/*/*.cpp")
list(REMOVE_ITEM TVSOURCE "${CMAKE_CURRENT_SOURCE_DIR}/source/tvision/geninc.cpp")
file(GLOB_RECURSE HELLOSRC "${CMAKE_CURRENT_SOURCE_DIR}/hello.cpp")
file(GLOB_RECURSE TVEDITSRC "${CMAKE_CURRENT_SOURCE_DIR}/examples/tvedit/*.cpp")
file(GLOB_RECURSE TVDEMOSRC "${CMAKE_CURRENT_SOURCE_DIR}/examples/tvdemo/*.cpp")
file(GLOB_RECURSE TVDIRSRC "${CMAKE_CURRENT_SOURCE_DIR}/examples/tvdir/*.cpp")
file(GLOB_RECURSE TVHELPSRC "${CMAKE_CURRENT_SOURCE_DIR}/examples/tvhelp/*.cpp")
file(GLOB_RECURSE MMENUSRC "${CMAKE_CURRENT_SOURCE_DIR}/examples/mmenu/*.cpp")
file(GLOB_RECURSE PALETTESRC "${CMAKE_CURRENT_SOURCE_DIR}/examples/palette/*.cpp")

add_library(tvision STATIC ${TVSOURCE})

# Libraries

set(LIBS tvision)

if (TV_PLATF EQUAL TV_PLATF_UNIX)
    # ncursesw
    find_library(NCURSESW ncursesw)
    if (NOT NCURSESW AND APPLE)
        # macOS has no ncursesw by default
        find_library(NCURSESW ncurses)
    endif()
    list(APPEND LIBS ${NCURSESW})
    target_compile_definitions(tvision PRIVATE HAVE_NCURSES)

    # tinfow (comes with ncurses and is often provided as 'tinfo',
    # but we need to link the 'w' version when both are available)
    find_library(TINFOW tinfow)
    if (TINFOW)
        list(APPEND LIBS ${TINFOW})
        message(STATUS "tinfow library found")
    endif()

    if (TV_BUILD_USING_GPM)
        message(STATUS "gpm library requested")
        # Optional dependencies
        find_library(GPM gpm)
        if (GPM)
            list(APPEND LIBS ${GPM})
            target_compile_definitions(tvision PRIVATE HAVE_GPM)
            message(STATUS "gpm library found")
        else()
            message(WARNING "gpm library requested but not found")
        endif()
    endif()
endif()

# allow CMAKE_INSTALL_PREFIX to decide final install position
#
# static lib
#
install(TARGETS tvision ARCHIVE DESTINATION lib)

# includes
# ./include/tvision and children copied to destination/include/tvision etc...
#
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/tvision" DESTINATION include)

if (TV_BUILD_EXAMPLES)

    add_executable(hello ${HELLOSRC})
    add_executable(tvedit ${TVEDITSRC})
    add_executable(tvdemo ${TVDEMOSRC})
    add_executable(tvdir ${TVDIRSRC})
    add_executable(tvhc ${TVHELPSRC})
    add_executable(mmenu ${MMENUSRC})
    add_executable(palette ${PALETTESRC})

    set(TVAPPS hello tvedit tvdemo tvdir tvhc mmenu palette)

    foreach(t ${TVAPPS})
        target_link_libraries(${t} ${LIBS})
    endforeach(t)

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        target_compile_options(tvdir PRIVATE -Wno-stringop-truncation)
    endif()

    # binaries
    #
    install(DIRECTORY ${EXECUTABLE_OUTPUT_PATH} DESTINATION bin)

endif(TV_BUILD_EXAMPLES)

if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16.0")
    # Enable precompiled headers
    target_precompile_headers(tvision PRIVATE "include/tvision/tv.h")
    if (TV_BUILD_EXAMPLES)
        target_precompile_headers(hello PRIVATE "include/tvision/tv.h")
        foreach(t ${TVAPPS})
            if (NOT (${t} STREQUAL hello))
                target_precompile_headers(${t} REUSE_FROM hello)
            endif()
        endforeach(t)
    endif()
endif()

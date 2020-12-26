# Options summary:
#
# * TV_BUILD_EXAMPLES (default ON) to enable building examples.
# * TV_BUILD_USING_GPM (default ON) (only on linux) to enable linking to libgpm
#   (warning if not found).
# * TV_USE_STATIC_RTL (default ON) to link against the static version of the
#   runtime library (MSVC only).

cmake_minimum_required (VERSION 3.5)
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.13.0")
    cmake_policy(SET CMP0077 NEW) # 'option()' honors normal variables.
endif()
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.15.0")
    cmake_policy(SET CMP0091 NEW) # MSVC runtime library flags are selected by an abstraction.
endif()

set(MASTER_PROJECT FALSE)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MASTER_PROJECT TRUE)
endif()

function(tv_message mode)
    if (MASTER_PROJECT)
        set(msg)
    else()
        set(msg "(${PROJECT_NAME}) ")
    endif()
    foreach(i ${ARGN})
        set(msg "${msg}${i}")
    endforeach()
    message(${mode} ${msg})
endfunction()

function(tv_message_mp)
    if (MASTER_PROJECT)
        tv_message(${ARGN})
    endif()
endfunction()

project(tvision)

# Project options

include(CheckCXXSourceCompiles)

check_cxx_source_compiles("
    #define _doDeclare(a, b) int a ## b;
    #define _expand(a, ...) _doDeclare(a, __VA_ARGS__)
    #define _declare(a) _expand(a, __COUNTER__)

    _declare(a)
    _declare(a)
    _declare(a)

    int main() { return a1; }
"   SUPPORTS_COUNTER_MACRO)

function(tv_enable_unity target)
    if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.16.0" AND SUPPORTS_COUNTER_MACRO)
        set_target_properties(${target} PROPERTIES UNITY_BUILD ON)
    endif()
endfunction()

function(tv_set_output_dir target)
    # Place everything in the build directory, for ease of use.
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endfunction()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    option(TV_BUILD_USING_GPM "Use GPM" ON)
    set(MAY_BUILD_USING_GPM TRUE)
endif()

if (MASTER_PROJECT)
    option(TV_BUILD_EXAMPLES "Build example apps" ON)
endif()

if (MSVC)
    option(TV_USE_STATIC_RTL "Link against the static version of the runtime library (MSVC only)" OFF)
    if (TV_USE_STATIC_RTL)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
    set(MAY_USE_STATIC_RTL TRUE)
endif()

tv_message_mp(STATUS "Install path: ${CMAKE_INSTALL_PREFIX}")
tv_message(STATUS "Build Examples: ${TV_BUILD_EXAMPLES}")
if (MAY_BUILD_USING_GPM)
    tv_message(STATUS "Build w/GPM: ${TV_BUILD_USING_GPM}")
endif()
if (MAY_USE_STATIC_RTL)
    tv_message_mp(STATUS "Link w/static RTL: ${TV_USE_STATIC_RTL}")
endif()

# Libraries
file(GLOB_RECURSE TVSOURCE "${CMAKE_CURRENT_LIST_DIR}/source/*/*.cpp")
list(REMOVE_ITEM TVSOURCE "${CMAKE_CURRENT_LIST_DIR}/source/tvision/geninc.cpp")
add_library(${PROJECT_NAME} STATIC ${TVSOURCE})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include"
)

function(tv_add_private_includes target)
    target_include_directories(${target} PRIVATE
        "${PROJECT_SOURCE_DIR}/include/tvision"
        "${PROJECT_SOURCE_DIR}/include/tvision/compat"
    )
    if (NOT WIN32 AND NOT (CMAKE_SYSTEM_NAME STREQUAL "Linux"))
        target_include_directories(${target} PRIVATE
            "${PROJECT_SOURCE_DIR}/include/tvision/compat/malloc"
        )
    endif()
endfunction()

tv_add_private_includes(${PROJECT_NAME})

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PUBLIC
        /wd4068
        /wd4146
        /wd4166
        /wd4244
        /wd4250
        /wd4267
        /wd4996
        /permissive-
        /Zc:__cplusplus
    )
else()
    target_compile_options(${PROJECT_NAME} PUBLIC
        -Wall
        -Wno-unknown-pragmas
        -Wno-pragmas
        -Wno-reorder
        -Wno-deprecated
    )
endif()

if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        _CRT_SECURE_NO_WARNINGS
    )
endif()

# Dependencies

if (NOT WIN32)
    # ncursesw
    find_library(NCURSESW ncursesw)
    if (NOT NCURSESW AND APPLE)
        # macOS has no ncursesw by default
        find_library(NCURSESW ncurses)
    endif()
    # Some distributions place ncurses.h under a separate directory.
    find_path(NCURSESW_INCLUDE "ncursesw/ncurses.h")
    if (NCURSESW_INCLUDE)
        target_include_directories(${PROJECT_NAME} PRIVATE "${NCURSESW_INCLUDE}/ncursesw")
    endif()

    target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_NCURSES)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${NCURSESW})

    # tinfow (comes with ncurses and is often provided as 'tinfo',
    # but we need to link the 'w' version when both are available)
    find_library(TINFOW tinfow)
    if (TINFOW)
        target_link_libraries(${PROJECT_NAME} PUBLIC ${TINFOW})
    endif()

    # gpm
    if (TV_BUILD_USING_GPM)
        tv_message(STATUS "gpm library requested")
        find_library(GPM gpm)
        if (NOT GPM)
            tv_message(WARNING "gpm library requested but not found")
        else()
            tv_message(STATUS "gpm library found")
            target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_GPM)
            target_link_libraries(${PROJECT_NAME} PUBLIC ${GPM})
        endif()
    endif()
endif()

# allow CMAKE_INSTALL_PREFIX to decide final install position
#
# static lib
#
install(TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION lib)

# includes
# ./include/tvision and children copied to destination/include/tvision etc...
#
install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/include/tvision" DESTINATION include)

# Examples

add_subdirectory(examples)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.16.0")
    # Enable precompiled headers
    target_precompile_headers(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/include/tvision/tv.h")
endif()

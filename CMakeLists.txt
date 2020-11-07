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
        "${CMAKE_CURRENT_LIST_DIR}/include/tvision"
        "${CMAKE_CURRENT_LIST_DIR}/include/tvision/compat"
    )
    if (NOT WIN32 AND NOT (CMAKE_SYSTEM_NAME STREQUAL "Linux"))
        target_include_directories(${target} PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/include/tvision/compat/malloc"
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


if (NOT WIN32)
    # ncursesw
    find_library(NCURSESW ncursesw)
    if (NOT NCURSESW AND APPLE)
        # macOS has no ncursesw by default
        find_library(NCURSESW ncurses)
    endif()

    target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_NCURSES)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${NCURSESW})

    # tinfow (comes with ncurses and is often provided as 'tinfo',
    # but we need to link the 'w' version when both are available)
    find_library(TINFOW tinfow)
    if (TINFOW)
        target_link_libraries(${PROJECT_NAME} PUBLIC ${TINFOW})
    endif()

    if (TV_BUILD_USING_GPM)
        tv_message(STATUS "gpm library requested")
        # Optional dependencies
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

if (TV_BUILD_EXAMPLES)

    list(APPEND TVAPPS hello tvedit tvdemo tvdir tvhelp mmenu palette)

    foreach(app ${TVAPPS})
        if (app STREQUAL "hello")
            list(APPEND app-src "${CMAKE_CURRENT_LIST_DIR}/hello.cpp")
        else()
            file(GLOB_RECURSE app-src "${CMAKE_CURRENT_LIST_DIR}/examples/${app}/*.cpp")
        endif()

        add_executable(${app} ${app-src})
        tv_add_private_includes(${app})
        target_link_libraries(${app} PUBLIC ${PROJECT_NAME})
        # binaries
        install(TARGETS ${app} RUNTIME DESTINATION bin)
    endforeach()

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(tvdir PRIVATE -Wno-stringop-truncation)
    endif()

endif()

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.16.0")
    # Enable precompiled headers
    target_precompile_headers(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/include/tvision/tv.h")
    if (TV_BUILD_EXAMPLES)
        target_precompile_headers(hello PRIVATE "${CMAKE_CURRENT_LIST_DIR}/include/tvision/tv.h")
        foreach(app ${TVAPPS})
            if (NOT(app STREQUAL "hello"))
                target_precompile_headers(${app} REUSE_FROM "hello")
            endif()
        endforeach()
    endif()
endif()

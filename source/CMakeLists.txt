# Turbo Vision sources.

file(GLOB_RECURSE TVSOURCE "${CMAKE_CURRENT_LIST_DIR}/*/*.cpp")
list(REMOVE_ITEM TVSOURCE "${CMAKE_CURRENT_LIST_DIR}/tvision/geninc.cpp")

add_library(${PROJECT_NAME} STATIC ${TVSOURCE})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_14)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_11)

target_include_directories(${PROJECT_NAME} PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
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
        /Zc:externConstexpr
        /Zc:inline
    )
    if (TV_REDUCE_APP_SIZE) # Implies CMake >= 3.13.0
        target_compile_options(${PROJECT_NAME} PRIVATE
            /Gy
        )
        target_link_options(${PROJECT_NAME} PUBLIC
            /OPT:REF
        )
    endif()
else()
    target_compile_options(${PROJECT_NAME} PUBLIC
        -Wall
        -Wno-unknown-pragmas
        -Wno-pragmas
        -Wno-reorder
        -Wno-deprecated
    )
    if (TV_REDUCE_APP_SIZE) # Implies CMake >= 3.13.0
        target_compile_options(${PROJECT_NAME} PRIVATE
            -fdata-sections -ffunction-sections
        )
        target_link_options(${PROJECT_NAME} PUBLIC
            -Wl,--gc-sections
        )
    endif()
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PUBLIC
        -Wno-overloaded-virtual
    )
endif()

if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        _CRT_SECURE_NO_WARNINGS
    )
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
    # Do not include unnecessary STL headers to speed up compilation.
    TVISION_NO_STL
)

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

tv_set_output_dir(${PROJECT_NAME})

# allow CMAKE_INSTALL_PREFIX to decide final install position
#
# static lib
#
install(TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION lib)

# includes
# ./include/tvision and children copied to destination/include/tvision etc...
#
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/tvision" DESTINATION include)

# Build optimization

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.16.0")
    # Enable precompiled headers.
    target_precompile_headers(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/include/pch.h")

    if (TV_LIBRARY_UNITY_BUILD)
        # Enable Unity Build.
        # Files with non-trivial global variables must not be included in Unity Build.
        file(GLOB_RECURSE TVSOURCE_NOUNITY "${CMAKE_CURRENT_LIST_DIR}/tvision/s*.cpp")
        list(APPEND TVSOURCE_NOUNITY "${CMAKE_CURRENT_LIST_DIR}/tvision/new.cpp")
        list(REMOVE_ITEM TVSOURCE_NOUNITY
            "${CMAKE_CURRENT_LIST_DIR}/tvision/stddlg.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/tvision/strmstat.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/tvision/syserr.cpp"
        )
        set_property(SOURCE ${TVSOURCE_NOUNITY} PROPERTY SKIP_UNITY_BUILD_INCLUSION ON)
        set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ON)
    endif()
endif()

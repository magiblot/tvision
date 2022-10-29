if (TV_BUILD_TESTS)
    file(GLOB_RECURSE TEST_SRC "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

    add_executable(${PROJECT_NAME}-test ${TEST_SRC})

    target_compile_features(${PROJECT_NAME}-test PRIVATE cxx_std_14)
    target_include_directories(${PROJECT_NAME}-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}")

    tv_add_private_includes(${PROJECT_NAME}-test)
    tv_set_warnings(${PROJECT_NAME}-test)

    find_library(GTEST gtest REQUIRED)
    tv_message(STATUS "Found 'gtest': ${GTEST}")
    find_library(GTEST_MAIN gtest_main REQUIRED)
    tv_message(STATUS "Found 'gtest_main': ${GTEST_MAIN}")
    target_link_libraries(${PROJECT_NAME}-test PRIVATE
        ${PROJECT_NAME}
        ${GTEST}
        ${GTEST_MAIN}
    )

    add_custom_command(TARGET ${PROJECT_NAME}-test POST_BUILD COMMAND ${PROJECT_NAME}-test WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

endif()

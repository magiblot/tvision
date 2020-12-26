if (TV_BUILD_EXAMPLES)

    function(tv_app_enable_pch app)
        if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.16.0")
            if (app STREQUAL "hello")
                target_precompile_headers(hello PRIVATE "${PROJECT_SOURCE_DIR}/include/tvision/tv.h")
            else()
                target_precompile_headers(${app} REUSE_FROM hello)
            endif()
        endif()
    endfunction()

    function(tv_app_optimize app)
        tv_app_enable_pch(${app})
        tv_enable_unity(${app})
    endfunction()

    function(tv_app_common app)
        tv_set_output_dir(${app})
        tv_add_private_includes(${app})
        tv_app_optimize(${app})
    endfunction()

    function(tv_app_add_generic app)
        file(GLOB_RECURSE app-src "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
        add_executable(${app} ${app-src})
        target_link_libraries(${app} PUBLIC ${PROJECT_NAME})
        tv_app_common(${app})
    endfunction()

    add_executable(hello "${PROJECT_SOURCE_DIR}/hello.cpp")
    target_link_libraries(hello PUBLIC ${PROJECT_NAME})
    tv_app_common(hello)

    list(APPEND TVAPPS tvedit tvdemo tvdir tvforms tvhc mmenu palette)
    list(APPEND TVINSTALLAPPS tvedit tvdemo tvhc)

    foreach(app ${TVAPPS})
        add_subdirectory(${app})
    endforeach()

    install(TARGETS ${TVINSTALLAPPS} RUNTIME DESTINATION bin)

endif()

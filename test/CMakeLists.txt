include(fetch_content_wrapper)
cmake_language(CALL ${PROJECT_NAME}_fetch_content_wrapper
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        origin/main
)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    get_target_property(GMOCK_REAL_TARGET GTest::gmock ALIASED_TARGET)
    if(GMOCK_REAL_TARGET)
        target_compile_options(GMOCK_REAL_TARGET PRIVATE
            /Qspectre
            /wd5045
        )
    endif()
endif()

cmake_language(CALL ${PROJECT_NAME}_fetch_content_wrapper
    mock_wrapper
    GIT_REPOSITORY https://github.com/meni-adin/c-mock-wrapper.git
    GIT_TAG        origin/main
)

enable_testing()

add_subdirectory(logger_test)

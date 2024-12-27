include(fetch_content_wrapper)
fetch_content_wrapper(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        origin/main
)

enable_testing()

add_subdirectory(logger_test)

include(fetch_content_wrapper)
fetch_content_wrapper(
    errors
    GIT_REPOSITORY https://github.com/meni-adin/c-errors.git
    GIT_TAG        origin/main
)

add_subdirectory(logger)

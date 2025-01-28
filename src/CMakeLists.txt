include(fetch_content_wrapper)
cmake_language(CALL ${PROJECT_NAME}_fetch_content_wrapper
    mock_wrapper
    GIT_REPOSITORY https://github.com/meni-adin/c-mock-wrapper.git
    GIT_TAG        origin/main
)

cmake_language(CALL ${PROJECT_NAME}_fetch_content_wrapper
    errors
    GIT_REPOSITORY https://github.com/meni-adin/c-errors.git
    GIT_TAG        origin/main
)

add_subdirectory(logger)

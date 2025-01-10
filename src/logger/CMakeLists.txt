set(TARGET_NAME logger)

set(TARGET_SOURCES
    "logger.c"
)

add_library(${TARGET_NAME} STATIC
    ${TARGET_SOURCES}
)

target_include_directories(${TARGET_NAME}
    PUBLIC "include" ${INCLUDE_DIRECTORIES_TO_PROPAGATE}
)

target_link_libraries(${TARGET_NAME}
    errors
)

set_target_c_compiler_flags(${TARGET_NAME})

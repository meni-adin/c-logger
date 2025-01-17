function(set_compiler_flags)
    if((CMAKE_C_COMPILER_ID STREQUAL "AppleClang") OR (CMAKE_C_COMPILER_ID STREQUAL "GNU"))
        if(CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
        endif()
        if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
        endif()
    elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(
            /wd5072
        )
else()
        message(FATAL_ERROR "Unknown C compiler: ${CMAKE_C_COMPILER_ID}")
    endif()
endfunction()

function(set_target_c_compiler_flags target)
    if((CMAKE_C_COMPILER_ID STREQUAL "AppleClang") OR (CMAKE_C_COMPILER_ID STREQUAL "GNU"))
        target_compile_options(${target} PRIVATE
            -Werror
            -Wall
            -Wextra
            -Wpedantic
            -Wvla
            -Wformat
            # -Wshadow  # turned off as it may be useful in macros
            -Wconversion
            -Wnull-dereference
            -Wdouble-promotion
            -Wimplicit-fallthrough
        )
        if(${PROJECT_NAME_UC}_ENABLE_COVERAGE)
            target_compile_options(${target} PRIVATE
            -fprofile-arcs
            -ftest-coverage
            )
            target_link_options(${target} PRIVATE
            -fprofile-arcs
            -ftest-coverage
            )
        endif()
        if(CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
        endif()
        if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
            target_compile_options(${target} PRIVATE
                # -Wformat-signedness  # currently fails clang-tidy using compile_commands.json on Ubuntu
            )
        endif()
    elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${target} PRIVATE
            /Wall
            /WX
            /wd4710
            /wd4711
            /wd4820
            /wd5045
            /wd5072
        )
else()
        message(FATAL_ERROR "Unknown C compiler: ${CMAKE_C_COMPILER_ID}")
    endif()
endfunction()

function(set_target_cpp_compiler_flags target)
    if((CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
        if(${PROJECT_NAME_UC}_ENABLE_COVERAGE)
            target_compile_options(${target} PRIVATE
                -fprofile-arcs
                -ftest-coverage
            )
            target_link_options(${target} PRIVATE
                -fprofile-arcs
                -ftest-coverage
            )
        endif()
        if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        endif()
        if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        endif()
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${target} PRIVATE
            /Wall
            /WX
            /wd4625
            /wd4626
            /wd4710
            /wd4711
            /wd5026
            /wd5027
            /wd5072
        )
    else()
        message(FATAL_ERROR "Unknown C++ compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endfunction()

function(enable_sanitizers)
    if((CMAKE_C_COMPILER_ID STREQUAL "AppleClang") OR (CMAKE_C_COMPILER_ID STREQUAL "GNU"))
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address)
    elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(/fsanitize=address)
        add_link_options(/fsanitize=address)

        foreach(flag_var
            CMAKE_C_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_RELWITHDEBINFO
            CMAKE_C_FLAGS_MINSIZEREL
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_RELWITHDEBINFO
            CMAKE_CXX_FLAGS_MINSIZEREL)
            if(${flag_var} MATCHES "/MT")
                string(REPLACE "/MT" "/MD" ${flag_var} "${${flag_var}}")
            endif()
            if(${flag_var} MATCHES "/MTd")
                string(REPLACE "/MTd" "/MDd" ${flag_var} "${${flag_var}}")
            endif()
        endforeach()
    endif()
endfunction()

function(remove_cmakefiles_directories)
    message(STATUS "Removing 'CMakeFiles' directories to enable regeneration of coverage related files")
    file(GLOB_RECURSE CMAKEFILES_DIRS LIST_DIRECTORIES YES "${CMAKE_BINARY_DIR}/*")

    set(CMAKEFILES_DIRS_TO_REMOVE "")
    foreach(DIR ${CMAKEFILES_DIRS})
        get_filename_component(DIR_NAME ${DIR} NAME)
        get_filename_component(PARENT_NAME ${DIR} DIRECTORY)
        if(DIR_NAME STREQUAL "CMakeFiles" AND IS_DIRECTORY ${DIR})
            if (PARENT_NAME STREQUAL CMAKE_BINARY_DIR)
                message(STATUS "Skipping top-level 'CMakeFiles' directory")
                continue()
            endif()
            list(APPEND CMAKEFILES_DIRS_TO_REMOVE ${DIR})
        endif()
    endforeach()

    if(NOT CMAKEFILES_DIRS_TO_REMOVE)
        message(STATUS "No 'CMakeFiles' directories to remove")
    endif()

    foreach(DIR ${CMAKEFILES_DIRS_TO_REMOVE})
        message(STATUS "Removing directory: ${DIR}")
        file(REMOVE_RECURSE ${DIR})
    endforeach()

    message(STATUS "Done Removing 'CMakeFiles' directories")
endfunction()

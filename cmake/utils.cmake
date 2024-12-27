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
            /wd5045
            /wd5072
        )
else()
        message(FATAL_ERROR "Unknown C compiler: ${CMAKE_C_COMPILER_ID}")
    endif()
endfunction()

function(set_target_cpp_compiler_flags target)
    if((CMAKE_C_COMPILER_ID STREQUAL "AppleClang") OR (CMAKE_C_COMPILER_ID STREQUAL "GNU"))
        if(CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
        endif()
        if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
        endif()
    elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${target} PRIVATE
            /Wall
            /WX
            /wd4626
            /wd4710
            /wd4711
            /wd5027
            /wd5072
        )
    else()
        message(FATAL_ERROR "Unknown C compiler: ${CMAKE_C_COMPILER_ID}")
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

# add_check_clang_format(name)
#
# Create a target to check for compliance with clang-format output and add it
# as a dependency of the `check_clang_format` target. The script that performs
# the check looks for all files whose filenames end in .h and .cpp, searching
# for them in the directory where the function is called and all its
# subdirectories.
#
# External requirements:
#
#     - clang-format
#
# Example:
#
#     # Add a target to check the current directory and its subdirectories.
#     add_check_clang_format(check_format_src)
#

find_program(CLANG_FORMAT clang-format)

if(CLANG_FORMAT)
    message("** Found clang-format")

    add_custom_target(check_clang_format)

    function(add_check_clang_format name)
        add_custom_target(
            ${name}
            COMMAND find ${CMAKE_CURRENT_SOURCE_DIR} \\\( -name '*.cpp' -or -name '*.h' \\\) -exec ${CMAKE_MODULE_PATH}/clangformatdiff.sh {} \\+
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )

        add_dependencies(check_clang_format ${name})
    endfunction(add_check_clang_format)
else(CLANG_FORMAT)
    message("** clang-format not found")

    function(add_check_clang_format name)
        message("** add_check_clang_format: target ${name} not created")
    endfunction(add_check_clang_format)
endif(CLANG_FORMAT)

cmake_minimum_required(VERSION 3.10)

project(test)

# Usage: cmake -E <command> [arguments...]
# refer: https://cmake.org/cmake/help/latest/manual/cmake.1.html#run-a-command-line-tool
add_custom_command(OUTPUT SOME_RESULT
                  COMMAND ${CMAKE_COMMAND} -E echo "Do something"
                  COMMENT "add_custom_command: get SOME_RESULT"
)

add_custom_target(TaskA ALL
                 DEPENDS SOME_RESULT
)



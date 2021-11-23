cmake_minimum_required(VERSION 3.10)

project(test)

# Note, should be before add_custom_command
add_custom_target(TaskA)

# Usage: cmake -E <command> [arguments...]
# refer: https://cmake.org/cmake/help/latest/manual/cmake.1.html#run-a-command-line-tool
add_custom_command(TARGET TaskA 
                  PRE_BUILD
                  COMMAND ${CMAKE_COMMAND} -E echo "Do something"
)




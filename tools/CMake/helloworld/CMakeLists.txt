
# refer
# https://releases.llvm.org/5.0.0/docs/CMake.html 
# https://llvm.org/docs/CMake.html

cmake_minimum_required(VERSION 3.13.4)

project(hello-world)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "This is BINARY dir " ${PROJECT_BINARY_DIR})
message(STATUS "This is SOURCE dir " ${PROJECT_SOURCE_DIR})
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

#SET(CMAKE_AR /usr/bin/ar)
SET(CMAKE_CXX_ARCHIVE_CREATE "/usr/bin/time -f \"real %e user %U sys %S\" <CMAKE_AR> qc <TARGET> <LINK_FLAGS> <OBJECTS>")
SET(CMAKE_C_ARCHIVE_CREATE "/usr/bin/time -f \"real %e user %U sys %S\" <CMAKE_AR> qc <TARGET> <LINK_FLAGS> <OBJECTS>")


#set(SOURCE_FILES src/main.c)
aux_source_directory(src SOURCE_FILES)

add_subdirectory(src/lib)


# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.

set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Now build our tools
add_executable(demo ${SOURCE_FILES})
target_link_libraries(demo tools)


# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader)

# Link against LLVM libraries
target_link_libraries(demo ${llvm_libs})

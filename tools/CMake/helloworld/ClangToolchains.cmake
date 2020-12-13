cmake_minimum_required( VERSION 2.6.3 )

MESSAGE(STATUS "Building LLVM bitcode files")

set(CMAKE_SYSTEM_NAME Linux)

SET (CMAKE_C_COMPILER             "/root/compile/llvm_install/bin/clang")
SET (CMAKE_C_FLAGS                "-Wall -std=c99")
SET (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
SET (CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
SET (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

SET (CMAKE_CXX_COMPILER             "/root/compile/llvm_install/bin/clang++")
SET (CMAKE_CXX_FLAGS                "-v -Wall -std=c++11")
SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

SET (CMAKE_AR      "/root/compile/llvm_install/bin/llvm-ar")
SET (CMAKE_LINKER  "/root/compile/llvm_install/bin/ld.lld")
SET (CMAKE_NM      "/root/compile/llvm_install/bin/llvm-nm")
SET (CMAKE_OBJDUMP "/root/compile/llvm_install/bin/llvm-objdump")
SET (CMAKE_RANLIB  "/root/compile/llvm_install/bin/llvm-ranlib")

MESSAGE(STATUS "Linker: " ${CMAKE_LINKER})

MESSAGE(STATUS "LLVM C Compiler: " ${CMAKE_C_COMPILER})
MESSAGE(STATUS "LLVM C FLAGS: " ${CMAKE_C_FLAGS})
MESSAGE(STATUS "LLVM CXX Compiler: " ${CMAKE_CXX_COMPILER})
MESSAGE(STATUS "LLVM CXX FLAGS: " ${CMAKE_CXX_FLAGS})

MESSAGE(STATUS "LLVM C flags: " ${LLVM_C_COMPILE_FLAGS})
MESSAGE(STATUS "LLVM CXX flags: " ${LLVM_CXX_COMPILE_FLAGS})

MESSAGE(STATUS "LLVM LD flags: " ${LLVM_LDFLAGS})
MESSAGE(STATUS "LLVM core libs: " ${LLVM_LIBS_CORE})
MESSAGE(STATUS "LLVM JIT libs: " ${LLVM_LIBS_JIT})
MESSAGE(STATUS "LLVM JIT objs: " ${LLVM_LIBS_JIT_OBJECTS})



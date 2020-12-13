# Detect LLVM and set various variable to link against the different component of LLVM
#
# LLVM_BIN_DIR : directory with LLVM binaries
# LLVM_LIB_DIR : directory with LLVM library
# LLVM_INCLUDE_DIR : directory with LLVM include
#
# LLVM_C_COMPILE_FLAGS : compile flags needed to build a C program using LLVM headers
# LLVM_CXX_COMPILE_FLAGS : compile flags needed to build a C++ program using LLVM headers
#
# LLVM_LDFLAGS : ldflags needed to link
# LLVM_LIBS_CORE : ldflags needed to link against a LLVM core library
# LLVM_LIBS_JIT : ldflags needed to link against a LLVM JIT
# LLVM_LIBS_JIT_OBJECTS : objects you need to add to your source when using LLVM JIT
#
# refer: http://git.icir.org/binpacpp.git/blob/HEAD:/cmake/FindLLVM.cmake


if (LLVM_INCLUDE_DIR)
	set(LLVM_FOUND TRUE)

else (LLVM_INCLUDE_DIR)
 
	find_program(LLVM_CLANG_EXECUTABLE
		NAMES clang
		PATHS
		/root/compile/llvm_install/bin
		REQUIRED
		NO_DEFAULT_PATH
	)
		
	if (LLVM_CLANG_EXECUTABLE)
		MESSAGE(STATUS "Found clang at: ${LLVM_CLANG_EXECUTABLE}")
		#set(CMAKE_C_COMPILER "clang")
	else(LLVM_CLANG_EXECUTABLE)
		MESSAGE(FATAL_ERROR "clang is required, but not found!")
	endif(LLVM_CLANG_EXECUTABLE)


endif (LLVM_INCLUDE_DIR)



cmake_minimum_required (VERSION 2.8.5)

include(ExternalProject)

find_program(YACC yacc)
find_program(FLEX flex)

if (${YACC} STREQUAL "YACC-NOTFOUND")
	message(FATAL_ERROR "Could not find YACC executable")
endif()
if (${FLEX} STREQUAL "FLEX-NOTFOUND")
	message(FATAL_ERROR "Could not find flex executable")
endif()

set(LIBJIT_SRC_DIR "${CMAKE_SOURCE_DIR}/lib/libjit")
set(LIBJIT_BIN_DIR "${CMAKE_BINARY_DIR}/lib/libjit")

ExternalProject_Add(
	libjit
	SOURCE_DIR        "${LIBJIT_SRC_DIR}"
	BINARY_DIR        "${LIBJIT_SRC_DIR}" # must be built in source, ugh
	CONFIGURE_COMMAND "${LIBJIT_SRC_DIR}/configure" "--prefix=${LIBJIT_BIN_DIR}"
	BUILD_COMMAND     "make"
)

find_library(
	LIBJIT
	NAMES libjit.so
	PATHS "${LIBJIT_BIN_DIR}/lib/"
	NO_DEFAULT_PATH
)

if (${LIBJIT} STREQUAL "LIBJIT-NOTFOUND")
	message(FATAL_ERROR "Could not find libjit library")
endif()

set(LIBJIT "${LIBJIT}" PARENT_SCOPE)

message("Run, './auto_gen.sh', 'configure' and 'make' by hand, kthxbye")


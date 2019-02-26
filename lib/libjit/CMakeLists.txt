
cmake_minimum_required(VERSION 3.11)

find_program(YACC yacc)
find_program(FLEX flex)

if (${YACC} STREQUAL "YACC-NOTFOUND")
  message(FATAL_ERROR "Could not find YACC executable")
endif()
if (${FLEX} STREQUAL "FLEX-NOTFOUND")
  message(FATAL_ERROR "Could not find flex executable")
endif()

set(LIBJIT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(LIBJIT_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/libjit-src")
set(LIBJIT_LIB     "${LIBJIT_BIN_DIR}/jit/.libs/libjit.a")
set(LIBJIT_INCLUDE "${LIBJIT_BIN_DIR}/include")

set(CONFIGURE_LIBJIT_COMMAND
  COMMAND cp --archive --update "${LIBJIT_SRC_DIR}/*" "${LIBJIT_BIN_DIR}"
  COMMAND "${CMAKE_COMMAND}" -E chdir "${LIBJIT_BIN_DIR}" ./bootstrap
  COMMAND "${CMAKE_COMMAND}" -E chdir "${LIBJIT_BIN_DIR}" ./configure --prefix "${CMAKE_INSTALL_PREFIX}"
)

add_custom_target(
  configure-libjit
  ${CONFIGURE_LIBJIT_COMMAND}
  USES_TERMINAL
)

add_custom_command(
  COMMENT "Configuring libjit"
  OUTPUT  "${LIBJIT_BIN_DIR}/config.log"
  DEPENDS "${LIBJIT_SRC_DIR}/configure.ac"
  ${CONFIGURE_LIBJIT_COMMAND}
)

# add_custom_command(
add_custom_target(
  build-libjit
  COMMENT "Building libjit"
  DEPENDS "${LIBJIT_BIN_DIR}/config.log"
  COMMAND cp --archive --update "${LIBJIT_SRC_DIR}" "${LIBJIT_BIN_DIR}"
  COMMAND "$(MAKE)" -C "${LIBJIT_BIN_DIR}"
)

## make sure paths exist or CMake complains
file(MAKE_DIRECTORY "${LIBJIT_INCLUDE}")

add_library(libjit STATIC IMPORTED GLOBAL)
set_target_properties(libjit PROPERTIES IMPORTED_LOCATION "${LIBJIT_LIB}")
target_include_directories(libjit INTERFACE "${LIBJIT_BIN_DIR}/include")
add_dependencies(libjit build-libjit)

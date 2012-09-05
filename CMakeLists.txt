cmake_minimum_required(VERSION 2.6)
project(libdrizzle C)
set(VERSION_MAJOR 1)
set(VERSION_RELEASE 0)
set(VERSION_MINOR 0)
set(LIBDRIZZLE_VERSION ${VERSION_MAJOR}.${VERSION_RELEASE}.${VERSION_MINOR})
set(ARCHIVE_NAME "${CMAKE_PROJECT_NAME}-${VERSION}")

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

find_package(OpenSSL REQUIRED)

option(DEBUG_BUILD "Build in debug mode" OFF)

set(CMAKE_C_FLAGS "-Wall -Wunused -Wwrite-strings -Wno-strict-aliasing -Wextra -Wshadow -g")

if (NOT DEBUG_BUILD)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
endif ()

if (NOT WIN32)
  include(CheckCCompilerFlag)
  check_c_compiler_flag(-fvisibility=hidden HAVE_VISIBILITY)
  set(HAVE_VISIBILITY ${HAVE_VISIBILITY} CACHE BOOL "GCC support for hidden visibility")
  if (HAVE_VISIBILITY)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
  endif (HAVE_VISIBILITY)
endif()

if (NOT CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local" CACHE STRING "Install path" FORCE)
endif (NOT CMAKE_INSTALL_PREFIX)
MARK_AS_ADVANCED(CMAKE)

option(ERROR_ON_WARN "Error on warnings" OFF)
if (ERROR_ON_WARN)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
endif (ERROR_ON_WARN)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${drizzle_SOURCE_DIR} ${OPENSSL_INCLUDE_DIRECTORIES})

add_definitions(-DBUILDING_LIBDRIZZLE)

SET(SOURCE_FILES
libdrizzle/column.c
libdrizzle/command.c
libdrizzle/conn.c
libdrizzle/conn_uds.c
libdrizzle/drizzle.c
libdrizzle/error.c
libdrizzle/field.c
libdrizzle/handshake.c
libdrizzle/pack.c
libdrizzle/query.c
libdrizzle/result.c
libdrizzle/row.c
libdrizzle/sha1.c
libdrizzle/ssl.c
libdrizzle/state.c
)

add_library(drizzle ${SOURCE_FILES})
target_link_libraries(drizzle ${OPENSSL_LIBRARIES})
set_target_properties(drizzle PROPERTIES DEFINE_SYMBOL DRIZZLECLIENT_DLL)
set_target_properties(drizzle PROPERTIES SOVERSION 1)
set_target_properties(drizzle PROPERTIES VERSION ${LIBDRIZZLE_VERSION})

INSTALL(TARGETS drizzle ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)

INSTALL(DIRECTORY libdrizzle DESTINATION include FILES_MATCHING PATTERN "*.h")

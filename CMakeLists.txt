cmake_minimum_required(VERSION 2.6)
project(libdrizzle C)
set(VERSION_MAJOR 5)
set(VERSION_RELEASE 0)
set(VERSION_TAG "alpha1")
set(LIBDRIZZLE_VERSION ${VERSION_MAJOR}.${VERSION_RELEASE}-${VERSION_TAG})
set(ARCHIVE_NAME "${CMAKE_PROJECT_NAME}-${LIBDRIZZLE_VERSION}")

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

if(NOT DEFINED BUILD_SHARED_LIBS)
    option(BUILD_SHARED_LIBS "Build a shared library instead of static" ON)
endif()


find_package(OpenSSL REQUIRED)

option(DEBUG_BUILD "Build in debug mode" OFF)

set(CMAKE_C_FLAGS "-Wall -Wunused -Wwrite-strings -Wno-strict-aliasing -Wextra -Wshadow -std=gnu99 -pedantic -g")

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
libdrizzle/binlog.c
)

add_library(drizzle-redux ${SOURCE_FILES})
target_link_libraries(drizzle-redux ${OPENSSL_LIBRARIES})
set_target_properties(drizzle-redux PROPERTIES DEFINE_SYMBOL DRIZZLECLIENT_DLL)
set_target_properties(drizzle-redux PROPERTIES SOVERSION ${VERSION_MAJOR})
set_target_properties(drizzle-redux PROPERTIES VERSION ${LIBDRIZZLE_VERSION})

INSTALL(TARGETS drizzle-redux ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)

INSTALL(DIRECTORY libdrizzle DESTINATION include FILES_MATCHING PATTERN "*.h")

option(CI_TESTS "Run CI tests on the code" OFF)

if (CI_TESTS)
  include(CTest)
  find_package(cppcheck REQUIRED)
  include(CppcheckTargets)
  add_cppcheck(drizzle-redux)
  add_subdirectory(test/unit)
endif (CI_TESTS)

option(BUILD_HTML_DOCS "Build the HTML documentation" OFF)
option(BUILD_PDF_DOCS "Build the PDF documentation (requires latex and BUILD_HTML_DOCS)" OFF)

if (BUILD_HTML_DOCS)
  add_subdirectory(docs)
  if (BUILD_PDF_DOCS)
    find_package(LATEX REQUIRED)
    if (NOT PDFLATEX_COMPILER)
      message(FATAL_ERROR "pdflatex required, please install texlive-latex and texlive-scheme-all")
    endif()
  endif()
endif (BUILD_HTML_DOCS)

if (EXISTS "${CMAKE_SOURCE_DIR}/.bzr/")
    set(USING_BZR TRUE)
else()
    set(USING_BZR FALSE)
endif()

if (USING_BZR)
  add_custom_target(dist
    COMMAND bzr export --root=${ARCHIVE_NAME}
      ${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar.gz
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VERSION "${LIBDRIZZLE_VERSION}")
include(CPack)

MESSAGE(STATUS "-----------------------------------------------")
MESSAGE(STATUS "libdrizzle-redux ${LIBDRIZZLE_VERSION}")
if (USING_BZR)
  MESSAGE(STATUS "from BZR tree")
endif()
MESSAGE(STATUS "-----------------------------------------------")
MESSAGE(STATUS "BUILD_SHARED_LIBS = ${BUILD_SHARED_LIBS}")
MESSAGE(STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
MESSAGE(STATUS "DEBUG_BUILD = ${DEBUG_BUILD}")
MESSAGE(STATUS "CI_TESTS = ${CI_TESTS}")
MESSAGE(STATUS "BUILD_HTML_DOCS = ${BUILD_HTML_DOCS}")
MESSAGE(STATUS "BUILD_PDF_DOCS = ${BUILD_PDF_DOCS}")
MESSAGE(STATUS "================================================")
MESSAGE(STATUS "Change a values with: cmake -D<Variable>=<Value>")
MESSAGE(STATUS "------------------------------------------------")
MESSAGE(STATUS)


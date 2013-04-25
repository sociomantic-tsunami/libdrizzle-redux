#!/usr/bin/env bash
# libdrizzle
# Copyright (C) 2012 Drizzle Development Team, http://drizzle.org/
# All rights reserved.
#
# Use and distribution licensed under the BSD license.  See
# the COPYING file in this directory for full text.

AC_INIT([libdrizzle],[5.1.4],[http://bugs.launchpad.net/drizzle],[libdrizzle],[http://drizzle.org/])

AC_CONFIG_AUX_DIR([build-aux])

AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([1.11 color-tests -Wno-portability subdir-objects foreign tar-ustar])
AC_PREREQ([2.68])

AC_CANONICAL_HOST
AC_CANONICAL_BUILD
AC_ARG_PROGRAM
AC_USE_SYSTEM_EXTENSIONS

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_HEADERS([config.h:config.in])dnl Keep filename to 8.3 for MS-DOS.

AC_CONFIG_SRCDIR([libdrizzle/drizzle.cc])

#shared library versioning
LIBDRIZZLE_LIBRARY_VERSION=9:0:0
#                         | | |
#                  +------+ | +---+
#                  |        |     |
#                 current:revision:age
#                  |        |     |
#                  |        |     +- increment if interfaces have been added
#                  |        |        set to zero if interfaces have been removed or changed
#                  |        +- increment if source code has changed
#                  |           set to zero if current is incremented
#                  +- increment if interfaces have been added, removed or changed
AC_SUBST([LIBDRIZZLE_LIBRARY_VERSION])

LT_PREREQ([2.2])
LT_INIT
LT_LANG([C++])

AX_PLATFORM
AX_ASSERT

# Specialty checks
AX_CXX_CINTTYPES
CONFIG_EXTRA
SOCKET_SEND_FLAGS

# Checks for programs.
AX_PROG_SPHINX_BUILD

# Checks for libraries.
AX_CXX_GCC_ABI_DEMANGLE

AS_IF([test "x${TARGET_WINDOWS}" != "xtrue"],[AX_CHECK_OPENSSL])

AC_PATH_ZLIB

# Check for -lm
LT_LIB_M

# Checks for header files.
AC_DEFUN([CHECK_FOR_CXXABI],
         [AC_LANG_PUSH([C++])
         AC_CHECK_HEADERS([cxxabi.h])
         AC_LANG_POP])

CHECK_FOR_CXXABI
AC_CHECK_HEADERS([argp.h])
AC_CHECK_HEADERS([errno.h])
AC_CHECK_HEADERS([fcntl.h])
AC_CHECK_HEADERS([io.h])
AC_CHECK_HEADERS([openssl/ssl.h])
AC_CHECK_HEADERS([poll.h])
AC_CHECK_HEADERS([pwd.h])
AC_CHECK_HEADERS([sys/socket.h])
AC_CHECK_HEADERS([windows.h])
AC_CHECK_HEADERS([winsock2.h])
AC_CHECK_HEADERS([ws2tcpip.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_TYPES([in_port_t])
AC_TYPE_SSIZE_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([fcntl])
AC_CHECK_FUNCS([on_exit])
AC_CHECK_FUNCS([poll])
AC_CHECK_FUNCS([ppoll])

AX_PTHREAD(, [AC_MSG_ERROR(could not find libpthread)])
AX_ENDIAN
AX_HEX_VERSION([LIBDRIZZLE],[$VERSION])
AX_HARDEN_COMPILER_FLAGS

AX_CREATE_GENERIC_CONFIG
AX_AM_JOBSERVER([yes])

AC_CONFIG_FILES([Makefile
                 docs/conf.py
                 rpm/spec
                 libdrizzle-5.1/version.h
                 ])

AC_OUTPUT

echo "---"
echo "Configuration summary for $PACKAGE_NAME version $VERSION"
echo ""
echo "   * Installation prefix:       $prefix"
echo "   * System type:               $host_vendor-$host_os"
echo "   * Host CPU:                  $host_cpu"
echo "   * Compiler Vendor:           $ax_cv_c_compiler_vendor"
echo "   * C Compiler:                $CC"
echo "   * C Compiler Version:        $ax_c_compiler_version"
echo "   * C Flags:                   $CFLAGS"
echo "   * C++ Compiler:              $CXX"
echo "   * C++ Version:               $ax_cxx_compiler_version"
echo "   * C++ Flags:                 $CXXFLAGS"
echo "   * CPP Flags:                 $CPPFLAGS"
echo "   * LIB Flags:                 $LIB"
echo "   * Assertions enabled:        $ax_enable_assert"
echo "   * Debug enabled:             $ax_enable_debug"
echo "   * Warnings as failure:       $ac_cv_warnings_as_errors"
echo "   * make -j:                   $enable_jobserver"
echo "   * VCS checkout:              $ac_cv_vcs_system"
echo ""
echo "---"

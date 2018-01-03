#!/usr/bin/env bash
# libdrizzle
# Copyright (C) 2012 Drizzle Development Team, http://drizzle.org/
# All rights reserved.
#
# Use and distribution licensed under the BSD license.  See
# the COPYING file in this directory for full text.

AC_INIT([libdrizzle-redux],[5.7.1],[https://github.com/sociomantic-tsunami/libdrizzle-redux/issues],[libdrizzle-redux],[https://github.com/sociomantic-tsunami/libdrizzle-redux])

AC_CONFIG_AUX_DIR([build-aux])

AC_CANONICAL_TARGET

# Automake version before 1.13 (when the serial-tests option was dnl still the default) still defined the badly obsolete macro.  'AM_PROG_INSTALL'.
AM_INIT_AUTOMAKE(1.11 no-define color-tests -Wno-portability subdir-objects foreign tar-ustar m4_ifndef([AM_WITH_REGEX], [serial-tests]))
AC_PREREQ([2.68])

AC_CANONICAL_HOST
AC_CANONICAL_BUILD
AC_ARG_PROGRAM
AC_USE_SYSTEM_EXTENSIONS

AX_ENABLE_SSL

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_HEADERS([config.h:config.in])dnl Keep filename to 8.3 for MS-DOS.

AC_CONFIG_SRCDIR([libdrizzle/drizzle.cc])

# Shared library versioning
#
# The following explanation considers library changes and ABI compatibility from
# the users' perspective and how they relate to the shared library version.(§)
#
# Programs using the previous version may use the new version as drop-in
# replacement, and programs using the new version can also work with the
# previous one. In other words, no recompiling nor relinking is needed.
# In this case:
#     bump revision only, don’t touch current nor age.
#
# Programs using the previous version may use the new version as drop-in
# replacement, but programs using the new version may use APIs not present in
# the previous one. In other words, a program linking against the new version
# may fail with “unresolved symbols” if linking against the old version at
# runtime. In this case:
#     set revision to 0, bump current and age.
#
# Programs may need to be changed, recompiled, relinked in order to use the
# new version:
#     bump current, set revision and age to 0.
#
# (§) The explanation is taken from this blogpost:
# http://blog.asleson.org/index.php/2014/07/08/libtool-library-versioning-version-info-currentrevisionage/
#
LIBDRIZZLE_LIBRARY_VERSION=12:8:2
#                         | | |
#                  +------+ | +---+
#                  |        |     |
#                 current:revision:age
#                  |        |     |
#                  |        |     +- increment if interfaces have been added
#                  |        |        set to zero if interfaces have been removed
#                                    or changed
#                  |        +- increment if source code has changed
#                  |           set to zero if current is incremented
#                  +- increment if interfaces have been added, removed or changed
AC_SUBST([LIBDRIZZLE_LIBRARY_VERSION])

LT_PREREQ([2.2])
LT_INIT
LT_LANG([C++])

AC_PROG_CXX
AC_PROG_CC
AC_PROG_CC_C99
AS_IF([test "x${ac_cv_prog_cc_c99}" == "xno"],[AC_MSG_ERROR([No c99 compatible compiler found])])

AX_PLATFORM
AX_ASSERT

# Checks c++ 11 support
AX_CXX_COMPILE_STDCXX([11],[],[optional])

# Specialty checks
AX_CXX_CINTTYPES
CONFIG_EXTRA
SOCKET_SEND_FLAGS

# Checks for latex
ACLTX_PROG_LATEX
ACLTX_PROG_PDFLATEX
ACLTX_PROG_KPSEWHICH

# Checks for programs.
AX_PROG_SPHINX_BUILD

# Checks for libraries.
AX_CXX_GCC_ABI_DEMANGLE

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

# Check for setsockopt support
AX_CHECK_SETSOCKOPT

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
                 pkg/rpm/spec
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

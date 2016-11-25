# SYNOPSIS
#
#   AX_ENABLE_SSL()
#
# DESCRIPTION
#
#   Add a feature named ssl, which is enabled by default on all platforms but Windows.
#   If enabled, uses `AX_CHECK_OPENSSL` to set the `LDFLAGS`, `LIBS` and `CPPFLAGS`,
#   as well as defining `USE_OPENSSL` inside `config.h`
#
# LICENSE
#
#   Copyright (c) 2016 Sociomantic Labs, All rights reserved
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

AC_DEFUN([AX_ENABLE_SSL], [
    # SSL support, for encrypted connections
    # Enabled by default on non-Windows platform
    # Note: We use `--enable-ssl` for the *feature*,
    # but AX_CHECK_OPENSSL uses `--with-openssl=dir` for the root directory
    AC_ARG_ENABLE([ssl],
                  AS_HELP_STRING([--disable-ssl], [Disable support for encrypted connection, using OpenSSL]))

    AS_IF([test "x${enable_ssl}" != "xno"], [
            AS_IF([test "x${TARGET_WINDOWS}" != "xtrue"],
                  [AX_CHECK_OPENSSL
                  AC_DEFINE([USE_OPENSSL], [], [Support for encrypted connection is enabled])
                  LDFLAGS="$LDFLAGS $OPENSSL_LDFLAGS"
                  LIBS="$OPENSSL_LIBS $LIBS"
                  CPPFLAGS="$OPENSSL_INCLUDES $CPPFLAGS"],
                  # On Windows we disable it by default
                  [AS_IF([test "x${enable_ssl}" = "xyes"],
                         [AX_CHECK_OPENSSL
                         AC_DEFINE([USE_OPENSSL], [], [Support for encrypted connection is enabled])
                         LDFLAGS="$LDFLAGS $OPENSSL_LDFLAGS"
                         LIBS="$OPENSSL_LIBS $LIBS"
                         CPPFLAGS="$OPENSSL_INCLUDES $CPPFLAGS"])]
                 )
    ])
])

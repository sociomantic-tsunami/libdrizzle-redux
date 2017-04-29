dnl @synopsis Tests for socket options capability.
dnl
dnl Defines support for the following options:
dnl     SO_KEEPALIVE
dnl     TCP_KEEPALIVE
dnl     TCP_KEEPCNT
dnl     TCP_KEEPIDLE
dnl     TCP_KEEPINTVL
dnl
dnl The macro is a modified version of the acinclude.m4 from the ZeroMQ library
dnl http://github.com/zeromq/libzmq
dnl
dnl @license Mozilla Public License v2 (MPLv2)


dnl ############################################################################
dnl # CHECK_SO_KEEPALIVE([action-if-found], [action-if-not-found])             #
dnl # Check if SO_KEEPALIVE is supported                                       #
dnl ############################################################################
AC_DEFUN([CHECK_SO_KEEPALIVE], [{
    AC_MSG_CHECKING(whether SO_KEEPALIVE is supported)
    AC_RUN_IFELSE([/* SO_KEEPALIVE test */
        AC_LANG_PROGRAM([
#if defined(HAVE_WINSOCK2_H) && HAVE_WINSOCK2_H
# include <winsock2.h>
# include <windows.h>
# else
# include <sys/types.h>
# include <sys/socket.h>
#endif
],[
int main (int argc, char *argv [])
{
    int s, rc, opt = 1;
    return (
        ((s = socket (PF_INET, SOCK_STREAM, 0)) == -1) ||
        ((rc = setsockopt (s, SOL_SOCKET, SO_KEEPALIVE, (char*) &opt, sizeof (int))) == -1) ||
        ((rc = setsockopt (s, SOL_SOCKET, SO_KEEPALIVE, (int*) &opt, sizeof (int))) == -1)
    );
}
    ])],
    [AC_MSG_RESULT(yes) ; cv_so_keepalive="yes" ; $1],
    [AC_MSG_RESULT(no)  ; cv_so_keepalive="no"  ; $2],
    [AC_MSG_RESULT(not during cross-compile) ; cv_so_keepalive="no"]
    )
}])

dnl ############################################################################
dnl # CHECK_TCP_KEEPCNT([action-if-found], [action-if-not-found])              #
dnl # Check if TCP_KEEPCNT is supported                                        #
dnl ############################################################################
AC_DEFUN([CHECK_TCP_KEEPCNT], [{
    AC_MSG_CHECKING(whether TCP_KEEPCNT is supported)
    AC_RUN_IFELSE([/* TCP_KEEPCNT test */
        AC_LANG_PROGRAM([
#if defined(HAVE_WINSOCK2_H) && HAVE_WINSOCK2_H
# include <winsock2.h>
# include <windows.h>
#else
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/tcp.h>
#endif
],[
int main (int argc, char *argv [])
{
    int s, rc, opt = 1;
    return (
        ((s = socket (PF_INET, SOCK_STREAM, 0)) == -1) ||
        ((rc = setsockopt (s, IPPROTO_TCP, TCP_KEEPCNT, (char*) &opt, sizeof (int))) == -1) ||
        ((rc = setsockopt (s, IPPROTO_TCP, TCP_KEEPCNT, (int*) &opt, sizeof (int))) == -1)
    );
}
    ])],
    [AC_MSG_RESULT(yes) ; cv_tcp_keepcnt="yes" ; $1],
    [AC_MSG_RESULT(no)  ; cv_tcp_keepcnt="no"  ; $2],
    [AC_MSG_RESULT(not during cross-compile) ; cv_tcp_keepcnt="no"]
    )
}])

dnl ############################################################################
dnl # CHECK_TCP_KEEPIDLE([action-if-found], [action-if-not-found])             #
dnl # Check if TCP_KEEPIDLE is supported                                       #
dnl ############################################################################
AC_DEFUN([CHECK_TCP_KEEPIDLE], [{
    AC_MSG_CHECKING(whether TCP_KEEPIDLE is supported)
    AC_RUN_IFELSE([/* TCP_KEEPIDLE test */
        AC_LANG_PROGRAM([
#if defined(HAVE_WINSOCK2_H) && HAVE_WINSOCK2_H
# include <winsock2.h>
# include <windows.h>
#else
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/tcp.h>
#endif
],[
int main (int argc, char *argv [])
{
    int s, rc, opt = 1;
    return (
        ((s = socket (PF_INET, SOCK_STREAM, 0)) == -1) ||
        ((rc = setsockopt (s, IPPROTO_TCP, TCP_KEEPIDLE, (char*) &opt, sizeof (int))) == -1) ||
        ((rc = setsockopt (s, IPPROTO_TCP, TCP_KEEPIDLE, (int*) &opt, sizeof (int))) == -1)
    );
}
    ])],
    [AC_MSG_RESULT(yes) ; cv_tcp_keepidle="yes" ; $1],
    [AC_MSG_RESULT(no)  ; cv_tcp_keepidle="no"  ; $2],
    [AC_MSG_RESULT(not during cross-compile) ; cv_tcp_keepidle="no"]
    )
}])

dnl ############################################################################
dnl # CHECK_TCP_KEEPINTVL([action-if-found], [action-if-not-found])            #
dnl # Check if TCP_KEEPINTVL is supported                                      #
dnl ############################################################################
AC_DEFUN([CHECK_TCP_KEEPINTVL], [{
    AC_MSG_CHECKING(whether TCP_KEEPINTVL is supported)
    AC_RUN_IFELSE([/* TCP_KEEPINTVL test */
        AC_LANG_PROGRAM([
#if defined(HAVE_WINSOCK2_H) && HAVE_WINSOCK2_H
# include <winsock2.h>
# include <windows.h>
#else
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/tcp.h>
#endif
],[
int main (int argc, char *argv [])
{
    int s, rc, opt = 1;
    return (
        ((s = socket (PF_INET, SOCK_STREAM, 0)) == -1) ||
        ((rc = setsockopt (s, IPPROTO_TCP, TCP_KEEPINTVL, (const char*) &opt, sizeof (int))) == -1) ||
        ((rc = setsockopt (s, IPPROTO_TCP, TCP_KEEPINTVL, (int*) &opt, sizeof (int))) == -1)
    );
}
    ])],
    [AC_MSG_RESULT(yes) ; cv_tcp_keepintvl="yes" ; $1],
    [AC_MSG_RESULT(no)  ; cv_tcp_keepintvl="no"  ; $2],
    [AC_MSG_RESULT(not during cross-compile) ; cv_tcp_keepintvl="no"]
    )
}])

dnl ############################################################################
dnl # CHECK_TCP_KEEPALIVE([action-if-found], [action-if-not-found])            #
dnl # Check if TCP_KEEPALIVE is supported                                      #
dnl ############################################################################
AC_DEFUN([CHECK_TCP_KEEPALIVE], [{
    AC_MSG_CHECKING(whether TCP_KEEPALIVE is supported)
    AC_RUN_IFELSE([ /* TCP_KEEPALIVE test */
        AC_LANG_PROGRAM([
#if defined(HAVE_WINSOCK2_H) && HAVE_WINSOCK2_H
# include <winsock2.h>
# include <windows.h>
#else
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/tcp.h>
#endif
],
[
int main (int argc, char *argv [])
{
    int s, rc, opt = 1;
    return (
        ((s = socket (PF_INET, SOCK_STREAM, 0)) == -1) ||
        ((rc = setsockopt (s, IPPROTO_TCP, TCP_KEEPALIVE, (const char*) &opt, sizeof (int))) == -1) ||
        ((rc = setsockopt (s, IPPROTO_TCP, TCP_KEEPALIVE, (int*) &opt, sizeof (int))) == -1)
    );
}
    ])],
    [AC_MSG_RESULT(yes) ; cv_tcp_keepalive="yes" ; $1],
    [AC_MSG_RESULT(no)  ; cv_tcp_keepalive="no"  ; $2],
    [AC_MSG_RESULT(not during cross-compile) ; cv_tcp_keepalive="no"]
    )
}])

AC_DEFUN([AX_CHECK_SETSOCKOPT],
  [AC_MSG_CHECKING(which socket options are supported)
    # TCP Socket option checks.
    CHECK_SO_KEEPALIVE([
        AC_DEFINE([HAVE_SO_KEEPALIVE],
            [1],
            [Whether SO_KEEPALIVE is supported.])
        ])

    CHECK_TCP_KEEPCNT([
        AC_DEFINE([HAVE_TCP_KEEPCNT],
            [1],
            [Whether TCP_KEEPCNT is supported.])
        ])

    CHECK_TCP_KEEPIDLE([
        AC_DEFINE([HAVE_TCP_KEEPIDLE],
            [1],
            [Whether TCP_KEEPIDLE is supported.])
        ])

    CHECK_TCP_KEEPINTVL([
        AC_DEFINE([HAVE_TCP_KEEPINTVL],
            [1],
            [Whether TCP_KEEPINTVL is supported.])
        ])

    CHECK_TCP_KEEPALIVE([
        AC_DEFINE([HAVE_TCP_KEEPALIVE],
            [1],
            [Whether TCP_KEEPALIVE is supported.])
        ])
  ])

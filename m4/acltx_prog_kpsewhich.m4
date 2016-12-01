dnl @synopsis ACLTX_PROG_KPSEWHICH([ACTION-IF-NOT-FOUND])
dnl
dnl Tests if the application kpsewhich exists. If so, the variable $kpsewhich is
dnl assigned the name of the application, or 'no' if kpsewhich was not found.
dnl In the latter case, configure will emit a warning, if ACTION-IF-NOT-FOUND is
dnl not specified
dnl
dnl @category LaTeX
dnl @author Boretti Mathieu <boretti@eig.unige.ch>
dnl @version 2006-07-16
dnl @license LGPL

AC_DEFUN([ACLTX_PROG_KPSEWHICH],[
AC_CHECK_PROGS(kpsewhich,[kpsewhich],no)
if test $kpsewhich = "no" ;
then
	ifelse($#,0,[AC_MSG_WARN([Unable to find the kpsewhich application])],
        $1)
fi
])

dnl @synopsis ACLTX_PROG_LATEX([ACTION-IF-NOT-FOUND])
dnl
dnl Tests if the application latex exists. If so, the variable $latex is
dnl set to the name of the application, or 'no' if latex was not found.
dnl In the latter case, configure will emit a warning, if ACTION-IF-NOT-FOUND is
dnl not specified
dnl
dnl It is possible to set manually the program to use using latex=...
dnl
dnl @category LaTeX
dnl @author Boretti Mathieu <boretti@eig.unige.ch>
dnl @version 2006-07-16
dnl @license LGPL

AC_DEFUN([ACLTX_PROG_LATEX],[
AC_ARG_VAR(latex,[specify default latex application])
if test "$ac_cv_env_latex_set" = "set" ; then
    AC_MSG_CHECKING([Checking for latex])
    latex="$ac_cv_env_latex_value";
    AC_MSG_RESULT([$latex (from parameter)])
else
    AC_CHECK_PROGS(latex,[latex elatex lambda],no)
fi
if test $latex = "no" ;
then
    ifelse($#,0,[AC_MSG_WARN([Unable to find the latex application])],
        $1)
fi])

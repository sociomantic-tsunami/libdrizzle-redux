dnl @synopsis ACLTX_PROG_PDFLATEX([ACTION-IF-NOT-FOUND])
dnl
dnl Tests if the pdflatex application exists and sets the variable $pdflatex to
dnl the name of the application or to 'no', if it was not found.
dnl In the latter case, configure will emit a warning, if ACTION-IF-NOT-FOUND is
dnl not specified
dnl
dnl @category LaTeX
dnl @author Boretti Mathieu <boretti@eig.unige.ch>
dnl @version 2006-07-16
dnl @license LGPL

AC_DEFUN([ACLTX_PROG_PDFLATEX],[
AC_CHECK_PROGS(pdflatex,[pdflatex],no)
if test $pdflatex = "no" ;
then
       ifelse($#,0,[AC_MSG_WARN([Unable to find the pdflatex application])],
        $1)
fi
])

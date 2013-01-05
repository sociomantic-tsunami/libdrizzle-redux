AC_DEFUN([CONFIG_EXTRA], [

AH_TOP([
#pragma once

/* _SYS_FEATURE_TESTS_H is Solaris, _FEATURES_H is GCC */
#if defined( _SYS_FEATURE_TESTS_H) || defined(_FEATURES_H)
# error "You should include config.h as your first include file"
#endif

])

AH_BOTTOM([

/* This seems to be required for older compilers @note http://stackoverflow.com/questions/8132399/how-to-printf-uint64-t  */
#ifndef __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS
#endif
 
#if defined(__cplusplus)
#  include CINTTYPES_H
#else
#  include <inttypes.h>
#endif

#if !defined(HAVE_ULONG) && !defined(__USE_MISC)
# define HAVE_ULONG 1
typedef unsigned long int ulong;
#endif

#ifndef HAVE_MSG_DONTWAIT
# define MSG_DONTWAIT 0
#endif // HAVE_MSG_DONTWAIT

])

])dnl CONFIG_EXTRA

dnl Checks for the correct version of the courier-unicode library.

AC_DEFUN([AX_COURIER_UNICODE_VERSION],[

AC_MSG_CHECKING(courier-unicode library and version)

vers="$1"

if test "$vers" = ""
then
	vers=2.2.3
fi

set -- `echo "$vers" | tr '.' ' '`

v=$[]1
r=`echo "00"$[]2 | sed 's/.*(...)$/$[]1/'`

p=$[]3

if test "$p" = ""
   then p="0"
fi

p=`echo "00"$p | sed 's/.*(...)$/$[]1/'`

if test "$v$r$p" -lt 2002002
then
   r=$[]2
   p=$[]3
   if test "$p" = ""
       then p="0"
   fi
fi

AC_TRY_COMPILE([
#include <courier-unicode.h>
#ifndef COURIER_UNICODE_VERSION
#define COURIER_UNICODE_VERSION 0
#endif

#if COURIER_UNICODE_VERSION < ]$v$r$p[
#error "courier-unicode ]$vers[ library is required"
#endif

],[],[],
AC_MSG_ERROR([
ERROR: The Courier Unicode Library ]$vers[ header files appear not to be installed.
You may need to upgrade the library or install a separate development
subpackage in addition to the main package.])
)

AC_MSG_RESULT([ok])
])

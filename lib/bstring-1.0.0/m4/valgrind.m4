# valgrind.m4 - Check for Valgrind.
#
# Copyright 2012, 2013 Michael Steinert
#
# This file is free software; the copyright holder gives unlimited
# permission to copy and/or distribute it, with or without modifications,
# as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.

#serial 1

# AX_PROG_VALGRIND
# ----------------
# Setup Valgrind and the "memcheck" target(s).
AC_DEFUN([AX_PROG_VALGRIND],
[dnl
	AC_ARG_VAR([VALGRIND_PATH], [Path to the Valgrind executable.])
	AC_ARG_VAR([VALGRIND_FLAGS], [Flags for Valgrind.])
	AC_ARG_VAR([VALGRIND_ENVIRONMENT],
		   [Environment variables for Valgrind.])
	AC_CHECK_PROGS([VALGRIND_PATH], [valgrind], [:])
	AM_CONDITIONAL([HAVE_VALGRIND], [test "x$VALGRIND_PATH" != "x:"])
])dnl

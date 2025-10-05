AC_PREREQ([2.71])
AC_INIT([wordle], [0.1], [celdaemon@voidgroup.net], [wordle], [https://github.com/CelDaemon/wordle])
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AM_CONDITIONAL([BUILD_STATIC], [AS_CASE(["$target"], [*mingw32], [true], [false])])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_SRCDIR([wordle.c])
AC_CHECK_LIB([z], [gzopen])
AC_OUTPUT

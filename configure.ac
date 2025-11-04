AC_PREREQ([2.71])
AC_INIT([wordle], [1.0.0], [celdaemon@voidgroup.net], [wordle], [https://github.com/CelDaemon/wordle])
AM_MAINTAINER_MODE
AC_CANONICAL_TARGET
PKG_CHECK_MODULES([Z], [zlib])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AM_CONDITIONAL([BUILD_STATIC], [AS_CASE(["$target"], [*mingw32], [true], [false])])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_SRCDIR([wordle.c])
AC_CHECK_LIB([z], [gzopen], [], [AC_MSG_ERROR([libz was not found])])
AC_CHECK_FUNC([getopt_long], [], [AC_MSG_ERROR([getopt_long was not found])])
AC_OUTPUT

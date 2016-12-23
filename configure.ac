AC_INIT([libopentracing], [1.0.0])
#AC_CONFIG_SRCDIR(libopentracing.cc)
#AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AM_INIT_AUTOMAKE([subdir-objects])

AM_CONDITIONAL(COMPILER_IS_GCC, test "x$GCC" = "xyes")

AC_CONFIG_MACRO_DIR([m4])

#AC_CONFIG_HEADERS([config.h])

AM_PROG_LIBTOOL

AC_PROG_INSTALL

AC_LANG_C
AC_PROG_CC
AC_PROG_CXX
AC_PROG_MAKE_SET

AC_HEADER_STDC

#AC_CHECK_HEADERS(unistd.h netdb.h netinet/in.h sys/types.h sys/socket.h,,AC_MSG_ERROR([required header file missing]))
#AC_CHECK_FUNCS(gethostbyname socket htons connect shutdown,,AC_MSG_ERROR([required standard library function missing]))

AC_CONFIG_FILES([
 Makefile
])
AC_OUTPUT

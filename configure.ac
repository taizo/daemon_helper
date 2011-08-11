dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT
AC_CONFIG_SRCDIR([configure.ac])
AC_CANONICAL_TARGET([])
AM_INIT_AUTOMAKE(daemon_helper, 0.0.1)
AC_CONFIG_HEADERS([config.h])
dnl AM_DISABLE_STATIC

AC_PROG_CC
AC_PROG_CPP
AC_PROG_YACC
AM_PROG_LEX
AM_WITH_DMALLOC
AM_MAINTAINER_MODE


AC_LIBTOOL_DLOPEN
AC_LIBLTDL_INSTALLABLE
AC_PROG_LIBTOOL
dnl AC_CONFIG_SUBDIRS(libltdl)

AC_ISC_POSIX

dnl Switch to C++ mode and check for needed C++ headers
AC_PROG_CXX
AC_PROG_CXXCPP

AC_LANG([C++])

AM_PROG_CC_STDC
AC_HEADER_STDC

AC_SYS_LARGEFILE

AC_CHECK_SIZEOF(unsigned char,1)
AC_CHECK_SIZEOF(unsigned short,2)
AC_CHECK_SIZEOF(unsigned int,4)
AC_CHECK_SIZEOF(unsigned long,4)
AC_CHECK_SIZEOF(unsigned long long,8)



pkglibdir=${libdir}/${PACKAGE}

dnl Use -Wall if we have gcc.
changequote(,)dnl
if test "x$GCC" = "xyes"; then
  case " $CFLAGS " in
  *[\ \	]-Wall[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wall" ;;
  esac
fi
changequote([,])dnl

dnl Debugging support
AC_MSG_CHECKING([whether using the debugging support])
AC_ARG_ENABLE(debug,
	[  --enable-debug          Enable debugging support [default=no]],
        [ENABLE_DEBUG=yes], [ENABLE_DEBUG=no])
AC_MSG_RESULT($ENABLE_DEBUG)
AM_CONDITIONAL(DEBUG, test "x$ENABLE_DEBUG" = "xyes")


if test "x$ENABLE_DEBUG" = "xyes"; then
  AC_DEFINE(ENABLE_DEBUG, 1, [whether using the debugging support])
  CFLAGS="-O0 -g"
  CXXFLAGS="-O0 -g"

  dnl Memory leak detection support
  AC_MSG_CHECKING([whether using the memory leak detection support])
  AC_ARG_ENABLE(memory-leak-detect,
	[  --enable-memory-leak-detect          Enable memory leak detection support [default=no]],
        [ENABLE_MEMORY_LEAK_DETECT=yes], [ENABLE_MEMORY_LEAK_DETECT=no])
  AC_MSG_RESULT($ENABLE_MEMORY_LEAK_DETECT)

  if test "x$ENABLE_MEMORY_LEAK_DETECT" = "xyes" ; then
    AC_DEFINE(ENABLE_MEMORY_LEAK_DETECT, 1, [whether using memory leak detection])
  fi

  if test "${with_dmalloc+set}" = set; then
    withval="$with_dmalloc"
    if test "$withval" = yes; then
      LIBS="$LIBS -ldmallocthcxx"
    fi
  fi
fi

AC_SUBST(memstat_LDFLAGS)
case $host_os in
  solaris*)
    memstat_LDFLAGS="-lkvm -lkstat"
    ;;
  *)
    memstat_LDFLAGS=
    ;;
esac

AC_SUBST(INCLTDL)
AC_SUBST(LIBLTDL)

AC_CONFIG_FILES([
daemon_helper.spec
Makefile
src/Makefile
src/helper/Makefile
])
AC_OUTPUT


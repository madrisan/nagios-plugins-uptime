dnl Process this file with autoconf to produce a configure script.

m4_define(PROG_VERSION, 2)
m4_define(PROG_NAME, [nagios-plugins-linux-uptime])
m4_define(PROG_BUGREPORT, [davide.madrisan@gmail.com])

AC_INIT([PROG_NAME],[PROG_VERSION],[PROG_BUGREPORT],[PROG_NAME])
AC_PREREQ([2.59])
AC_CONFIG_SRCDIR([src/check_uptime.c])
AM_INIT_AUTOMAKE([gnu dist-bzip2])
AC_CONFIG_HEADERS(config.h:config.hin)

AM_MAINTAINER_MODE
AC_CANONICAL_HOST

dnl Checks for programs
AC_PROG_CC
AC_PROG_GCC_TRADITIONAL

dnl Checks for header files
AC_HEADER_STDC

AC_PREFIX_DEFAULT(/usr/local/nagios)

#AC_C_PROTOTYPES
#AC_C_CONST
#AC_LANG([C])
#AM_PROG_CC_C_O

AC_CONFIG_FILES([
   Makefile
   src/Makefile
   ])
AC_OUTPUT

dnl Process this file with autoconf to produce a configure script.

m4_define(PROG_VERSION, 6)
m4_define(PROG_NAME, [nagios-plugins-uptime])
m4_define(PROG_BUGREPORT, [davide.madrisan@gmail.com])

AC_INIT([PROG_NAME],[PROG_VERSION],[PROG_BUGREPORT],[PROG_NAME])
AC_PREREQ([2.59])

AC_CONFIG_SRCDIR([src/check_uptime.c])
#AC_CONFIG_LIBOBJ_DIR([lib])
AC_CONFIG_HEADERS(config.h:config.hin)

AM_INIT_AUTOMAKE([gnu dist-bzip2])

AM_MAINTAINER_MODE
AC_CANONICAL_HOST

AC_SUBST(WARNINGS)

dnl Checks for programs
AC_PROG_CC
AC_PROG_GCC_TRADITIONAL
AC_PROG_RANLIB

dnl Checks for header files
AC_HEADER_STDC
AC_HEADER_TIME

AC_CHECK_HEADERS([sys/param.h])
# check sys/sysctl.h separately, as it requires other header
# on at least OpenBSD
AC_CHECK_HEADERS([sys/sysctl.h], [], [],
[[#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
]])
AC_CHECK_HEADERS(sys/time.h strings.h)

AC_CHECK_HEADERS(getopt.h err.h)
AC_MSG_CHECKING([for struct option in getopt])
AC_COMPILE_IFELSE(
  [AC_LANG_PROGRAM([[#include <getopt.h>]],
    [[struct option op; op.name;]])],
  [ac_cv_struct_option_getopt_h=yes],
  [ac_cv_struct_option_getopt_h=no])
AC_MSG_RESULT([$ac_cv_struct_option_getopt_h])

dnl Check for a sysinfo syscall
AC_CHECK_HEADERS([sys/sysinfo.h])
AC_MSG_CHECKING([for uptime in struct sysinfo])
AC_COMPILE_IFELSE(
  [AC_LANG_PROGRAM([[#include <sys/sysinfo.h>]],
    [[struct sysinfo info;
      sysinfo(&info);
      printf("Uptime = %d\n",info.uptime);]])],
  [ac_cv_struct_sysinfo_has_uptime=yes
   AC_DEFINE_UNQUOTED(HAVE_STRUCT_SYSINFO_WITH_UPTIME, 1,
     [Define to 1 if you have the member uptime in the struct sysinfo.])
  ],
  [ac_cv_struct_sysinfo_has_uptime=no])
AC_MSG_RESULT([$ac_cv_struct_sysinfo_has_uptime])

dnl Check for function sysctl (KERN_BOOTTIME) - FreeBSD
AC_CHECK_HEADERS([sys/types.h sys/param.h])

AC_MSG_CHECKING(for function sysctl (KERN_BOOTTIME))
AC_COMPILE_IFELSE(
  [AC_LANG_PROGRAM([[
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPES_H */

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif /* HAVE_SYS_PARAM_H */ 

#include <sys/sysctl.h>
#include <unistd.h>
#include <time.h>
   ]],[[
size_t len;
struct timeval uptime;
int mib[2];

mib[0] = CTL_KERN;
mib[1] = KERN_BOOTTIME;

len = sizeof(uptime);
sysctl(mib, 2, &uptime, &len, 0, 0);]])],
  [ac_cv_function_sysctl_kern_boottime=yes
   AC_DEFINE_UNQUOTED(HAVE_FUNCTION_SYSCTL_KERN_BOOTTIME, 1,
     [Define to 1 if 'KERN_BOOTTIME' exists.])
  ],
  [ac_cv_function_sysctl_kern_boottime=no])
AC_MSG_RESULT([$ac_cv_function_sysctl_kern_boottime])

dnl Check for kstat.h and libkstat - Solaris
AC_CHECK_HEADERS([kstat.h],
  [AC_SEARCH_LIBS(kstat_open, kstat,
     [AC_DEFINE([HAVE_LIBKSTAT], 1,
        [Define to 1 if you have the 'libkstat' library (-lkstat)])])])

dnl Check for libperfstat.h - AIX
AC_SEARCH_LIBS(perfstat_memory_total, perfstat, 
  [AC_DEFINE([HAVE_LIBPERFSTAT], 1,
     [Define to 1 if you have the 'libperfstat' library (-lperfstat)])])

AC_PREFIX_DEFAULT(/usr/local/nagios)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_PROTOTYPES

AC_MSG_CHECKING(for gcc compiler warnings)
AC_ARG_ENABLE(warnings,
[  --disable-warnings   disable compiler warnings],
[if test "${enableval}" = no -o "$GCC" = no; then
   AC_MSG_RESULT(no)
   WARNINGS=
else
   AC_MSG_RESULT(yes)
   WARNINGS="\
-Wall -Wstrict-prototypes -Wmissing-prototypes -Wpointer-arith \
-Wmissing-declarations -Wredundant-decls -Wnested-externs \
-Wsign-compare -Wreturn-type -Wswitch -Wshadow \
-Wwrite-strings -Wextra -Wunused-parameter -Wformat=2"
fi], [
if test "$GCC" = yes; then
   AC_MSG_RESULT(yes)
   WARNINGS="\
-Wall -Wstrict-prototypes -Wmissing-prototypes -Wpointer-arith \
-Wmissing-declarations -Wredundant-decls -Wnested-externs \
-Wsign-compare -Wreturn-type -Wswitch -Wshadow \
-Wwrite-strings -Wextra -Wunused-parameter -Wformat=2"
else
   WARNINGS=
   AC_MSG_RESULT(no)
fi])

dnl Provide implementation of some required functions if necessary
AC_REPLACE_FUNCS(getopt_long)

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

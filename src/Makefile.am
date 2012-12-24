AM_CFLAGS = @WARNINGS@

noinst_LIBRARIES = libcompat.a
libcompat_a_SOURCES = nputils.c nputils.h compat_getopt.h
libcompat_a_LIBADD = $(LIBOBJS)

libexec_PROGRAMS = check_uptime

check_uptime_SOURCES = check_uptime.c
check_uptime_LDADD = libcompat.a

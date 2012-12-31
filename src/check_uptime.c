/*
 * License: GPL
 * Copyright (c) 2010,2012 Davide Madrisan <davide.madrisan@gmail.com>
 *
 * A Nagios plugin to check the time the server is running
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <fcntl.h>
#include <getopt.h>

#if HAVE_KSTAT_H
#include <kstat.h>
#endif

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

#if HAVE_SYS_SYSINFO_H
#include <sys/sysinfo.h>
#endif

#if TIME_WITH_SYS_TIME
#include <sys/time.h>
#include <time.h>
#else
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#include <time.h>
#endif
#endif

#include <unistd.h>

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#include <sys/sysctl.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#include "nputils.h"

#if !defined(restrict) && __STDC_VERSION__ < 199901
#if __GNUC__ > 2 || __GNUC_MINOR__ >= 92
#define restrict __restrict__
#else
#define restrict
#endif
#endif

#define BUFSIZE 127
static char buf[BUFSIZE + 1];
static char result_line[BUFSIZE + 1], perfdata_line[BUFSIZE + 1];

int uptime (double *restrict);
char *sprint_uptime (double);

#define SET_IF_DESIRED(x,y) do { if(x) *(x) = (y); } while(0)

static void __attribute__ ((__noreturn__)) print_version (void)
{
  puts (PACKAGE_NAME " version " PACKAGE_VERSION);
  exit (STATE_OK);
}

static const struct option longopts[] = {
  {"critical", 1, 0, 'c'},
  {"warning", 1, 0, 'w'},
  {"help", 0, 0, 'h'},
  {"version", 0, 0, 'V'},
  {NULL, 0, 0, 0}
};

static void __attribute__ ((__noreturn__)) usage (FILE * out)
{
  fputs (PACKAGE_NAME " ver." PACKAGE_VERSION " - \
check the time the server is running\n\
Copyright (C) 2012 Davide Madrisan <" PACKAGE_BUGREPORT ">\n", out);
  fputs ("\n\
  Usage:\n\
\t" PACKAGE_NAME " [--warning [@]start:end] [--critical [@]start:end]\n\
\t" PACKAGE_NAME " --help\n\
\t" PACKAGE_NAME " --version\n\n", out);
  fputs ("\
  Where:\n\
\t1. start <= end\n\
\t2. start and \":\" is not required if start=0\n\
\t3. if range is of format \"start:\" and end is not specified, assume end is infinity\n\
\t4. to specify negative infinity, use \"~\"\n\
\t5. alert is raised if metric is outside start and end range (inclusive of endpoints)\n\
\t6. if range starts with \"@\", then alert if inside this range (inclusive of endpoints)\n\n", out);
  fputs ("\
  Examples:\n\t" PACKAGE_NAME "\n\
\t" PACKAGE_NAME " --warning 30: --critical 15:\n\n", out);

  exit (out == stderr ? STATE_UNKNOWN : STATE_OK);
}

/* assume uptime never be zero seconds in practice */
enum
{
  UPTIME_RET_FAIL,
  UPTIME_RET_OK
};

int
uptime (double *restrict uptime_secs)
{
#if defined(HAVE_STRUCT_SYSINFO_WITH_UPTIME)

  struct sysinfo info;

  if (0 != sysinfo (&info))
    {
      perror ("cannot get the system uptime");
      return UPTIME_RET_FAIL;
    }
  SET_IF_DESIRED (uptime_secs, info.uptime);

  return UPTIME_RET_OK;

#elif defined(HAVE_FUNCTION_SYSCTL_KERN_BOOTTIME)

  int mib[2], now;
  size_t len;
  struct timeval system_uptime;

  mib[0] = CTL_KERN;
  mib[1] = KERN_BOOTTIME;

  len = sizeof (system_uptime);

  if (0 != sysctl (mib, 2, &system_uptime, &len, NULL, 0))
    return UPTIME_RET_FAIL;

  now = time (NULL);

  SET_IF_DESIRED (uptime_secs, now - system_uptime.tv_sec);

  return UPTIME_RET_OK;

#elif defined(HAVE_KSTAT_H)

  kstat_ctl_t *kc;
  kstat_t *kp;
  kstat_named_t *kn;

  long hz;
  long secs;

  hz = sysconf (_SC_CLK_TCK);

  kc = kstat_open ();
  if (0 == kc)
    return UPTIME_RET_FAIL;

  kp = kstat_lookup (kc, "unix", 0, "system_misc");
  if (0 == kp)
    {
      kstat_close (kc);
      return UPTIME_RET_FAIL;
    }

  if (-1 == kstat_read (kc, kp, 0))
    {
      kstat_close (kc);
      return UPTIME_RET_FAIL;
    }
  kn = (kstat_named_t *) kstat_data_lookup (kp, "clk_intr");
  secs = kn->value.ul / hz;

  kstat_close (kc);

  SET_IF_DESIRED (uptime_secs, secs);
  return UPTIME_RET_OK;

#else

  return UPTIME_RET_FAIL;

#endif
}

char *
sprint_uptime (double uptime_secs)
{
  int upminutes, uphours, updays;
  int pos = 0;

  updays = (int) uptime_secs / (60 * 60 * 24);
  if (updays)
    pos += sprintf (buf, "%d day%s ", updays, (updays != 1) ? "s" : "");
  upminutes = (int) uptime_secs / 60;
  uphours = upminutes / 60;
  uphours = uphours % 24;
  upminutes = upminutes % 60;

  if (uphours)
    {
      pos +=
	sprintf (buf + pos, "%d hour%s %d min", uphours,
		 (uphours != 1) ? "s" : "", upminutes);
    }
  else
    pos += sprintf (buf + pos, "%d min", upminutes);

  return buf;
}

int
main (int argc, char **argv)
{
  int c, uptime_mins, status;
  char *critical = NULL, *warning = NULL;
  double uptime_secs;
  thresholds *my_threshold = NULL;

  while ((c = getopt_long (argc, argv, "c:w:hV", longopts, NULL)) != -1)
    {
      switch (c)
	{
	default:
	  usage (stderr);
	  break;
	case 'c':
	  critical = optarg;
	  break;
	case 'w':
	  warning = optarg;
	  break;
	case 'h':
	  usage (stdout);
	  break;
	case 'V':
	  print_version ();
	  break;
	}
    }

  status = set_thresholds (&my_threshold, warning, critical);
  if (status == NP_RANGE_UNPARSEABLE)
    usage (stderr);

  if (UPTIME_RET_OK == uptime (&uptime_secs))
    {
      uptime_mins = (int) uptime_secs / 60;
      status = get_status (uptime_mins, my_threshold);
      free (my_threshold);
    }
  else
    status = STATE_UNKNOWN;

  switch (status)
    {
    default:
      c = snprintf (result_line, BUFSIZE,
		    "UPTIME UNKNOWN: can't get system uptime counter");
      break;
    case STATE_CRITICAL:
      c = snprintf (result_line, BUFSIZE, "UPTIME CRITICAL:");
      break;
    case STATE_WARNING:
      c = snprintf (result_line, BUFSIZE, "UPTIME WARNING:");
      break;
    case STATE_OK:
      c = snprintf (result_line, BUFSIZE, "UPTIME OK:");
      break;
    }

  if (status != STATE_UNKNOWN)
    {
      snprintf (result_line + c, BUFSIZE - c, " %s",
		sprint_uptime (uptime_secs));
      snprintf (perfdata_line, BUFSIZE, "uptime=%d", uptime_mins);
    }

  printf ("%s|%s\n", result_line, perfdata_line);
  return status;
}

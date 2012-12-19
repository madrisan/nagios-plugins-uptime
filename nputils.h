#pragma once

#define STATE_OK        0
#define STATE_WARNING   1
#define STATE_CRITICAL  2
#define STATE_UNKNOWN   3

#ifndef TRUE
# define TRUE 1
#endif

#ifndef FALSE
# define FALSE 0
#endif

/* Return codes for _set_thresholds */
#define NP_RANGE_UNPARSEABLE 1
#define NP_WARN_WITHIN_CRIT  2

#define OUTSIDE 0
#define INSIDE  1

/* see: nagios-plugins-1.4.15/lib/utils_base.h */
typedef struct range_struct
{
  double start;
  int start_infinity;		/* FALSE (default) or TRUE */
  double end;
  int end_infinity;
  int alert_on;			/* OUTSIDE (default) or INSIDE */
} range;

typedef struct thresholds_struct
{
  range *warning;
  range *critical;
} thresholds;

int get_status (double, thresholds *);
int set_thresholds (thresholds **, char *, char *);

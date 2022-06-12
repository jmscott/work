/*
 *  Synopsis:
 *	Common time routines.
 */
#include <sys/time.h>
#include <time.h>
#include <sys/errno.h>
#include <string.h>
#include <stdio.h>

#include "jmscott/libjmscott.h"

static char	*RFC3339Nano = "%04d-%02d-%02dT%02d:%02d:%02d.%09ld+00:00";
static char	*RFC3339Micro = "%04d-%02d-%02dT%02d:%02d:%02d.%03ld+00:00";

/*
 *  Note:
 *	Will "struct timeval" become obsolete?
 */
char *
jmscott_RFC3339_timeval(char *buf, int buf_size, struct timeval *tv)
{
	if (!tv)
		return "null timeval";
	struct tm *t = gmtime(&tv->tv_sec);
	if (!t)
		return strerror(errno);

	/*
	 *  Format the buffer.
	 */
	return snprintf(buf, buf_size, RFC3339Micro,
		t->tm_year + 1900,
		t->tm_mon + 1,
		t->tm_mday,
		t->tm_hour,
		t->tm_min,
		t->tm_sec,
		tv->tv_usec
	) < buf_size ? (char *)0 : "time buffer too small";
}

char *
jmscott_RFC3339Nano_now(char *buf, int buf_size)
{
	struct timespec	now;
	struct tm *t;
	
        if (clock_gettime(CLOCK_REALTIME, &now) < 0)
		return strerror(errno);
	t = gmtime(&now.tv_sec);
	if (t == NULL)
		return strerror(errno);
	/*
	 *  Format the time buffer.
	 */
	return snprintf(buf, buf_size, RFC3339Nano,
		t->tm_year + 1900,
		t->tm_mon + 1,
		t->tm_mday,
		t->tm_hour,
		t->tm_min,
		t->tm_sec,
		now.tv_nsec
	) < buf_size ? (char *)0 : "time buffer too small";
}

/*
 *  Synopsis:
 *	Common time routines.
 */
#ifndef JMSCOTT_CLANG_TIME
#define JMSCOTT_CLANG_TIME

#include <time.h>
#include <sys/errno.h>
#include <string.h>

#include <stdio.h>

#if __APPLE__ == 1 && __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 101200

typedef enum {
	CLOCK_REALTIME,
	CLOCK_MONOTONIC,
	CLOCK_PROCESS_CPUTIME_ID,
	CLOCK_THREAD_CPUTIME_ID
} clockid_t;

extern int clock_gettime(clockid_t, struct timespec *);

#endif	/* APPLE MAC OSX */

static char	*RFC3339Nano = "%04d-%02d-%02dT%02d:%02d:%02d.%09ld+00:00";

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
	 *  Format the record buffer.
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

#endif

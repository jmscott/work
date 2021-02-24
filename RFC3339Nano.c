/*
 *  Synopsis:
 *	Write the current time as YYYY-MM-DDThh:mm:ss.ns+00:00 to stdout.
 *  Usage:
 *	NOW=$(RFC3339Nano)
 *  Exit Status:
 *	0	time written
 *	1	failed
 *  Note:
 *	Consider writing program RFC3339, for dates like
 *
 *	      1990-12-31T23:59:60Z
 *
 *	Not clear to me if is Z is appropriate in RFC 3339.
 *	See the "Examples" section of RFC.
 *
 *		https://tools.ietf.org/html/rfc3339
 *		
 */
#include <sys/errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

extern int	errno;

#if __APPLE__ == 1 && __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 101200

typedef enum {
	CLOCK_REALTIME,
	CLOCK_MONOTONIC,
	CLOCK_PROCESS_CPUTIME_ID,
	CLOCK_THREAD_CPUTIME_ID
} clockid_t;

extern int clock_gettime(clockid_t, struct timespec *);

#endif

static char 	*prog = "RFC3339Nano";
static char	*RFC3339Nano = "%04d-%02d-%02dT%02d:%02d:%02d.%09ld+00:00\n";

static void
die(char *msg)
{
	char buf[1024];

	strcpy(buf, prog);
	strcat(buf, ": ERROR: ");
	strcat(buf, msg);
	strcat(buf, "\n");

	write(2, buf, strlen(buf));
	_exit(1);
}

static void
die2(char *msg1, char *msg2)
{
	char buf[1024];

	strcpy(buf, msg1);
	strcat(buf, ": ");
	strcat(buf, msg2);
	die(buf);
}

int
main(int argc, char **argv)
{
	char tstamp[512];
	struct timespec	now;
	struct tm *t;
	int nwrite;

	if (argc != 1)
		die("wrong number arguments");
	(void)argv;
        if (clock_gettime(CLOCK_REALTIME, &now) < 0)
		die2("clock_gettime(REALTIME) failed", strerror(errno));
	t = gmtime(&now.tv_sec);
	if (!t)
		die2("gmtime() failed", strerror(errno));
	/*
	 *  Format the record buffer.
	 */
	nwrite = snprintf(tstamp, sizeof tstamp, RFC3339Nano,
		t->tm_year + 1900,
		t->tm_mon + 1,
		t->tm_mday,
		t->tm_hour,
		t->tm_min,
		t->tm_sec,
		now.tv_nsec
	);
	if (write(1, tstamp, nwrite) != nwrite)
		die2("write(stdout) failed", strerror(errno));
	_exit(0);
}

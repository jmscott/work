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

#include "jmscott/die.c"
#include "jmscott/posio.c"
#include "jmscott/time.c"

char 	*jmscott_progname = "RFC3339Nano";

static void
die(char *msg)
{
	jmscott_die(1, msg);
}

static void
die2(char *msg1, char *msg2)
{
	jmscott_die2(1, msg1, msg2);
}

int
main(int argc, char **argv)
{
	char now[37], *err;

	if (argc != 1)
		die("wrong number arguments");
	(void)argv;
	if ((err = jmscott_RFC3339Nano_now(now, sizeof now - 1)))
		die2("RFC3339Nano_now() failed", err);
	size_t len = strlen(now);
	now[len++] = '\n';
	now[len] = 0;

	if (jmscott_write(1, now, len))
		die2("write(stdout) failed", strerror(errno));
	_exit(0);
}

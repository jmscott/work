/*
 *  Synopsis:
 *	Is the modify time of a file less than a certain number of seconds.
 *  Usage:
 *	stale-mtime 60 run/flowd.pid
 *	stale-mtime <seconds> <path-to-file>
 *  Exit Status:
 *	0	file modify time is greater than or equal to now - <seconds>.
 *	1	file modify time is less than now - <seconds>.
 *	2	file does not exist.
 *	3	unexpected error
 */
#include <sys/errno.h>
#include <sys/stat.h>

#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "jmscott/die.c"
#include "jmscott/posio.c"

#define EXIT_STALE	0
#define EXIT_FRESH	1
#define EXIT_NO_EXIST	2
#define EXIT_FAULT	3

extern int	errno;

char *jmscott_progname = "stale-mtime";

static void
die(char *msg, int exit_status)
{
	jmscott_die(exit_status, msg);
}

static void
die2(char *msg1, char *msg2, int exit_status)
{
	jmscott_die2(exit_status, msg1, msg2);
}

int
main(int argc, char **argv)
{
	time_t now;

	time(&now);

	if (argc != 3)
		die("wrong number of arguments", EXIT_FAULT);

	/*
	 *  Verify the number of seconds of staleness is >= 0 and < 999999999.
	 */

	char *sec = argv[1];
	if (strlen(sec) > 9)
		die("too many chars in seconds: expected <= 9", EXIT_FAULT);
	for (char *p = sec;  *p;  p++) {
		char c = *p;

		if (c < '0' || c > '9')
			die("non digit character in seconds", EXIT_FAULT);
	}
	int stale_after = now - (time_t)atoi(sec);

	char *path = argv[2]; 
	if (strlen(path) >= 256)
		die("too many characters in file path (>= 256)", EXIT_FAULT);

	//  stat() the file to determine freshness.
	struct stat st;

	if (jmscott_stat(path, &st) < 0) {
		if (errno == ENOENT)
			exit(EXIT_NO_EXIST);
		die2(strerror(errno), path, EXIT_FAULT);
	}
	if (st.st_mtime < stale_after)
		exit(EXIT_STALE);
	exit(EXIT_FRESH);
}

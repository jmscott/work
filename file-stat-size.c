/*
 *  Synopsis:
 *	Write the file size in bytes on standard output
 *  Usage:
 *	file-stat-size <path-to-file>
 *  Exit Status
 *	0	size written to standard out
 *	1	unexpected error
 */
#include <sys/errno.h>
#include <string.h>

#include "jmscott/libjmscott.h"

#define EXIT_OK		0
#define EXIT_FAULT	1

extern int	errno;

char*		jmscott_progname = "file-stat-size";

static void
die(char *msg)
{
	jmscott_die(EXIT_FAULT, msg);
}

static void
die2(char *msg1, char *msg2)
{
	jmscott_die2(EXIT_FAULT, msg1, msg2);
}

static void
die3(char *msg1, char *msg2, char *msg3)
{
	jmscott_die3(EXIT_FAULT, msg1, msg2, msg3);
}

static void
_write(void *p, ssize_t nbytes)
{
	if (jmscott_write(1, p, nbytes) < 0)
		die2("write(stdout) failed", strerror(errno));
}

int
main(int argc, char **argv)
{
	char *path;
	struct stat st;
	char digits[128], *p;

	if (argc != 2)
		die("wrong number of arguments");
	path = argv[1];

	/*
	 *  Stat the file to get the size.
	 */
	if (jmscott_stat(path, &st) != 0)
		die3(path, "stat() failed", strerror(errno));

	/*
	 *  Convert size to decimal digit string.
	 */
	p = jmscott_ulltoa((unsigned long long)st.st_size, digits);
	*p++ = '\n';

	_write(digits, p - digits);
	_exit(0);
}

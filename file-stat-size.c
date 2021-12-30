/*
 *  Synopsis:
 *	Write the file size in bytes on standard output
 *  Usage:
 *	file-stat-size <path-to-file>
 *  Exit Status
 *	0	size written to standard out
 *	1	unexpected error
 */
#include <sys/stat.h>

#include "clang/die.c"
#include "clang/string.c"

#define EXIT_OK		0
#define EXIT_BAD_STAT	1
#define EXIT_BAD_ARGC	2
#define EXIT_BAD_WRITE	3

#define COMMON_NEED_DIE3
#define COMMON_NEED_ULLTOA
#define COMMON_NEED_WRITE
#include "common.c"

static void
die(char *msg)
{
	jmscott_die(1, msg);
}

static void
die2(char *msg)
{
	jmscott_die2(1, msg);
}

static void
die3(char *msg1, char *msg2, char *msg3)
{
	jmscott_die3(msg1, msg2, msg3);
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

	jmscott_progname = "file-stat-size";

	if (argc != 2)
		die("wrong number of arguments");
	path = argv[1];

	/*
	 *  Stat the file to get the size.
	 */
	if (stat(path, &st) != 0)
		die3(path, "stat() failed", strerror(errno));

	/*
	 *  Convert size to decimal digit string.
	 */
	p = ulltoa((unsigned long long)st.st_size, digits);
	*p++ = '\n';

	_write(digits, p - digits);
	exit(0);
}

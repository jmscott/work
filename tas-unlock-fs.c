/*
 *  Synopsis
 *	Remove a test and set file system lock in <path/to/lock>
 *  Usage:
 *	tas-unlock-fs <path/to/lock/file>
 *  Exit Status:
 *	0  -  lock exists and removed
 *	1  -  lock does not exist
 *	2  -  unexpected error
 */
#include <sys/errno.h>
#include <string.h>
#include <unistd.h>

#include "jmscott/libjmscott.h"

extern int	errno;
char *		jmscott_progname = "tas-unlock-fs";

#define EXIT_EXIST		0
#define EXIT_NO_EXIST		1
#define EXIT_ERR		2

static void
die(char *arg1)
{
	jmscott_die(EXIT_ERR, arg1);
}

static void
die3(char *arg1, char *arg2, char *arg3)
{
	jmscott_die3(EXIT_ERR, arg1, arg2, arg3);
}

int
main(int argc, char **argv)
{
	char *lock_path;

	if (argc != 2)
		die("wrong number of command arguments");

	lock_path = argv[1];
	if (jmscott_unlink(lock_path) == 0)
		_exit(EXIT_EXIST);
	if (errno == ENOENT)
		_exit(EXIT_NO_EXIST);
	die3("unlink(lock) failed", strerror(errno), lock_path);
}

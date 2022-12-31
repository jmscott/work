/*
 *  Synopsis
 *	Create a test and set lock in a posix file system.
 *  Usage:
 *	tas-lock-fs <path/to/lock/file>
 *  Exit Status:
 *	0  -  no lock exists, so create lock file in <path/to/lock/file>
 *	1  -  lock already exists
 *	2  -  unexpected error
 *  See:
 *	https://github.com/jmscott/work/blob/master/fs-tas-unlock.c
 *	https://en.wikipedia.org/wiki/Test-and-set
 *  Note:
 *	Should the lock file contain the process id of the creator?
 *	Having the process id means we can automate cleanup of stale lock
 *	files.  Perhaps the mtime is sufficent for cleanup of stale locks.
 */

#include <sys/errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "jmscott/libjmscott.h"

extern int	errno;

char *		jmscott_progname = "tas-lock-fs";

#define EXIT_CREATED		0
#define EXIT_EXISTS		1
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
	int fd;

	if (argc != 2)
		die("wrong number of command arguments");

	lock_path = argv[1];
	if (!lock_path[0])
		die("empty lock path");
	fd = jmscott_open(lock_path, O_CREAT | O_EXCL, 0640);
	if (fd < 0) {
		if (errno == EEXIST)
			_exit(EXIT_EXISTS);
		die3("open(lock) failed", strerror(errno), lock_path);
	}

	if (jmscott_close(fd) < 0)
		die3("close(lock) failed", strerror(errno), lock_path);
	_exit(EXIT_CREATED);
}

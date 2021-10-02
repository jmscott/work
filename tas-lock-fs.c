/*
 *  Synopsis
 *	Create a test and set lock in a posix file system.
 *  Usage:
 *	fs-tas-lock <path/to/lock/file>
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

extern int	errno;

static char *prog = "tas-lock-fs";

#define EXIT_CREATED		0
#define EXIT_EXISTS		1
#define EXIT_ERR		2

#ifndef PIPE_MAX
#define PIPE_MAX		4096
#endif

/*
 * Synopsis:
 *  	Fast, safe and simple string concatenator
 *  Usage:
 *  	buf[0] = 0
 *  	_strcat(buf, sizeof buf, "hello, world");
 *  	_strcat(buf, sizeof buf, ": ");
 *  	_strcat(buf, sizeof buf, "good bye, cruel world");
 */
static void
_strcat(char *tgt, int tgtsize, char *src)
{
	//  find null terminated end of target buffer
	while (*tgt++)
		--tgtsize;
	--tgt;

	//  copy non-null src bytes, leaving room for trailing null
	while (--tgtsize > 0 && *src)
		*tgt++ = *src++;

	// target always null terminated
	*tgt = 0;
}

static void
die(char *arg1)
{
	char msg[PIPE_MAX];

	_strcat(msg, sizeof msg, prog);
	_strcat(msg, sizeof msg, ": ERROR: ");
	_strcat(msg, sizeof msg, arg1);
	_strcat(msg, sizeof msg, "\n");

	write(2, msg, strlen(msg)); 
	_exit(EXIT_ERR);
}

static void
die2(char *arg1, char *arg2)
{
	char msg[PIPE_MAX];

	msg[0] = 0;
	_strcat(msg, sizeof msg, arg1);
	_strcat(msg, sizeof msg, ": ");
	_strcat(msg, sizeof msg, arg2);
	die(msg);
}

static void
die3(char *arg1, char *arg2, char *arg3)
{
	char msg[PIPE_MAX];

	msg[0] = 0;
	_strcat(msg, sizeof msg, arg1);
	_strcat(msg, sizeof msg, ": ");
	_strcat(msg, sizeof msg, arg2);

	die2(msg, arg3);
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
again_open:
	fd = open(lock_path, O_CREAT | O_EXCL, 0640);
	if (fd < 0) {
		if (errno == EEXIST)
			_exit(EXIT_EXISTS);
		if (errno == EINTR)
			goto again_open;
		die3("open(lock) failed", lock_path, strerror(errno));
	}

again_close:
	if (close(fd) < 0) {
		if (errno == EINTR)
			goto again_close;
		die3("close(lock) failed", lock_path, strerror(errno));
	}
	_exit(EXIT_CREATED);
}

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

extern int	errno;

#define EXIT_EXIST		0
#define EXIT_NO_EXIST		1
#define EXIT_ERR		2

static char *prog = "tas-unlock-fs";

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

int
main(int argc, char **argv)
{
	char *lock_path;

	if (argc != 2)
		die("wrong number of command arguments");

	lock_path = argv[1];
again:
	if (unlink(lock_path) == 0)
		_exit(EXIT_EXIST);
	if (errno == ENOENT)
		_exit(EXIT_NO_EXIST);
	if (errno != EINTR)
		die2("unlink(lock) failed", strerror(errno));
	goto again;
}

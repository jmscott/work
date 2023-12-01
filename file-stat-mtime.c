/*
 *  Synopsis:
 *	Extract the modify epoch of a file.
 *  Usage:
 *	file-stat-mtime <path/to/file>
 *  Exit Status:
 *	0	ok
 *	1	file does not exist
 *	2	error
 */
#include <sys/errno.h>
#include <string.h>

#include "jmscott/libjmscott.h"

extern int	errno;
char *		jmscott_progname = "file-stat-mtime";

static void
die(char *msg)
{
	jmscott_die(2, msg);
}

static void
die2(char *msg1, char *msg2)
{
	jmscott_die2(2, msg1, msg2);
}

static void
enoent(char *path)
{
	jmscott_die2(1, "file does not exist", path);
}

/*
 *  write() exactly nbytes bytes, restarting on interrupt and dieing on error.
 */
static void
_write(void *p, ssize_t nbytes)
{
	if (jmscott_write_all(1, p, nbytes))
		die2("write(stdout) failed", strerror(errno));
}

int
main(int argc, char **argv)
{
	int status;
	struct stat st;
	time_t now;

	time(&now);

	if (argc != 2)
		die("wrong number of arguments");

	char *path = argv[1];

	if (*path == 0)
		die("empty file path");
	status = jmscott_stat(path, &st);
	if (status < 0) {
		if (errno == ENOENT)
			enoent(path);
		die2("stat() failed", strerror(errno));
	}
	int mtime = (int)st.st_mtime;
	int len = 0, n, rem;
	char buf[22];

	buf[0] = '0';
	n = mtime;
	while (n != 0) {
		len++;
		n /= 10;
	}
	for (int i = 0;  i < len;  i++) {
        	rem = mtime % 10;
		mtime = mtime / 10;
		buf[len - (i + 1)] = rem + '0';
	}
	buf[len++] = '\n';
	_write(buf, len);
	_exit(0);
}

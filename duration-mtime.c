/*
 *  Synopsis:
 *	Extract the duration seconds of the modify time.
 *  Usage:
 *	duration-mtime <path/to/file>
 *  Exit Status:
 *	0	ok
 *	1	file does not exist
 *	2	error
 */
#include <sys/stat.h>
#include <sys/errno.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include "jmscott/die.c"
#include "jmscott/posio.c"

extern int errno;

char *jmscott_progname = "duration-mtime";

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
	if (jmscott_write(1, p, nbytes))
		die2("write(1) failed", strerror(errno));
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
	status = stat(path, &st);
	if (status < 0) {
		if (errno == ENOENT)
			enoent(path);
		die2("stat() failed", strerror(errno));
	}
	int duration = (int)(now - st.st_mtime);
	if (duration == 0) {
		_write("0\n", 2);
		_exit(0);
	}
	int len = 0, n, rem;
	char buf[22];

	buf[0] = '0';
	n = duration;
	while (n != 0) {
		len++;
		n /= 10;
	}
	for (int i = 0;  i < len;  i++) {
        	rem = duration % 10;
		duration = duration / 10;
		buf[len - (i + 1)] = rem + '0';
	}
	buf[len++] = '\n';
	_write(buf, len);
	_exit(0);
}

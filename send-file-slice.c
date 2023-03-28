/*
 *  Synopsis:
 *	Inclusively slice a chunk of a seekable file onto standard output.
 *  Usage:
 *	slice-file <start-offset> <end-offset> <file-path>
 *  Exit Status:
 *	0	ok, wrote <stop-offset> - <start-offset> bytes to stdout
 *	1	file length < stop-offset, no bytes written
 *	5	unexpected error.
 *  Note:
 *	An empty stream exits 0.  Why?
 */
#include <sys/errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "jmscott/libjmscott.h"

extern int	errno;

char *jmscott_progname = "send-file-slice";

static void
die(char *msg)
{
	jmscott_die(5, msg);
}

static void
die2(char *msg1, char *msg2)
{
	jmscott_die2(5, msg1, msg2);
}

int main(int argc, char **argv)
{
	size_t start, stop;
	char *err;

	argc--;
	if (argc != 3)
		die("wrong count of CLI args: expected 3");
	if ((err = jmscott_a2size_t(argv[1], &start)))
		die2("a2size_t(start) failed", err);
	if ((err = jmscott_a2size_t(argv[2], &stop)))
		die2("a2size_t(stop) failed", err);
	if (start > stop)
		die("start > stop");
	if (!argv[1][0])
		die("empty file path");

	int in;
	in = jmscott_open(argv[3], O_RDONLY, 0);
	if (in < 0)
		die2("open(input) failed", strerror(errno));

	char *status = jmscott_sendfile(in, 1, start, stop - start + 1);
	if (status)
		die2("sendfile() failed", status);
	if (jmscott_close(in) != 0)
		die2("close(in) failed", strerror(errno));
	_exit(0);
}

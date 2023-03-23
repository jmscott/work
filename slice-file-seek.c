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

char *jmscott_progname = "slice-file";

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

static ssize_t
_read(int fd, unsigned char *buf, size_t buf_size)
{
	ssize_t nr;

	nr = jmscott_read(fd, buf, buf_size);
	if (nr < 0)
		die2("read(input) failed", strerror(errno));
	return nr;
}

static void
_write(unsigned char *buf, size_t buf_size)
{
	if (jmscott_write(1, buf, buf_size))
		die2("write(stdout) failed", strerror(errno));
}

size_t
_lseek(int fd, off_t offset, int whence)
{
	off_t pos = jmscott_lseek(fd, offset, whence);
	if (pos < 0)
		die2("lseek() failed", strerror(errno));
	return (size_t)pos;
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

	off_t pos = jmscott_lseek(in, start, SEEK_SET);
	if (pos < 0)
		die2("lseek(input) failed", strerror(errno));

	//  write slice to standard output
	unsigned char buf[4096];

	int nwrite = stop - start + 1;		//  start==stop ==> 1 byte
	while (nwrite > 0) {
		int sz = sizeof buf;
		if (nwrite < sz)
			sz = nwrite;
		int nb = 0;
		nb += _read(in, buf, sz);
		if (nb == 0)
			_exit(1);		// at end of file
		_write(buf, nb);
		nwrite -= nb;
	}
	if (jmscott_close(in) != 0)
		die2("close(in) failed", strerror(errno));
	_exit(0);
}

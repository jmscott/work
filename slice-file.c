/*
 *  Synopsis:
 *	Slice a portion of a file onto standard output.
 *  Usage:
 *	slice-file <file-path> <start-offset> <end-offset>
 *  Exit Status:
 *	0	ok, wrote <stop-offset> - <start-offset> bytes to stdout
 *	1	ok, wrote less than <end-of-file> - <start> bytes to stdout
 *	5	unexpected error.
 *  Note:
 *	An empty stream exits 0.  Why?
 */

#include "jmscott/die.c"
#include "jmscott/string.c"
#include "jmscott/posio.c"

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

static size_t
_read(int fd, unsigned char *buf, size_t buf_size)
{
	size_t nr;

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

int main(int argc, char **argv)
{
	argc--;
	if (argc != 3)
		die("wrong count of CLI args: expected 3");

	char *err;

	if (!argv[1][0])
		die("empty file path");
	int in = jmscott_open(argv[1], O_RDONLY, 0);
	if (in < 0)
		die2("open(input) failed", strerror(errno));

	size_t start;
	if ((err = jmscott_a2size_t(argv[2], &start)))
		die2("a2size_t(start) failed", err);
	if (jmscott_lseek(in, 0, SEEK_SET) < 0)
		die2("lseek(input, start offset) failed", strerror(errno));

	size_t stop;
	if ((err = jmscott_a2size_t(argv[3], &stop)))
		die2("a2size_t(stop offset) failed", err);

	if (stop < start)
		die("stop < start");

	if (stop == start)
		_exit(0);

	if (jmscott_lseek(in, (off_t)start, SEEK_SET) < 0)
		die2("lseek(input) failed", strerror(errno));

	//  write slice to standard output
	unsigned char buf[JMSCOTT_ATOMIC_MSG_SIZE];

	int nwrite = stop - start;
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
	_exit(0);
}

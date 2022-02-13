/*
 *  Synopsis:
 *	Slice a portion of a file onto standard output.
 *  Usage:
 *	slice-file <start-offset> <end-offset> <path-to-file>
 *  Exit Status:
 *	0	ok, wrote <<stop-offset> - <start-offset> bytes to 
 *	5	unexpected error.
 *  Note:
 *	Need a file version that uses seek.
 *
 *	An empty stream exits 0.  Why?
 */

#include "jmscott/die.c"
#include "jmscott/string.c"
#include "jmscott/posio.c"

char *jmscott_progname = "slice-stdin";

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
_read(unsigned char *buf, size_t buf_size)
{
	size_t nr;

	nr = jmscott_read(1, buf, buf_size);
	if (nr < 0)
		die2("jmscott_read() failed", strerror(errno));
	return nr;
}

static void
_write(unsigned char *buf, size_t buf_size)
{
	if (jmscott_write(1, buf, buf_size) != (ssize_t)buf_size)
		die2("jmscott_write(stdout) failed", strerror(errno));
}

int main(int argc, char **argv)
{
	char *err;

	argc--;
	if (argc != 2)
		die("wrong count of CLI args: expected 2");

	size_t start_offset;
	if ((err = jmscott_a2size_t(argv[1], &start_offset)))
		die2("jmscott_a2size_t(start_offset) failed", err);

	size_t stop_offset;
	if ((err = jmscott_a2size_t(argv[2], &stop_offset)))
		die2("jmscott_a2size_t(stop_offset) failed", err);

	if (stop_offset < start_offset)
		die("stop_offset < start_offset");

	if (stop_offset == start_offset)
		_exit(0);

	size_t nr = 0, nread = 0;
	unsigned char buf[4096];
	while ((nr = _read(buf, sizeof buf)) > 0) {
		nread += nr;
		_write(buf, sizeof buf);
	}
	if (nr == 0)
		_exit(0);

	_exit(0);
}

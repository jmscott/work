/*
 *  Synopsis:
 *	Simple test to insure timed reads at EOF return 0.
 *  Usage:
 *	$ dd bs=512 if=/dev/zero of=x.dat count=1
 *	$ cc test-read-timeout-eof.c -L. -ljmscott && ./a.out x.dat; echo $?
 */
#include <sys/errno.h>
#include <fcntl.h>
#include <string.h>

#include "libjmscott.h"

extern int	errno;

char *jmscott_progname = "t";

static void
die(char *msg)
{
	jmscott_die(1, msg);
}

static void
die2(char *msg1, char *msg2)
{
	jmscott_die2(1, msg1, msg2);
}

int
main(int argc, char **argv)
{
	if (argc != 2)
		die("wrong number of command args");
	int fd = jmscott_open(argv[1], O_RDONLY, 0);
	if (fd < 0)
		die2("open(ro) failed", strerror(errno));

	char buf[256];

	int nr = jmscott_read_timeout(fd, buf, 256, 3000);
	if (nr != 256) {
		if (nr < 0)
			die2("read(b0) failed", strerror(errno));
		if (nr == 0)
			die("read(b0) returned 0");
		die("read(b0) return != 256");
	}
	nr = jmscott_read_timeout(fd, buf, 256, 20);
	if (nr != 256) {
		if (nr < 0)
			die2("read(b1) failed", strerror(errno));
		if (nr == 0)
			die("read(b1) returned 0");
		die("read(b1) return != 256");
	}
	nr = jmscott_read_timeout(fd, buf, 256, 20);
	if (nr < 0)
		die2("read(eof) failed", strerror(errno));
	if (nr > 0)
		die("read(eof) > 0");
	if (jmscott_read_timeout(fd, buf, 256, 20) != 0)
		die("read(eof2) != 0");
	if (jmscott_close(fd) < 0)
		die2("close(ro) failed", strerror(errno));
	return 0;
}

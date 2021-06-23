/*
 *  Synopsis:
 *	Extract the duration seconds of the modify time.
 *  Usage:
 *	mtime-duration <path/to/file>
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

extern int errno;
static char *prog = "-mtime";

static void
die(char *msg)
{
	char buf[1024];

	strcpy(buf, prog);
	strcat(buf, ": ERROR: ");
	strncat(buf, msg, sizeof buf - (strlen(buf) + 2));
	strncat(buf, "\n", sizeof buf - (strlen(buf) + 2));

	buf[sizeof buf - 2] = '\n';
	buf[sizeof buf - 1] = 0;
	write(2, buf, strlen(buf)); 
	_exit(2);
}

static void
die2(char *msg1, char *msg2)
{
	char buf[1024];

	strncpy(buf, msg1, sizeof buf - 1);
	strncat(buf, ": ", sizeof buf - (strlen(buf) + 1));
	strncat(buf, msg2, sizeof buf - (strlen(buf) + 1));
	die(buf);
}

static void
enoent(char *path)
{
	write(2, prog, strlen(prog));
	write(2, ": ERROR: file does not exist: ", 29);
	write(2, path, strlen(path));
	write(2, "\n", 1);
	_exit(1);
}

/*
 *  write() exactly nbytes bytes, restarting on interrupt and dieing on error.
 */
static void
_write(void *p, ssize_t nbytes)
{
	int nb = 0;

	AGAIN:

	nb = write(1, p + nb, nbytes);
	if (nb < 0) {
		if (errno == EINTR)
			goto AGAIN;
		die2("write(1) failed", strerror(errno));
	}
	nbytes -= nb;
	if (nbytes > 0)
		goto AGAIN;
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

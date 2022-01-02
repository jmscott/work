/*
 *  Synopsis:
 *	Restartable versions of well known posix i/o functions.
 *  Usage:
 *	#include "jmscott/include/posio.c"
 */

#ifndef JMSCOTT_CLANG_POSIO
#define JMSCOTT_CLANG_POSIO

/*
 *  Synopsis:
 *	Restartable read() of up to "nbytes".
 *  Exit Status:
 *	>=0	nb bytes read() successfully
 *	-1	error occured, consult errno.
 */
static ssize_t
jmscott_read(int fd, void *p, ssize_t nbytes)
{
	ssize_t nb;

AGAIN:
	nb = read(fd, p, nbytes);
	if (nb >= 0)
		return nb;
	if (errno == EINTR)
		goto AGAIN;
	return -1;
}

/*
 *  Synopsis:
 *	Read exactly "size" bytes into "blob".
 *  Usage:
 *	static void
 *	_slurp(int fd, void *blob, ssize_t size) {
 *		switch (jmscott_read_blob(fd, blob, size)) {
 *		case 0:
 *			return 0;
 *		case -1:
 *			jmscott_die2("error slurping blob", strerror(errno));
 *		case -2:
 *			jmscott_die("unexpected end-of-file on blob");
 *		case -3:
 *			jmscott_die("end-of-file not sean slurping blob");
 *		}
 *	}
 *  Returns:
 *	0	read exactly "size" bytes into "blob".
 *	
 *	Upon error byte values in *blob are undefined.
 *
 *	-1	read() error, consult errno
 *	-2	unexpected  end-of-file reading.
 *	-3	unread bytes remain on stream.
 */
int
jmscott_read_exact(int fd, void *blob, ssize_t size)
{
	int nread = 0, nr;

AGAIN:
	nr = jmscott_read(fd, blob + nread, size - nread);
	if (nr < 0)
		return -1;
	if (nr > 0) {
		nread += nr;
		if (nread < size)
			goto AGAIN;
		/*
		 *  Prove no unread bytes exist.
		 */
		switch (jmscott_read(fd, blob, 1)) {
		case -1:
			return -1;
		case 0:
			return 0;
		}
		return -3;
	}
	return -2;
}

/*
 *  Synopsis:
 *	write() exactly nbytes, restarting on interrupt.
 *  Returns:
 *	0	wrote "nbytes" with no error.
 *	-1	error in write(), consult errno.
 */
int
jmscott_write(int fd, void *p, ssize_t nbytes)
{
	int nb = 0;

AGAIN:
	nb = write(fd, p + nb, nbytes);
	if (nb < 0) {
		if (errno == EINTR)
			goto AGAIN;
		return -1;
	}
	nbytes -= nb;
	if (nbytes > 0)
		goto AGAIN;
	return 0;
}

#endif	//  JMSCOTT_CLANG_POSIO
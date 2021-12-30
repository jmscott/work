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
	if (errno == EINTR)		//  try read()
		goto AGAIN;
	return -1;
}

/*
 *  Synopsis:
 *	Read exactly "size" bytes into "blob".
 *  Usage:
 *	static void
 *	_read_blob(int fd, void *blob, ssize_t size) {
 *		if jmscott_read_blob(fd, blob, size) >= 0)
 *			return 0;
 *		jmscott_die2("can not slurp blob", strerror(errno));
 *	}
 *  Exit Status:
 *	0	read exactly "size" bytes into "blob".
 *	
 *	Values in "blob" are undefined
 *
 *	-1	read() error, consult errno
 *	-2	premature end-of-file with no error.
 *	-3	unread bytes remain on stream, with no error.
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
 */
static int
jmscott_write(int fd, void *p, ssize_t nbytes)
{
	int nb = 0;

AGAIN:
	nb = write(fd, p + nb, nbytes);
	if (nb < 0) {
		if (errno == EINTR)
			goto again;
		return -1;
	}
	nbytes -= nb;
	if (nbytes > 0)
		goto AGAIN;
	return 0;
}

#endif	//  JMSCOTT_CLANG_POSIO

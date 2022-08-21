/*
 *  Synopsis:
 *	Restartable versions of well known posix i/o functions.
 *  Usage:
 *	#include "jmscott/include/posio.c"
 *  Note:
 *	Consider refactoring indefinite reads to simply set timeout==0.
 */

#include <sys/errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include "jmscott/libjmscott.h"

/*
 *  Synopsis:
 *	Restartable read() of up to "nbytes".
 *  Exit Status:
 *	>=0	nb bytes read() successfully
 *	-1	error occured, consult errno.
 */
ssize_t
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
 *  Posix poll() for readable input data, timeout in milliseconds.
 *
 *  Exit Status:
 *	0	readable data exists
 *	1	timeout, no readable data exists
 *	-1	unknown error, see errno.
 *  Note:
 *	Not clear if signals in fds[0].revents interpreted correctly.
 */
int
jmscott_poll_POLLIN(int fd, int msec)
{
	struct pollfd fds[1];

	fds[0].fd = fd;
	fds[0].events = POLLIN;
	int status;
AGAIN:
	status = poll(fds, 1, msec);
	if (status == 0)
		return 1;
	if (status < 0) {
		if (errno == EINTR || errno == EAGAIN)
			goto AGAIN;
		return -1;
	}

	//  signals handled properly?
	return (fds[0].revents & POLLIN) ? 0 : 1;
}

/*
 *  Synopsis:
 *	Restartable read() of up to "nbytes", with timeout 
 *  Exit Status:
 *	>=0	nb bytes read() successfully
 *	-1	unknown error, see errno
 *	-2	timeout
 *
 *  Note:
 *	How does poll() handle end of file?
 */
ssize_t
jmscott_read_timeout(int fd, void *p, ssize_t nbytes, int msec)
{
	if (msec == 0)
		return jmscott_read(fd, p, nbytes);

	int status;
	status = jmscott_poll_POLLIN(fd, msec);
	if (status == 1)
		return -2;
	if (status < 0)
		return -1;
	
	//  readable data exists, do the read
	return jmscott_read(fd, p, nbytes);
}

/*
 *  Synopsis:
 *	Read exactly "size" bytes into "blob" or indicate why not.
 *  Usage:
 *	static void
 *	_slurp(int fd, void *blob, ssize_t size) {
 *		switch (jmscott_read_exact(fd, blob, size)) {
 *		case 0:
 *			return 0;
 *		case -1:
 *			jmscott_die2("read(exact) failed", strerror(errno));
 *		case -2:
 *			jmscott_die("unexpected end-of-file");
 *		case -3:
 *			jmscott_die("unread bytes > size");
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
	int nread = 0;
	ssize_t nr;

AGAIN:
	nr = jmscott_read(fd, blob + nread, size - nread);
	if (nr < 0)
		return -1;
	if (nr > 0) {
		nread += nr;
		if (nread < size)
			goto AGAIN;

		//  prove no remaining bytes exist to read
		nr = jmscott_poll_POLLIN(fd, 0);
		if (nr == 1)
			return 0;
		if (nr == 0)
			return -3;
		return -1;
	}
	return -2;
}

/*
 *  Synopsis:
 *	Read exactly "size" bytes into "blob" or indicate why not.
 *  Usage:
 *	static void
 *	_slurp(int fd, void *blob, ssize_t size) {
 *		switch (jmscott_read_exact(fd, blob, size)) {
 *		case 0:
 *			return 0;
 *		case -1:
 *			jmscott_die2("read(exact) failed", strerror(errno));
 *		case -2:
 *			jmscott_die("unexpected end-of-file");
 *		case -3:
 *			jmscott_die("unread bytes > size");
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
 *	-4	timeout before reading exactly "size" bytes
 */
int
jmscott_read_exact_timeout(int fd, void *blob, ssize_t size, int msec)
{
	int nread = 0;
	ssize_t nr;

AGAIN:
	nr = jmscott_read_timeout(fd, blob + nread, size - nread, msec);
	if (nr < 0) {
		if (nr == -2)
			return -4;
		return -1;
	}
	if (nr > 0) {
		nread += nr;
		if (nread < size)
			goto AGAIN;

		//  prove no remaining bytes exist to read
		nr = jmscott_poll_POLLIN(fd, 0);
		if (nr == 1)
			return 0;
		if (nr == 0)
			return -3;
		return -1;
	}
	return -2;
}

/*
 *  Synopsis:
 *	write() exactly nbytes, restarting on interrupt.
 *  Returns:
 *	0	wrote "nbytes" with no error.
 *	-1	error in write(), consult errno.
 *  Note:
 *	Need to rename to jmscott_write_all().
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

/*
 *  Synopsis:
 *	lseek() to a file position, restarting on intearrupt.
 *  Returns:
 *	0	seek ok
 *	-1	error in seek(), consult errno.
 */
off_t
jmscott_lseek(int fd, off_t offset, int whence)
{
	off_t where;
AGAIN:
	where = lseek(fd, offset, whence);
	if (where >= 0)
		return where;
	if (errno == EINTR)
		goto AGAIN;
	return -1;
}

/*
 *  Synopsis:
 *	open a file, restarting on interrupt.
 *  Returns:
 *	0	opened file, returned file descriptor
 *	-1	error in open(), consult errno.
 */
int
jmscott_open(char *path, int oflag, mode_t mode)
{
	int fd;
AGAIN:
	fd = open(path, oflag, mode);
	if (fd >= 0)
		return fd;
	if (errno == EINTR)
		goto AGAIN;
	return -1;
}

/*
 *  Synopsis:
 *	close a file, restarting on interrupt.
 *  Returns:
 *	0	closed file
 *	-1	error in open(), consult errno.
 */
int
jmscott_close(int fd)
{
AGAIN:
	if (close(fd) == 0)
		return 0;
	if (errno == EINTR)
		goto AGAIN;
	return -1;
}

int
jmscott_fstat(int fd, struct stat *buf)
{
AGAIN:
	if (fstat(fd, buf) == 0)
		return 0;
	if (errno == EINTR)
		goto AGAIN;
	return -1;
}

int
jmscott_stat(char *path, struct stat *buf)
{
AGAIN:
	if (stat(path, buf) == 0)
		return 0;
	if (errno == EINTR)
		goto AGAIN;
	return -1;
}

/*
 *  Make a directory path.
 */
int
jmscott_mkdir(const char *path, mode_t mode)
{
AGAIN:
        if (mkdir(path, mode)) {
                if (errno == EINTR)
                        goto AGAIN;
                return -1;
        }
        return 0;
}

/*
 *  Make a directory path with with no error if dir already exists.
 *
 *  Exit Status:
 *	0	dir exists (mode may not match)
 *	-1	error occured, consult errno
 *
 *  Note:
 *	When dir exists, the mode is not rechecked, which is consistent
 *	with the command "mkdir -p <path>".
 */
int
jmscott_mkdir_EEXIST(const char *path, mode_t mode)
{
AGAIN:
	if (jmscott_mkdir(path, mode) == 0 || errno == EEXIST)
		return 0;
	if (errno == EINTR)
		goto AGAIN;
	return -1;
}

int
jmscott_link(const char *old_path, const char *new_path)
{
AGAIN:
        errno = 0;
        if (link(old_path, new_path) == 0)
                return 0;
        if (errno == EINTR)
                goto AGAIN;
        return -1;
}

int
jmscott_unlink(const char *path)
{
AGAIN:
        errno = 0;
        if (unlink(path) == 0)
                return 0;
        if (errno == EINTR)
                goto AGAIN;
        return -1;
}

int
jmscott_access(const char *path, int mode)
{
AGAIN:
        errno = 0;
        if (access(path, mode) == 0)
                return 0;
        if (errno == EINTR)
                goto AGAIN;
        return -1;
}

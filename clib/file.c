/*
 *  Synopsis:
 *	Helpful, common file manipulation functions.
 *  Note:
 *	Move sendfile.c to file.c, so other helper file funcs can be added.
 */
#include <sys/errno.h>
#include <string.h>
#include <unistd.h>

#include "jmscott/libjmscott.h"

extern int	errno;

#if defined(__APPLE__)
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#else
#include <sys/sendfile.h>
#endif

/*
 *  Synopsis:
 *	write() exactly nbytes, restarting on interrupt.
 *  Returns:
 *	0	wrote "nbytes" with no error.
 *	-1	error in write(), consult errno.
 */
int
jmscott_write_all(int fd, void *p, ssize_t nbytes)
{
	int nb = 0;

	while (nbytes > 0) {
		nb = jmscott_write(fd, p + nb, nbytes);
		if (nb < 0)
			return -1;
		nbytes -= nb;
	}
	return 0;
}

#if defined(__APPLE__)
static char *
copyio(int in, int out, long long *send_size)
{
	ssize_t nr, total = 0;

	char buf[4096];
	while ((nr = jmscott_read(in, buf, sizeof buf)) > 0) {
		if (jmscott_write_all(out, buf, nr) < 0)
			return strerror(errno);
		total += nr;
	}
	if (nr < 0)
		return strerror(errno);
	if (send_size)
		*send_size = total;
	return (char *)0;
}

static char *
apple_sendfile(int in_fd, int out_fd, long long *send_size)
{
	off_t sz = 0;

	int status;
AGAIN:
	status = sendfile(
			in_fd,
			out_fd,
			(off_t)0,
			&sz,
			(struct sf_hdtr *)0,
			0
	);
	if (status < 0) {
		if (errno == EINTR || errno == EAGAIN)
			goto AGAIN;
		if (errno == ENOTSOCK)
			return copyio(in_fd, out_fd, send_size);
		return strerror(errno);
	}

	//  macos insures all bytes sent in first call to sendfile();

	if (send_size)
		*send_size = (long long)sz;
	return (char *)0;
}
#endif

/*
 *  send whole file, using sendfile() when appropriate or falling
 *  back to read/write copy.
 *
 *  on non-apple, if *send_size == 0, then set *send_size to stat size.
 *  on apple, *send_size update to actualy bytes written (up to EOF).
 */
char *
jmscott_send_file(int in_fd, int out_fd, long long *send_size)
{
#if defined(__APPLE__)
	return apple_sendfile(in_fd, out_fd, send_size);
#else
	long long sz;

	struct stat st;

	if (jmscott_fstat(in_fd, &st))
		return strerror(errno);
	sz = (long long)st.st_size;

	//  probably linux sendfile() semantics.

	size_t len = sz;
	int nw;
AGAIN:
	nw = sendfile(out_fd, in_fd, (off_t *)0, len);
	if (nw < 0) {
		if (errno == EINTR || errno == EAGAIN)
			goto AGAIN;
		return strerror(errno);
	}
	len -= nw;
	if (len > 0)
		goto AGAIN;
	if (send_size)
		*send_size = sz;
	return (char *)0;
	goto AGAIN;
#endif

}

char *
jmscott_fsizeat(int at_fd, const char *path, off_t *size)
{
	struct stat st;

	if (jmscott_fstatat(at_fd, path, &st, 0))
		return strerror(errno);
	*size = st.st_size;
	return (char *)0;
}

char *
jmscott_rewind(int fd)
{
	if (jmscott_lseek(fd, (off_t)0, SEEK_SET))
		return strerror(errno);
	return (char *)0;
}

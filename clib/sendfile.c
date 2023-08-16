/*
 *  Synopsis:
 *	Portable interface to non-posix sendfile().
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

//  mac os (<ventura) can not sendfile on non socket, so brute force

static char *
send_not_socket(int in, int out, off_t offset, off_t len)
{
	int status = jmscott_lseek(in, offset, SEEK_SET);
	if (status < 0)
		return strerror(status);

	int nr, nw;
	char buf[4096];
	while ((nr = jmscott_read(in, buf, sizeof buf)) > 0) {
		if ((nw = jmscott_write_all(out, buf, nr)) < 0)
			return strerror(errno);
		len -= nw;
		if (len == 0)
			return (char *)0;
	}
	if (nr < 0)
		return strerror(errno);
	if (len > 0)
		return "not all bytes written";
	return (char *)0;
}

char *
jmscott_sendfile(int in, int out, off_t offset, off_t len)
{
	int total = len;
	int status;
AGAIN:
#if defined(__APPLE__)
	status = sendfile(in, out, offset, &len, (struct sf_hdtr *)0, 0);
#else
	status = sendfile(out, in, &offset, len);
	len = status;
#endif
	if (status < 0) {
		if (errno == ENOTSOCK)
			return send_not_socket(in, out, offset, len);
		if (errno == EINTR || errno == EAGAIN)
			goto AGAIN;
		return strerror(errno);
	}
	if (len == 0)
		return "unexpected end of file";
	total -= len;
	if (total == 0)
		return (char *)0;
	offset += len;
	len = total;
	goto AGAIN;
}

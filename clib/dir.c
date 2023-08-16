/*
 *  Synopsis:
 *	Various helpful functions for posix direcotry manipulation.
 */
#include <sys/errno.h>
#include <fcntl.h>
#include <string.h>

#include "jmscott/libjmscott.h"

/*
 *  Note:
 *	May be able to do a real defer in clang/gcc/
 *	https://fdiv.net/2015/10/08/emulating-defer-c-clang-or-gccblocks
 */
#define DEFER {if (!err) err = strerror(errno); goto BYE;}

/*
 *  Synopsis:
 *	Make all directories in a full path, relative to parent dir.
 *  Description:
 *	Make all directories in a full path, relative to parent dir.
 *	Return (char *)0 if full path created;  otherwise, return string
 *	describing the error in english.  Modes/ownership of existing
 *	directories are not changed.
 *
 *  Note:
 *	Escaped forward and contiguous slashes cause an error, which is a bug.
 */
char *
jmscott_mkdirat_path(int at_fd, char *child_path, mode_t mode)
{
	char *err = (char *)0;

	if (!*child_path)
		return (char *)0;

	if (strstr(child_path, "\\/"))
		return "child path can not contain escaped /";
	if (strstr(child_path, "//"))
		return "child path contains contiguous //";

	char path[JMSCOTT_PATH_MAX + 1];
	path[0] = 0;

	char *p = jmscott_strcat(path, sizeof path, child_path);

	//  zap possible terminating '/'
	if (p[-1] == '/')
		p[-1] = 0;
	p = path;

	char *slash;

	while ((slash = strchr(p, '/'))) {
		*slash = 0;

		if (jmscott_mkdirat_EEXIST(at_fd, path, mode) < 0)
			DEFER;
		*slash = '/';
		p = slash + 1;
	}
	if (jmscott_mkdirat_EEXIST(at_fd, path, mode) < 0)
		DEFER;
BYE:
	return err;
}

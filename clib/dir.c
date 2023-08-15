/*
 *  Synopsis:
 *	Various helpful functions for posix direcotry manipulation.
 */
#include <sys/errno.h>
#include <fcntl.h>
#include <string.h>

#include "jmscott/libjmscott.h"

#define DEFER {err = strerror(errno); goto BYE;}

/*
 *  Make all directories in a full path, relative to parent dir.
 *  Return (char *) if full path created;  otherwise, return string
 *  describing the error in english.  Existing directories are not changed.
 *
 *  Note:
 *	Escaped forward slashes cause an error~
 */
char *
jmscott_mkdir_path(char *parent_path, char *child_path, mode_t mode)
{
	char *err = (char *)0;

	if (!*child_path)
		return (char *)0;
	if (!*parent_path)
		return "parent path is empty";

	if (strstr(parent_path, "\\/"))
		return "parent path can not contain escaped /";
	if (strstr(parent_path, "//"))
		return "parent path contains contiguous //";

	if (strstr(child_path, "\\/"))
		return "child path can not contain escaped /";
	if (strstr(child_path, "//"))
		return "child path contains contiguous //";

	int parent_fd = jmscott_open(
			parent_path,
			O_DIRECTORY,
			mode
	);
	if (parent_fd < 0)
		return strerror(errno);

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

		if (jmscott_mkdirat_EEXIST(parent_fd, path, mode) < 0)
			DEFER;
		*slash = '/';
		p = slash + 1;
	}
	if (jmscott_mkdirat_EEXIST(parent_fd, path, mode) < 0)
		DEFER;

BYE:
	if (parent_fd >= 0) {
		if (jmscott_close(parent_fd) != 0 && !err)
			err = strerror(errno);
	}
	return err;
}

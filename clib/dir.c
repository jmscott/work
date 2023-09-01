/*
 *  Synopsis:
 *	Various helpful functions for posix direcotry manipulation.
 */
#include <sys/errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

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

int
jmscott_mkdirat_EEXIST(int at_fd, const char *path, mode_t mode)
{
	if (jmscott_mkdirat(at_fd, path, mode) < 0 && errno != EEXIST)
		return -1;
	return 0;
}

//  Note: rewrite using jmscott_halloc() memory allocator!
char *
jmscott_scan_dir(
	DIR *dp,
	int (*filter)(struct dirent *),
	char ***entries,
	int *ent_count
) {
	int ent_size = 256;
	struct dirent *ep;
	int ec = 0;

	*entries = 0;
	char **entp = (char **)malloc(ent_size * sizeof *entries
	);
	if (!entp)
		return "malloc(entries) failed: out of memory";

	//  posix manual on readdir ambiguous about errno 
	errno = 0;
	while ((ep = jmscott_readdir(dp))) {
		if (filter) {
			int status = (*filter)(ep);
			if (status < 0)
				return "dir entry filter < 0";
			if (status == 0)
				continue;
		}
		ec++;
		if (ec > ent_size) {
			ent_size *= 2;
			entp = (char **)realloc(
						entp,
						ent_size * sizeof *entries
			);
			if (!entp)
				return "realloc(entries) failed: out of memory";
		}
		entp[ec] = strdup(ep->d_name);
		if (!entp[ec])
			return "strdup(entry) failed: out of memory";
	}
	if (errno > 0)
		return strerror(errno);
	*entries = entp;
	*ent_count = ec;
	return (char *)0;
}

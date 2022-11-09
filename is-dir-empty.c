/*
 *  Synopsis:
 *	Is a directory empty.
 *  Usage
 *	is-dir-emtpy <path/to/dir>
 *  Exit Status:
 *	0	yes, directory exists and is empty
 *	1	directory exists and is not empty
 *	2	exists but is not directory
 *	3	no entry exists
 *	4	error
 */
#include <sys/stat.h>
#include <sys/errno.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

#include "jmscott/libjmscott.h"

#define EXIT_IS_EMPTY		0
#define EXIT_NOT_EMPTY		1
#define EXIT_EXISTS_NOT_DIR	2
#define EXIT_NO_ENTRY		3
#define EXIT_ERROR		4

extern int errno;

char *jmscott_progname = "is-dir-empty";
static char *usage = "is-dir-empty <path/to/dir>";

int
main(int argc, char **argv)
{
	(void)argv;
	if (argc != 2)
		jmscott_die_argc(EXIT_ERROR, 1, argc -1, usage);
	char *path = argv[1];

	struct stat st;
	int status = jmscott_stat(path, &st);
	if (status < 0) {
		if (status == ENOENT)
			_exit(EXIT_NO_ENTRY);
		jmscott_die2(EXIT_ERROR, "stat() failed", strerror(status));
	}
	if (S_ISDIR(st.st_mode))
		_exit(0);
	_exit(EXIT_EXISTS_NOT_DIR);
}

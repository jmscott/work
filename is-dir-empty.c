/*
 *  Synopsis:
 *	Is a directory empty, suitable for shell scripts.
 *  Usage
 *	is-dir-emtpy <path/to/dir>
 *  Exit Status:
 *	0	yes, directory exists and is empty
 *	1	directory exists and is not empty
 *	2	exists but is not directory
 *	3	no entry exists
 *	4	error
 *  Note:
 *	Minor race conditions exist.
 */
#include <sys/stat.h>
#include <sys/errno.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <dirent.h>

#include "jmscott/libjmscott.h"

#define EXIT_IS_EMPTY		0
#define EXIT_NOT_EMPTY		1
#define EXIT_EXISTS_NOT_DIR	2
#define EXIT_NO_ENTRY		3
#define EXIT_ERROR		4

extern int errno;

char *jmscott_progname = "is-dir-empty";
static char *usage = "is-dir-empty <path/to/dir>";

static void
die(char *msg)
{
	jmscott_die(EXIT_ERROR, msg);
}

static void
die2(char *msg1, char *msg2)
{
	jmscott_die2(EXIT_ERROR, msg1, msg2);
}

int
main(int argc, char **argv)
{
	errno = 0;
	if (--argc != 1)
		jmscott_die_argc(EXIT_ERROR, argc, 1, usage);
	argv++;
	char *path = argv[0];

	struct stat st;
	int status = jmscott_stat(path, &st);
	if (status < 0) {
		if (errno == ENOENT)
			_exit(EXIT_NO_ENTRY);
		die2("stat() failed", strerror(errno));
	}
	if (!S_ISDIR(st.st_mode))
		_exit(EXIT_EXISTS_NOT_DIR);

	DIR *dirp = opendir(path);
	if (dirp == NULL) {
		if (errno == 0)
			die("opendir() and errno==0");
		if (errno == ENOENT)
			 _exit(EXIT_NO_ENTRY);
		die2("opendir() failed", strerror(errno));
	}

	struct dirent *dp;
	if (dirp == NULL)
		die2("opendir() failed", strerror(status));


	//  scan the directory for more than two entries.
	//  we assume "." and ".." always exist.
	int dir_count = 0;
	while ((dp = readdir(dirp)) != NULL)
		if (++dir_count > 2)
			_exit(EXIT_NOT_EMPTY);
	(void)closedir(dirp);
	_exit(EXIT_IS_EMPTY);
}

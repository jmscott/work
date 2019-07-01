/*
 *  Synopsis:
 *	Is a file all ascii text
 *  Usage:
 *	istext [-vEl] [file1 [file2 ...]]
 *	find . -type f -print | istext
 *  See:
 *	https://github.com/jmscott/work/istext.c
 *  Note:
 *	This program should have been named "isascii".
 *	A true "istext" would consult the locale and use icu, rendering a quite
 *	complex piece of code.  A less ambitious is supporting utf8 and utf16.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern void	*malloc();
char		*program;

static int	chunk_size	= 1024;
static char	*chunk = 0;
static int	sense = 1;
static int	empty_is_text = 0;

extern int	stat();
#ifdef NO_LSTAT
static int	(*statfunc)() = stat;
#else
extern int	lstat();
static int	(*statfunc)() = lstat;
#endif

/*
**  Read and check a check of bytes for "textness"
*/
static int
check_chunk(fd, pos, file_name)
int fd;
long pos;
char *file_name;
{
int nread;
register char c, *p;

	if (lseek(fd, pos, SEEK_SET) < 0)
	{
		perror(file_name);
		return 0;
	}
	if ((nread = read(fd, chunk, chunk_size)) < 0)
	{
		perror(file_name);
		return 0;
	}

	/*
	**  Scan for non-ascii or null char
	*/
	for (p = chunk, c = *p;  nread > 0;  c = *p++, nread--)
		if (c == 0 || !isascii(c))
			return 0;
	return 1;
}

/*
**  Is a file text ?
**
**	1	if text
**	0	if unknown
**	-1	if not text
*/
static int
istext(file_name)
char *file_name;
{
static int fd = -1;
struct stat stat_buf;

	if (fd > -1)
		close(fd);

	if ((*statfunc)(file_name, &stat_buf) != 0)
	{
		perror(file_name);
		return 0;
	}
	if (!S_ISREG(stat_buf.st_mode))
		return -1;
	if (stat_buf.st_size == 0)
		return empty_is_text ? 1 : -1;

	if ((fd = open(file_name, O_RDONLY)) < 0)
	{
		perror(file_name);
		return 0;
	}

	/*
	**  Check chunk_size bytes at start of file.
	*/
	if (!check_chunk(fd, (long)0, file_name))
		return -1;
	if (stat_buf.st_size < chunk_size)
		return 1;
	
	/*
	**  Check middle chunk_size bytes
	*/
	if (!check_chunk(fd, (long)(stat_buf.st_size / 2), file_name))
		return -1;
	if (stat_buf.st_size < chunk_size * 2)
		return 1;

	/*
	**  Check final chunk_size bytes
	*/
	if (!check_chunk(fd, (long)(stat_buf.st_size - chunk_size), file_name))
		return -1;
	close(fd);
	fd = -1;
	return 1;
}

static int
options(argc, argv)
int argc;
char **argv;
{
int i;

	for (i = 1;  i < argc;  i++)
		if (strcmp("-v", argv[i]) == 0)
			sense = -1;
		else if (strcmp("-e", argv[i]) == 0)
			empty_is_text = 1;
		else if (strcmp("-L", argv[i]) == 0)
			statfunc = stat;
		else if (strcmp("-help", argv[i]) == 0)
		{
			fprintf(stderr, "Usage: %s [-e][-v][-L] [file ...]\n",
						program);
			exit(0);
		}
		else
			break;
	return i;
}

int
main(int argc, char **argv)
{
int count = 0, i;
char file_name[BUFSIZ];

	if ((program = strrchr(argv[0], '/')))
		program++;
	else
		program = argv[0];
	i = options(argc, argv);
	if ((chunk = malloc(chunk_size)) == NULL)
	{
		fprintf(stderr, "malloc(%d) failed", chunk_size);
		exit(2);
	}
	if (i == argc)		/* file names on stdin */
		while (fgets(file_name, sizeof file_name, stdin))
		{
		int len;

			len = strlen(file_name);
			if (len > 0 && file_name[len - 1] == '\n')
				file_name[len - 1] = 0;
			if (istext(file_name) * sense > 0)
			{
				count++;
				puts(file_name);
			}
		}
	else while (i < argc)				/* Arguments */
		if (istext(argv[i++]) * sense > 0)
		{
			count++;
			puts(argv[i - 1]);
		}
	exit(count > 0 ? 0 : 1);
}

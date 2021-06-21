/*
 *  Synopsis:
 *	Write english description of elapsed modification time of a file.
 *  Usage:
 *	duration-mtime-english <path/to/file>
 *  Note:
 *	Add option --round-single-unit.
 */
#include <sys/stat.h>
#include <sys/errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>

#define ROUND(n,d) ((((n)<0)^((d)<0))?(((n)-(d)/2)/(d)):(((n)+(d)/2)/(d)))

extern int errno;
static char *prog = "duration-mtime-english";

static void
die(char *msg)
{
	char buf[1024];

	strcpy(buf, prog);
	strcat(buf, ": ERROR: ");
	strncat(buf, msg, sizeof buf - (strlen(buf) + 2));
	strncat(buf, "\n", sizeof buf - (strlen(buf) + 2));

	buf[sizeof buf - 2] = '\n';
	buf[sizeof buf - 1] = 0;
	write(2, buf, strlen(buf)); 
	exit(2);
}

static void
die2(char *msg1, char *msg2)
{
	char buf[1024];

	strncpy(buf, msg1, sizeof buf - 1);
	strncat(buf, ": ", sizeof buf - (strlen(buf) + 1));
	strncat(buf, msg2, sizeof buf - (strlen(buf) + 1));
	die(buf);
}

static void
enoent(char *path)
{
	write(2, prog, strlen(prog));
	write(2, ": ERROR: file does not exist: ", 29);
	write(2, path, strlen(path));
	write(2, "\n", 1);
	exit(1);
}

int
main(int argc, char **argv)
{
	int status;
	struct stat st;
	time_t now;

	time(&now);

	if (argc != 2)
		die("wrong number of arguments");

	char *path = argv[1];

	if (*path == 0)
		die("empty file path");
	status = stat(path, &st);
	if (status < 0) {
		if (errno == ENOENT)
			enoent(path);
		die2("stat() failed", strerror(errno));
	}
	int duration = now - st.st_mtime;

	char answer[1024];

	if (duration < 60)					//  seconds
		sprintf(answer, "%ds\n", duration);
	else if (duration < 3600) {				//  min & sec
		int min = duration / 60;
		int sec = duration - (min * 60);

		if (sec > 0)
			sprintf(answer, "%dm%ds\n", min, sec);
		else
			sprintf(answer, "%dm\n", min);
	} else if (duration < 86400) {				// hr & min
		int hr = duration / 3600;
		int min = ROUND(duration - (hr * 3600), 60);

		if (min > 0)
			sprintf(answer, "%dh%dm", hr, min);	//  hours
		else
			sprintf(answer, "%dh", hr);
	} else {						//  days & hr
		int day = duration / 86400;
		int hr = ROUND(duration - (day * 86400), 3600);

		if (hr > 0)
			sprintf(answer, "%dd%dh", day, hr);
		else
			sprintf(answer, "%dd", day);
	}
	write(1, answer, strlen(answer));
	return 0;
}

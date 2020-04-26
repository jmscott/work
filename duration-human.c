/*
 *  Synopsis:
 *	Write human readable, english duration using unix epoch at start time.
 *  Usage:
 *	START_EPOCH=$(date +%s)
 *	...
 *	echo "elpased time: $(duration-human $START_EPOCH)"
 *  Note:
 *	No rounding of <major><minor> calculations.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>

#define ROUND(n, d) ((((n) < 0) ^ ((d) < 0)) ? (((n) - (d)/2)/(d)) : (((n) + (d)/2)/(d)))

static char *prog = "duration-human";

static void
die(char *msg)
{
	char buf[1024];

	strcpy(buf, prog);
	strcat(buf, ": ERROR: ");
	strncat(buf, msg, 1024 - (strlen(buf) + 2));
	strncat(buf, "\n", 1024 - (strlen(buf) + 2));

	buf[sizeof buf - 2] = '\n';
	buf[sizeof buf - 1] = 0;
	exit(1);
}

int
main(int argc, char **argv)
{
	if (argc != 2)
		die("wrong number of arguments");

	char *se = argv[1], c;
	char *p = se;

	while ((c = *p++)) {
		if (!isdigit(c))
			die("start epoch: non-digit");
		if (p - se > 20)
			die("start epoch: > 20 digits");
	}
	time_t start_time = atoi(se);
	time_t now = time(NULL);
	if (start_time > now)
		die("start_time > now");

	int duration = now - start_time;

	char answer[1024];

	if (duration < 60)
		sprintf(answer, "%ds\n", duration);		// seconds
	else if (duration < 3600) {
		int sec = duration % 60;
		int min = (duration - sec) / 60;

		if (sec > 0)
			sprintf(answer, "%dm%ds\n", min, sec);	//  minutes
		else
			sprintf(answer, "%dm\n", duration);
	} else if (duration < 86400) {
		int min = duration % 3600;
		int hr = (duration - min) / 3600;

		if (min > 0)
			sprintf(answer, "%dh%dm", hr, min);	//  hours
		else
			sprintf(answer, "%dh", hr);
	} else {
		int hr = duration % 86400;
		int day = (duration - hr) / 86400;
		if (hr > 0)
			sprintf(answer, "%dd%dh", day, hr);	//  days
		else
			sprintf(answer, "%dd", day);
	}
	write(1, answer, strlen(answer));
	return 0;
}

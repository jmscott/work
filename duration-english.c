/*
 *  Synopsis:
 *	Write english description elapsed time from duration in integer seconds
 *  Usage:
 *	#!/bin/bash
 *
 *	#  on mac osx
 *	DURATION_SEC=$(expr $(date +%s) - $(date -v -1d +%s))
 *
 *	#  on gnu/linux
 *	DURATION_SEC=$(expr $(date +%s) - $(date -d yesterday +%s))
 *
 *	echo "elpased time: $(duration-english $DURATION_SEC)"
 *  Note:
 *	Add option --round-single-unit.
 *
 *	Improve parsing of duration seconds.
 *
 *	Should seconds be a floating point, for sub second timing?
 *
 *	No rounding of <major><minor> calculations.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>

#define ROUND(n, d) ((((n) < 0) ^ ((d) < 0)) ? (((n) - (d)/2)/(d)) : (((n) + (d)/2)/(d)))

static char *prog = "duration-english";

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
	write(2, buf, strlen(buf)); 
	exit(1);
}

int
main(int argc, char **argv)
{
	if (argc != 2)
		die("wrong number of arguments");

	char *d = argv[1], c;
	char *p = d;

	if (*d == 0)
		die("empty duration seconds");
	while ((c = *p++)) {
		if (!isdigit(c))
			die("duration seconds: non-digit");
		if (p - d > 20)
			die("duration seconds: > 20 digits");
	}
	int duration = atoi(d);
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

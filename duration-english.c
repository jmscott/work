/*
 *  Synopsis:
 *	Translate integer duration in seconds into english, similar to golang.
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
 *  Exit Status:
 *	0	ok
 *	1	unexpected error
 *  Note:
 *	Add option --round-single-unit.
 *
 *	Improve parsing of duration seconds.
 *
 *	Should seconds be a floating point, for sub second timing?
 *
 *	No rounding of <major><minor> calculations.
 */
#include <sys/errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

extern int	errno;

#include "jmscott/libjmscott.h"

#define ROUND(n, d) ((((n) < 0) ^ ((d) < 0)) ? (((n) - (d)/2)/(d)) : (((n) + (d)/2)/(d)))

char *jmscott_progname = "duration-english";

static void
die(char *msg)
{
	jmscott_die(2, msg);
}

static void
die2(char *msg1, char *msg2)
{
	jmscott_die2(2, msg1, msg2);
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
			sprintf(answer, "%dh%dm\n", hr, min);	//  hours
		else
			sprintf(answer, "%dh\n", hr);
	} else {						//  days & hr
		int day = duration / 86400;
		int hr = ROUND(duration - (day * 86400), 3600);

		if (hr > 0)
			sprintf(answer, "%dd%dh\n", day, hr);
		else
			sprintf(answer, "%dd\n", day);
	}
	if (jmscott_write_all(1, answer, strlen(answer)))
		die2("write(stderr) failed", strerror(errno));
	return 0;
}

/*
 *  Synopsis:
 *	Escape a json string read on standard input.
 *  Usage:
 *	JSON=$(echo 'hello, world' | escape-json-string)
 *  Exit Status:
 *	0		ok
 *	1		failed
 *  Note:
 *	Non ascii are not escaped properly!
 *
 *	What about escaping command line arguments?
 *
 *	Is code safe for UTF-8?
 */

#include <unistd.h>
#include <sys/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jmscott/die.c"

extern int errno;

char *jmscott_progname = "escape-json-string";

static void
die(char *msg)
{
	int eno = errno;

	if (eno > 0)
		jmscott_die2(1, msg, strerror(eno));
	jmscott_die(1 , msg);
}

int
main(int argc, char **argv)
{
	int c;

	(void)argv;
	errno = 0;
	if (argc != 1)
		die("wrong number of cli arguments");

	while ((c = getchar()) != EOF) {
		if (ferror(stdin))
			die("getchar(stdin) failed");
		switch (c) {
		case '\b':
			putchar('\\');
			putchar('b');
			break;
		case '\f':
			putchar('\\');
			putchar('f');
			break;
		case '\n':
			putchar('\\');
			putchar('n');
			break;
		case '\r':
			putchar('\\');
			putchar('r');
			break;
		case '\t':
			putchar('\\');
			putchar('t');
			break;
		case '"':
			putchar('\\');
			putchar('"');
			break;
		case '\\':
			putchar('\\');
			putchar('\\');
			break;
		default:
			putchar(c);
		}
		if (ferror(stdout))
			die("putchar(stdout) failed");
	}
	exit(0);
}

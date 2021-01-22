/*
 *  Synopsis:
 *	Escape a json string read on standard input.
 *  Usage:
 *	JSON=$(echo 'hello, world' | escape-json-string)
 *  Exit Status:
 *	0		ok
 *	1		failed
 *  Note:
 *	What about escaping command line arguments?
 *
 *	Is code safe for UTF-8?
 */

#include <unistd.h>
#include <sys/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int errno;

static char *prog = "escape-json-string";

static int die(char *msg)
{
	int eno = errno;

	write(2, prog, strlen(prog));
	write(2, ": ERROR: ", 8);
	write(2, msg, strlen(msg));
	if (eno > 0) {
		char *emsg = strerror(eno);

		write(2, ": ", 2);
		write(2, emsg, strlen(emsg));
	}
	write(2, "\n", 1);
	exit(1);
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

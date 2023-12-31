/*
 *  Synopsis:
 *	Convert byte count to english.
 */
#include <ctype.h>
#include <string.h>

extern int	errno;

#include "jmscott/libjmscott.h"

#define ROUND(n, d) ((((n) < 0) ^ ((d) < 0)) ? (((n) - (d)/2)/(d)) : (((n) + (d)/2)/(d)))

char *jmscott_progname = "byte-size-english";

static char *usage = "usage: byte-size-english <byte size>";

static void
die(char *msg)
{
        jmscott_die(1, msg);
}

static void
die2(char *msg1, char *msg2)
{
        jmscott_die2(1, msg1, msg2);
}

static void
_write(char *unit, int ull)
{
        char buf[30];

        char *p = jmscott_ulltoa(ull, buf);

        *p++ = unit[0];
	if (unit[1])
        	*p++ = unit[1];
        *p++ = '\n';
	*p = 0;

        if (jmscott_write(1, buf, p - buf) < 0)
                die2("write() failed", strerror(errno));
        _exit(0);
}

int
main(int argc, char **argv)
{
	if (argc != 2)
		jmscott_die_argc(1, 1, argc -1, usage);

        if (*argv[1] == 0)
                die("empty byte size");

	unsigned long long bs;
	char *err = jmscott_a2ui63(argv[1], &bs);
	if (err)
		die2("can not parse byte size", err);

	unsigned long long r = 1024;
        if (bs < r)
                _write("B", bs);

	r *= 1024;
        if (bs < r)
                _write("KB", ROUND(bs, r / 1024));

	r *= 1024;
        if (bs < r)
		_write("MB", ROUND(bs, r / 1024));

	r *= 1024;
        if (bs < r)
		_write("GB", ROUND(bs, r / 1024));

	r *= 1024;
        if (bs < r)
		_write("TB", ROUND(bs, r / 1024));

	r *= 1024;
        if (bs < r)
		_write("PB", ROUND(bs, r / 1024));
        return 0;
}


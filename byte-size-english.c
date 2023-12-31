/*
 *  Synopsis:
 *	Convert byte count to brief english K,M,G,T,P,E,Z,Y,R,Q in 1024 powers.
 */
#include <ctype.h>
#include <string.h>

#include "jmscott/libjmscott.h"

char *jmscott_progname = "byte-size-english";

extern int	errno;

static char english[] = {
	'B',			//  bytes
	'K',			//  kilo
	'M',			//  mega
	'G',			//  giga
	'T',			//  tera
	'P',			//  peta
	'E',			//  exa
	'Z',			//  zetta
	'Y',			//  yotta
	'R',			//  ronna
	'Q'			//  quetta
};

#define ROUND(n, d) (							\
		(((n) < 0) ^ ((d) < 0))					\
			?						\
			(((n) - (d)/2)/(d)) : (((n) + (d)/2)/(d))	\
		)

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

int
main(int argc, char **argv)
{
	if (argc != 2)
		jmscott_die_argc(1, 1, argc -1, usage);

        if (*argv[1] == 0)
                die("empty byte size");

	//  extract byte size

	unsigned long long bs;
	char *err = jmscott_a2ui63(argv[1], &bs);
	if (err)
		die2("can not parse byte size", err);

	unsigned long long r = 1024;
	for (int i = 0;  i < 11;  i++, r *= 1024) {
		if (bs >= r)
			continue;

        	char buf[30];

        	char *p = jmscott_ulltoa(ROUND(bs, r / 1024), buf);
		*p++ = english[i];
		if (i == 0)
			*p++ = 's';
		else
			*p++ = 'b';
		*p = '\n';
		if (jmscott_write(1, buf, p - buf) < 0)
			die2("write(1) failed", strerror(errno));
		_exit(0);
	}
	die2("byte size too big", argv[1]);
}


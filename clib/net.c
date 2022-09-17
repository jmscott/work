/*
 *  Convert 32 bit internet address to dotted text, rotating through
 *  static buffer pool.
 *
 *  Note:
 *	Need to replace static buf rotator code with hypothetical
 *	jmscott_bufstatcat() family!
 *
 *	Derived from code written by https://github.com/akfullfo.
 */
#include "jmscott/libjmscott.h"

char *
jmscott_net_32addr2text(u_long addr)
{
#define SHOW_IP_BUFS	4
#define SHOW_IP_BUFSIZE	20

	char *cp, *buf;
	u_int byte;
	int n;
	static char bufs[SHOW_IP_BUFS][SHOW_IP_BUFSIZE];
	static int curbuf = 0;

	buf = bufs[curbuf++];
	if (curbuf >= SHOW_IP_BUFS)
		curbuf = 0;

	cp = buf + SHOW_IP_BUFSIZE;
	*--cp = '\0';

	n = 4;
	do {
		byte = addr & 0xff;
		*--cp = byte % 10 + '0';
		byte /= 10;
		if (byte > 0) {
			*--cp = byte % 10 + '0';
			byte /= 10;
			if (byte > 0)
				*--cp = byte + '0';
		}
		*--cp = '.';
		addr >>= 8;
	} while (--n > 0);

	cp++;
	return cp;
}

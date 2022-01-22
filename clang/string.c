/*
 *  Synopsis:
 *	Various utf8 common string manipulation functions.
 */

#ifndef JMSCOTT_CLANG_STRING
#define JMSCOTT_CLANG_STRING

#include <ctype.h>
#include <stdint.h>

/*
 * Synopsis:
 *      Fast, safe and simple string concatenator
 *  Usage:
 *	cc -I/usr/local
 *
 *	#include "jmscott/lib/clang/strcat.c"
 *
 *      buf[0] = 0
 *      jmscott_strcat(buf, sizeof buf, "hello, world");
 *      jmscott_strcat(buf, sizeof buf, ": ");
 *      jmscott_strcat(buf, sizeof buf, "good bye, cruel world");
 *  Note:
 *	Need to rename string.c or utf8.c, since UTF-8 code will appear soon.
 */

void
jmscott_strcat(char *restrict tgt, int tgtsize, const char *restrict src)
{
        //  find null terminated end of target buffer
        while (*tgt++)
                --tgtsize;
        --tgt;

        //  copy non-null src bytes, leaving room for trailing null
        while (--tgtsize > 0 && *src)
                *tgt++ = *src++;

        // target always null terminated
        *tgt = 0;
}

/*
 *  Convert unsigned long long to decimal ascii string.
 *  Return the pointer to byte after final digit:
 */
char *
jmscott_ulltoa(unsigned long long ull, char *digits)
{
	char const digit[] = "0123456789";
	char* p, *end_p;
	unsigned long long power = ull;

	//  find the end of the formated ascii string

	p = digits;
	do
	{
		++p;
		power = power / 10;

	} while (power);
	end_p = p;

	//  in reverse order, replace byte with the ascii chars

	do
	{
		*--p = digit[ull % 10];
		ull = ull / 10;

	} while (ull);
	return end_p;
}

/*
 *  Strictly convert an ascii string to an unsigned 63bit
 *
 *	0 <= 9223372036854775808
 *
 *  returning a text description of why string can not be parsed.
 */
char *
jmscott_a2ui63(char *a, unsigned long long *ull)
{

	if (*a == 0)
		return "value has no chars";;

	unsigned long long u = 0;
	char *p = a, c;
	while ((c = *p++)) {
		if (p - a >= 20)
			return "value is >= 20 chars";
		if (!isdigit(c))
			return "value has non digit";
		u = u * 10 + (c - '0');
	}
	if (ull)
		*ull = u;
	return (char *)0;
}

char *
jmscott_a2size_t(char *a, size_t *sz)
{
	char *err;
	unsigned long long ull;

	if ((err = jmscott_a2ui63(a, &ull)))
		return err;
	if (ull > SIZE_MAX)
		return "value > maximum of size_t";
	if (sz)
		*sz = (size_t)ull;
	return (char *)0;
}

#endif	// define JMSCOTT_CLANG_STRING


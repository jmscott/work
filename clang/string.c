/*
 *  Synopsis:
 *	Various utf8 common string manipulation functions.
 */

#ifndef JMSCOTT_CLANG_STRING
#define JMSCOTT_CLANG_STRING

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

#endif	// define JMSCOTT_CLANG_STRING

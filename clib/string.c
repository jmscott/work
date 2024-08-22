/*
 *  Synopsis:
 *	Various utf8 C string manipulation functions, safe in signal handlers.
 *  Note:
 *	The BSD clib function strlcat() comes close to the behaviour of the
 *	jmscott_strcat*() functions.  However, strlcat() is not on linux.  
 *	
 *	Should jmscott_strcat*() funcs return char * to terminating \0?
 *
 *	Need version of *_strcatN() that separates args with ": ",
 *	maybe named jmscott_coloncatN().
 *
 *	indicate truncation as return value for functions jmscott_strcatN()
 *	in this way, apps can implement their own functions _strcatN(),
 *	that die upon truncation.
 */

#include <ctype.h>
#include <stdint.h>
#include <stddef.h>

#include "jmscott/libjmscott.h"

/*
 *  Composable string concat returning the char * terminating null.
 *
 *  Usage:
 *	char tgt[1024];
 *
 *	//  strcpy
 *	tgt[0] = 0;
 *	jmscott_strcat(tgt, sizeof tgt, "hello, world");
 *
 *	...
 *
 *	//  strcat
 *	jmscott_strcat(tgt, sizeof tgt, ": good bye, cruel world");
 *  Note:
 *	tgtsize can overflow.  why not add "--tgtsize == 0" in while loop?
 */
char *
jmscott_strcat(char *tgt, int tgtsize, const char *src)
{
        //  find null terminated end of target buffer
        while (*tgt++)
                --tgtsize;	//  overflow ?!
	--tgt;
	if (src == (char *)0)
		return tgt;

        //  copy non-null src bytes, leaving room for trailing null
        while (--tgtsize > 0 && *src)
                *tgt++ = *src++;

        *tgt = 0;
	return tgt;
}

char *
jmscott_strcat2(char *tgt, int tgtsize, const char *src1, const char *src2)
{
	char *p = jmscott_strcat(tgt, tgtsize, src1);
	return jmscott_strcat(p, tgtsize - (p - tgt), src2);
}

char *
jmscott_strcat3(
	char *tgt,
	int tgtsize,
	const char *src1,
	const char *src2,
	const char *src3
){
	char *p = jmscott_strcat(tgt, tgtsize, src1);
	return jmscott_strcat2(p, tgtsize - (p - tgt), src2, src3);
}

char *
jmscott_strcat4(
	char *tgt,
	int tgtsize,
	const char *src1,
	const char *src2,
	const char *src3,
	const char *src4
){
	char *p = jmscott_strcat(tgt, tgtsize, src1);
	return jmscott_strcat3(p, tgtsize - (p - tgt), src2, src3, src4);
}

char *
jmscott_strcat5(
	char *tgt,
	int tgtsize,
	const char *src1,
	const char *src2,
	const char *src3,
	const char *src4,
	const char *src5
){
	char *p = jmscott_strcat(tgt, tgtsize, src1);
	return jmscott_strcat4(p, tgtsize - (p - tgt), src2, src3, src4, src5);
}

char *
jmscott_strcat6(
	char *tgt,
	int tgtsize,
	const char *src1,
	const char *src2,
	const char *src3,
	const char *src4,
	const char *src5,
	const char *src6
){
	char *p = jmscott_strcat(tgt, tgtsize, src1);
	return jmscott_strcat5(
				p,
				tgtsize - (p - tgt),
				src2,
				src3,
				src4,
				src5,
				src6
	);
}

char *
jmscott_strcat7(
	char *tgt,
	int tgtsize,
	const char *src1,
	const char *src2,
	const char *src3,
	const char *src4,
	const char *src5,
	const char *src6,
	const char *src7
){
	char *p = jmscott_strcat(tgt, tgtsize, src1);
	return jmscott_strcat6(
				p,
				tgtsize - (p - tgt),
				src2,
				src3,
				src4,
				src5,
				src6,
				src7
	);
}

/*
 *  Convert unsigned long long to decimal ascii string.
 *  Return the pointer to byte after final digit:
 *
 *  !!  The caller must be sure to null terminate the return end string:
 *
 *	*jmscott_ulltoa(...) = 0;
 *
 *  !!  Not terminating can cause typical, wierd clang behavior!
 */
char *
jmscott_ulltoa(unsigned long long ull, char *tgt)
{
	char const digit[] = "0123456789";
	char* p, *end_p;
	unsigned long long power = ull;

	//  find the end of the formated ascii string

	p = tgt;
	do
	{
		++p;
		power = power / 10;

	} while (power > 0);
	end_p = p;

	//  in reverse order, replace byte with the ascii chars

	do
	{
		*--p = digit[ull % 10];
		ull = ull / 10;

	} while (ull);
	return end_p;
}

char *
jmscott_lltoa(long long ll, char *tgt)
{
	if (ll < 0) {
		*tgt++ = '-';
		return jmscott_ulltoa((unsigned long long)-ll, tgt);
	}
	return jmscott_ulltoa((unsigned long long)ll, tgt);
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

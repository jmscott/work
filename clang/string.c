/*
 *  Synopsis:
 *	Various utf8 common string parsing functions.
 */

#ifndef JMSCOTT_CLANG_STRING
#define JMSCOTT_CLANG_STRING

/*
 *  Convert unsigned long long to decimal ascii string.
 *  Return the pointer to byte after final digit:
 */
static char *
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

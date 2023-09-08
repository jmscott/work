/*
 *  Synopsis:
 *	Uniform digest routines.
 *  Note:
 *	https://github.com/jmscott/blobio
 */
#include <stdio.h>
#include <ctype.h>

#include "jmscott/libjmscott.h"

/*
 *  Synopsis:
 *	Frisk a udig matching: [a-z][a-z0-9]{0,7}:([:graph:]&&[:ascii:]){32,128}
 *  Returns:
 *	- null string if udig is syntactically correct.
 *	- a static string describing the error.
 *  See:
 *	https://github.com/jmscott/blobio
 */
char *
jmscott_frisk_udig(char *udig)
{
	if (!udig)
		return "udig is null";
	if (!*udig)
		return "udig is zero length";

	//  scan the algorithm matching [a-z][a-z]{0,7}:

	char *u = udig, c;

	c = *u++;
	if (!isalpha(c) || !islower(c))
		return "first char of algorithm not in [a-z]";

	while ((c = *u++)) {
		if (c == ':')
			break;

		if (u - udig > 8)
			return "algo > 8 chars";
		if (!isascii(c))
			return "char in algo is not ascii";
		if (isalpha(c)) {
			if (!islower(c))
				return "alpha char in algo is not lower";
		} else if (!isdigit(c))
			return "char in algo not in [a-z0-9]";
	}
	if (c != ':')
		return "algorithm not terminated with a colon";

	//  scan the digest matching [[:graph:]]{32,128}

	while ((c = *u++)) {
		if (u - udig > 128)
			return "digest > 128 character";
		if (!isascii(c))
			return "character in digest is not ascii";
		if (!isgraph(c))
			return "character in digest is not graphable";
	}
	if (u - udig < 32)
		return "digest < 32 chars";
	return (char *)0;
}

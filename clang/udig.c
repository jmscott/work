/*
 *  Synopsis:
 *	Uniform digest routines.
 *  Usage:
 *	#include "jmscott/lib/clang/die.c"
 *
 *	static void
 *	die(char msg) {
 *		jmscott_die(127, msg);
 *	}
 *  Note:
 *	https://github.com/jmscott/blobio
 */
#include <stdio.h>

#ifndef JMSCOTT_CLANG_UDIG
#define JMSCOTT_CLANG_UDIG

#include <ctype.h>

char *
jmscott_frisk_udig(char *udig)
{
	if (!udig)
		return "udig is null";

	//  scan the algorithm matching [a-z][a-z]{0,7}:

	char *u = udig, c;
	while ((c = *u++)) {
		if (c == ':')
			break;

		if (u - udig > 8)
			return "algo > 8 chars";
		if (!isascii(c))
			return "char in algo is not ascii";
		if (isalpha(c)) {
			if (!islower(c))
				return "char in algo is not lower";
		} else if (!isdigit(c))
			return "char in algo not a digit";

		//  first char of algo must match [a-z]

		if (u - udig == 1 && (c < 'a' || c > 'z'))
			return "first char of algorithm not in [a-z]";
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
	return (char *)0;
}

#endif 	// define JMSCOTT_CLANG_UDIG

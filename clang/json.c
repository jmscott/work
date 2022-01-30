/*
 *  Synopsis:
 *	Manipulate json objects.
 *  Note:
 *	Investigate clang scopeing rules for macros _PUT and _ESCAPE
 *	defined in function jmscott_ascii2json().
 */
#ifndef JMSCOTT_CLANG_JSON
#define JMSCOTT_CLANG_JSON

#include <ctype.h>

/*
 *  Convert an ascii string to escaped json "string".
 *  The surrounding double quotes are included in target json string.
 */
char *
jmscott_ascii2json_string(char *src, char *tgt, int tgt_size)
{

//  investigate scoping rules for c preprocessor.
#define _PUT(c)	{							\
	if (j - tgt >= tgt_size)					\
		return "source string too large for target buffer";	\
	*j++ = c;							\
}

#define _ESCAPE(c)	{						\
	_PUT('\\');							\
	_PUT(c);							\
}
	if (tgt_size < 3)
		return "tgt size < 3";

	char *s = src;
	char *j = tgt;

	*j++ = '"';

	char c;
	while ((c = *s++)) {
		if (!isascii(c))
			return "non ascii char in source string";

		//  Note: does case order match ascii order?
		switch (c) {
		case '"':
			_ESCAPE('"');
			break;
		case '\\':
			_ESCAPE('\\');
			break;
		case '\b':
			_ESCAPE('b');
			break;
		case '\f':
			_ESCAPE('f');
			break;
		case '\n':
			_ESCAPE('n');
			break;
		case '\r':
			_ESCAPE('r');
			break;
		case '\t':
			_ESCAPE('t');
			break;
		default:
			_PUT(c);
		}
	}
	_PUT('"');
	_PUT('\0');
	return (char *)0;
}

char *
jmscott_json_write(int fd, char *format)
{
	char *f, c;

	(void)fd;

	f = format;
	if (!f)
		return "null format";
step:
	while ((c = *f++) && isspace(c))
		;
	if (!c)
		return (char *)0;
	switch (c) {

	//  # comment
	case '#':
		while ((c = *f++) && c != '\n')
			;
		if (!c)
			return (char *)0;
		break;
	default:
		return "unknown format char";
	}
	goto step;
}

#endif

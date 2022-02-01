/*
 *  Synopsis:
 *	Manipulate json objects.
 *  Note:
 *	Investigate scoping rules for cpp macros, such as those defined in
 *	function jmscott_ascii2json().
 */
#ifndef JMSCOTT_CLANG_JSON
#define JMSCOTT_CLANG_JSON

#include <ctype.h>
#include <string.h>
#include <stdarg.h>

#include "jmscott/posio.c"

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

/*
 *  Synopsis:
 *	Write json structures described in a format string.
 *  Description:
 *	Write a json structure on a file, following a format string and
 *	variable arguments list.
 *
 *		jmscott_json_write("s", "hello, world");
 */
char *
jmscott_json_write(int fd, char *format, ...)
{
#define RETURN(e) {va_end(argv);  return(e);}

#define WRITE(s)							\
	{if (jmscott_write(1, s, strlen(s)))				\
		RETURN(strerror(errno));}
#define INDENT(level)							\
	for (int i = 0;  i < level;  i++)				\
		WRITE("\t");

	char *f, c;
	va_list argv;
	(void)fd;
	char *err;
	int indent = 1;

	va_start(argv, format);

	f = format;
	if (!f)
		RETURN("null format");

step:
	//  skip white space in format command
	while ((c = *f++) && isspace(c))
		;
	if (!c)
		RETURN((char *)0);
	switch (c) {

	//  # comment
	case '#':
		while ((c = *f++) && c != '\n')
			;
		if (!c)
			RETURN((char *)0);
		break;
	
	//  json string
	case 's': {
		char json[1024 * 64];

		char *a = va_arg(argv, char *);
		if ((err = jmscott_ascii2json_string(a, json, sizeof json)))
			RETURN(err);
		WRITE(json);
		break;
	}
	default:
		if (jmscott_write(fd, &c, 1))
			RETURN(strerror(errno));
	}
	goto step;
}

#endif

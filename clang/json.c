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
#include <stdlib.h>

#include "jmscott/posio.c"

struct jmscott_json
{
	int		out_fd;
	int		indent;

	int		trace;
};

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

struct jmscott_json *
jmscott_json_new()
{
	struct jmscott_json *jp = malloc(sizeof (struct jmscott_json));
	if (!jp)
		return (struct jmscott_json *)0;
	jp->out_fd = 1;
	jp->indent = 0;
	jp->trace = 0;
	return jp;
}

void
jmscott_json_trace(struct jmscott_json *jp, char *what, char *value)
{
	if (!jp->trace)
		return;
	write(2, "TRACE: ", 7);
	write(2, what, strlen(what));
	write(2, ": ", 2);
	write(2, value, strlen(value));
	write(2, "\n", 1);
}

void
jmscott_json_trace_c(struct jmscott_json *jp, char c)
{
	if (c) {
		char buf[2];

		buf[0] = c;
		buf[1] = 0;
		jmscott_json_trace(jp, "c", buf);
	} else
		jmscott_json_trace(jp, "c", "null");
}

/*
 *  Synopsis:
 *	Write json structures described in a format string.
 *  Description:
 *	Write a json structure on a file, following a format string and
 *	variable arguments list.
 *
 *		jmscott_json_write("s", "hello, world");
 *
 *	Important to note that correctness of json syntax is NOT insured!
 */
char *
jmscott_json_write(struct jmscott_json *jp, char *format, ...)
{

	char *f, c;
	va_list argv;
	char *err;
	int b;
	char js[1024 * 64], *cp;


	jmscott_json_trace(jp, "format", format);
/*
 *  Note: poor man's closure
 */
#define RETURN(e) {va_end(argv);  return(e);}

#define WRITE(s)							\
	{								\
		if (jmscott_write(jp->out_fd, s, strlen(s)))		\
			RETURN(strerror(errno));			\
	}

#define INDENT								\
	{								\
		for (int i = 0;  i < jp->indent;  i++)			\
			WRITE("\t");					\
	}

	va_start(argv, format);

	f = format;
	if (!f)
		RETURN("null format");
step:
	//  skip white space in format command
	while ((c = *f++) && isspace(c))
		;
	jmscott_json_trace_c(jp, c);
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
	case 'b':
		b = va_arg(argv, int);
		if (b)
			WRITE("true")
		else
			WRITE("false")
		break;
	case 'k':
		WRITE("\n");
		INDENT;
		//  fallthrough to case 's':
	
	//  json string
	case 's':
		cp = va_arg(argv, char *);

		jmscott_json_trace(jp, "s", cp);
		if ((err = jmscott_ascii2json_string(cp, js, sizeof js)))
			RETURN(err);
		jmscott_json_trace(jp, "s+", js);
		WRITE(js);
		break;
	case '{': 
		INDENT;
		WRITE("{\n");
		jp->indent++;
		break;
	case '[':
		INDENT;
		WRITE("[\n");
		jp->indent++;
		break;
	case '}':
		--jp->indent;
		INDENT;
		WRITE("}\n");
		break;
	case ']':
		--jp->indent;
		INDENT;
		WRITE("]\n");
		break;
	default:
		if (jmscott_write(jp->out_fd, &c, 1))
			RETURN(strerror(errno));
	}
	goto step;
}

#endif

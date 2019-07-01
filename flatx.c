/*
 *  Synopsis:
 *   	Pretty Print a Structured Vector Graphics stream.
 *  Usage:
 *	svg_pp <compass.svg >compass_pp.svg
 *  Description:
 * 	Format an xml svg document thusly:
 * 	suitable for easy parsing by simple scripts and database loaders.
 * 	Also, text that descends from a single node can be extracted.
 * 	Mutiple text nodes can not be extracted.
 *
 * 	The definition of a well formed document is what libexpat can parse.
 * 	Errors encounter by libexpat cause flatx to abort with an error.
 * 	The entire xml document is always read (but not buffered).
 *
 * 	The default output flattens xml input thusly:
 *
 * 		[byte-index][tab]/root/child1/child2/../childk
 * 		...
 * 		[byte-index][tab]/root/child1/child2/../childn
 *
 * 	The row and field separators are new-line and tab, but can be changed
 * 	to any arbitrary string.
 *
 * 	The XML document is always parsed to completion.  A malformed document
 * 	is always an error.
 *
 *  	Element order is an unsigned, 64bit value with a minimum of 1.
 *
 *  	Using --char-byte-index with other options is an error.
 *
 *  Usage:
 *   	flatx --prefix-fields 12345 --path-separator . <test.xml
 *   	flatx --char-byte-index 34345 <test.xml >34345.txt.
 *
 *  Options:
 *  	--prefix-fields string		Write prefix string before each row
 *
 *  	--order [byte,line,column,element]
 *  				Write order(s) as byte index, line number,
 *  				column offset, document element order.
 *  	--path-separator	Path separator char between elements,
 *  					Default string is '/'.
 *	--char-byte-index	Extract all char data for descendents of the
 *					element at this byte index.
 *  Requires:
 *  	libexpat, no particular version.
 *  See Also:
 * 	http://www.jclark.com/xml/expat.html
 *  Exit:
 *  	0	success, well formed xml.
 *  	1	xml parse error, usually means xml not well formed.
 *  	2	unexpected command line argument
 *  	3	out of memory
 *  	4	can't create XML Parser
 *  	5	input/output Error
 *  	6	signed 64 bit integer overflow
 *  	7	xml node not found
 * 	255	system error
 *  Blame:
 *  	jmscott@setspace.com
 *  Note:
 *	Does not compile in strict c99.
 *
 *  	I (jmscott) have not investigated how well the clib strlib functions
 *  	work with utf8 encoding.
 *
 *  	Entities and namespace are not handled, another seriously wrong
 *	ommision.
 *
 *	This comment needs to be merged into the --help output.
 */
#define FLATX_VERSION	"0.20120907.03"

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "expat.h"

/*
 *  Initial size of element stack.  Doubles when grown, until out of memory.
 */
#define INIT_ELEMENT_STACK_SIZE	64

#define MAX_MSG_SIZE		1024
#define MAX_INT64		0x7FFFFFFFFFFFFFFF

/*
 *  Quiet compiler warnings.
 */
#define HUSH_UNUSED_ARGS1(a)		if (!a) a = 0;  if (!a) a = 0;
#define HUSH_UNUSED_ARGS2(a1, a2)	if (!a1) a1 = 0;  if (!a2) a2 = 0;

/*
 *   Error exit codes returned to caller
 */
#define EXIT_XML_PARSE		1
#define EXIT_CLIARG		2
#define EXIT_OOM		3
#define EXIT_PARSECREATE	4
#define EXIT_IO			5
#define EXIT_I64OFLOW		6
#define EXIT_NO_ELEMENT		7

#define EXIT_PANIC		255

static char	progname[] = "flatx";

static char *err_english[] =
{
	"success",
	"xml parse error",
	"command line error",
	"out of memory",
	"XML_ParserCreate() failed",
	"i/o error",
	"overflow of signed 64 bit int",
	"no xml element"
};
#define EXIT_ENGLISH_MAX		EXIT_NO_ELEMENT

static char usage[] =
	"usage: flatx [--help] [--version] [--option ...]\n";
static char _help[] = "\
Synopsis:\n\
	Flatten an xml tree for easy parsing in typical unix scripts.\n\
Description:\n\
	Flatten the twigs or indexed char nodes of an xml document for\n\
	easy parsing by typical unix scripts, database loaders or\n\
	normalizers.  Output is written to standard output.\n\
\n\
	For twig output, unique \"row\" indexes may be given: document byte\n\
	order, line/column or element count.  Both the \"row\" and field\n\
	character string can be overridden from the defaults of ascii\n\
	new-line and tab.\n\
\n\
	For text output, all the descendent char nodes of the element node at\n\
	the gvien offset/index are written.  The element node is not written.\n\
\n\
	A prefix string may be emitted before each \"row\".  Typically the\n\
	prefix is a document id plus the field separator.\n\
\n\
	flatx emphasizes speed and memory footprint over functionality and\n\
	will never be a replacement for xpath queries.  Behaviour of the xml\n\
	parser will always be as expected by libexpat.\n\
\n\
	A partially parsed malformed document could generate output, but the\n\
	exit status of flatx will indicate the error.\n\
\n\
	All index/offsets start at 0 and end at 2^63 - 1.\n\
Exit Codes:\n\
	0	no errors\n\
	1	xml parsing error, usually not well formed document\n\
	2	bad command line argument\n\
	3	out of memory\n\
	4	libexpat XML_ParserCreate() Error\n\
	5	Read/Write error\n\
	6	Overflow of signed 64 bit int\n\
Note:\n\
	Only extracting the next char node is a serious bug. SAX parsers\n\
	may split char nodes into mutiple nodes.  I (jmscott@setspace.com)\n\
	will fix this.\n\
\n\
	Eventually attributes, in sorted, unique order should be extracted.\n\
\n\
	Much more rumination on the charset and xml version must be done.\n\
Options:\n\
	--order [byte,line,column,element]\n\
		Write order unique to each row.\n\
		The default order is byte index.\n\
	--prefix-fields '<string>'\n\
		Write fields in string before each row. A field separator as \n\
		the last character is not expected.\n\
	--path-separator <string>\n\
		The string used to separate components of the element path.\n\
		The default is the forward slash character /\n\
	--char-byte-index <byte-index>\n\
		Write the descendent char node(s) of the given element.\n\
Blame:\n\
	jmscott@setspace.com\n\
Usage:\n\
	flatx <hamlet.xml\n\
	flatx --char-byte-index 27 <hamlet.xml\n\
	flatx --help\n\
";

/*
 *  XML <element> stack item.
 *
 *  Once an item is allocated on the stack, the memory is not freed, only grown,
 *  allowing for reuse.  The alternative is to alloc/free items on each push 
 *  and pull, which seems wastefull, since typically trees are not too deep nor
 *  element names not too long.
 */
struct element
{
	size_t		alloc_length;
	unsigned	length;		
	XML_Char	*name;
	unsigned char	in_char_twig;
};

static struct element *elements;
static struct element *element_next =	0;
static struct element *element_limit =	0;

#define ORDER_BYTE	1
#define ORDER_LINE	2
#define ORDER_COLUMN	3
#define ORDER_ELEMENT	4

static int	orders[5] =	{0};
static unsigned long long element_order = 0;	/* api range is signed 64 bit */

/*
 *  Char extraction starting from a particular element.
 */
#define CHAR_STATE_OFF		0
#define CHAR_STATE_SEARCH	1
#define CHAR_STATE_EXTRACTING	2
#define CHAR_STATE_DONE		3

static XML_Index char_byte_index;
static int char_state = CHAR_STATE_OFF;

/*
 *  Path component separator chars.
 */
#define PATH_SEPARATOR_DEFAULT		"/"
static char	*path_sep =		0;
static size_t	path_sep_length =	0;

/*
 *  Field separator chars.
 */
#define FIELD_SEPARATOR_DEFAULT		"\t"
static char	*field_sep =		0;
static size_t	field_sep_length =	0;

/*
 *  Row separator chars.
 */
#define ROW_SEPARATOR_DEFAULT		"\n"
static char	*row_sep =		0;
static size_t	row_sep_length =	0;

static char	*prefix_fields;
static unsigned int prefix_fields_length =	0;

static char colon[] =			": ";

static XML_Parser parser;

static void
help()
{
	write(1, _help, sizeof _help - 1);
	exit(0);
}

static void
die(int status, char *msg)
{
	static char ERROR[] = "ERROR: ";
	static char PANIC[] = "PANIC: ";
	static char null[] = "PANIC: die(null message)";
	static char nl[] = "\n";

	write(2, progname, sizeof progname - 1);
	write(2, colon, sizeof colon - 1);

	/*
	 *  You can bet your sweet grep that all error messages start with
	 *
	 * 	ERROR: ...
	 */
	write(2, ERROR, sizeof ERROR - 1);

	/*
	 *  Write english description of message.
	 *
	 *  Insure error status code is in range.  PANIC otherwise.
	 */
	if (status == EXIT_PANIC)
		write(2, PANIC, sizeof PANIC - 1);
	else if (status < 1 || status > EXIT_ENGLISH_MAX) {
		static char badstatus[]="PANIC: error status code out of range";

		write(2, badstatus, sizeof badstatus - 1);
		write(2, nl, sizeof nl - 1);
		exit(255);
	} else
		write(2, err_english[status], strlen(err_english[status]));

	/*
	 *  Write extra info related to error.
	 */
	write(2, colon, sizeof colon - 1);
	if (msg)
		write(2, msg, strlen(msg));
	else
		write(2, null, sizeof null);
	write(2, nl, sizeof nl);
	write(2, usage, sizeof usage - 1);
	/*
	 *   Bye
	 */
	exit(status);
}

static void
die2(int status, char *msg1, char *msg2)
{
	char buf[MAX_MSG_SIZE + 1];
	buf[0] = buf[MAX_MSG_SIZE] = 0;

	if (msg1)
		strncpy(buf, msg1, MAX_MSG_SIZE);
	if (msg2) {
		if (buf[0]) {
			strncat(buf, colon, MAX_MSG_SIZE - strlen(buf));
			strncat(buf, msg2, MAX_MSG_SIZE - strlen(buf));
		}
		else
			strncpy(buf, msg2, MAX_MSG_SIZE);
	}
	die(status, buf);
}

static void
die3(int status, char *msg1, char *msg2, char *msg3)
{
	char buf[MAX_MSG_SIZE + 1];
	buf[0] = buf[MAX_MSG_SIZE] = 0;

	if (msg1)
		strncpy(buf, msg1, MAX_MSG_SIZE);
	if (msg2) {
		if (buf[0]) {
			strncat(buf, colon, MAX_MSG_SIZE - strlen(buf));
			strncat(buf, msg2, MAX_MSG_SIZE - strlen(buf));
		}
		else
			strncpy(buf, msg2, MAX_MSG_SIZE);
	}
	die2(status, buf, msg3);
}

static void
do_write(char *buf, int length)
{
	int nwrite = 0;

	if (length < 1)
		return;
	while (nwrite < length) {
		int status = write(
				1,
				(unsigned char *)buf + nwrite,
				length - nwrite
			     );
		if (status < 0) {
			if (errno == EINTR)
				continue;
			die2(EXIT_IO, "do_write: write(1) failed",
							strerror(errno));
		}
		nwrite += status;
	}
}

static ssize_t
do_read(char *buf, int buf_size)
{
	int status;
again:
	status = read(0, buf, buf_size);
	if (status < 0) {
		if (errno == EINTR)
			goto again;
		die2(EXIT_IO, "do_read: read(0) failed", strerror(errno));
		/*NOTREACHED*/
	}
	return status;
}

struct element *
push_element(const XML_Char *name)
{
	struct element *e;

	/*
	 *   Grow the element stack before pushing this element?
	 */
	if (element_next >= element_limit) {
		size_t s = element_limit - elements;
		size_t limit;
		size_t i;

		limit = s * 2;

		/*
		 *  Reallocate the element stack.
		 *
		 *  Make sure we don't wrap when growing stack.
		 *  We assume c wraps size_t arithmetic.
		 */
		if (s > limit)
			die(EXIT_OOM, "realloc(element stack): size_t wraps");
		elements = realloc(elements, limit * sizeof *elements);
		if (elements == NULL)
			die(EXIT_OOM, "realloc(elements) failed");

		/*
		 *  Zero out the newly allocted, upper portion of the stack.
		 */
		for (i = s;  i < limit;  i++) {
			elements[i].name = 0;
			elements[i].length = elements[i].alloc_length = 0;
		}
		element_limit = &elements[limit];
		element_next = &elements[s];
	}
	e = element_next++;

	/*
	 *  Record the name length for faster writes.
	 */
	e->length = strlen(name);

	/*
	 *  Possibly grow the name element on the stack.
	 */
	if (e->length > e->alloc_length) {
		e->name = realloc(e->name, e->length * sizeof *e->name);
		if (e->name == NULL)
			die(EXIT_OOM, "realloc(element name) failed");
		e->alloc_length = e->length;
	}

	/*
	 *  Copy the name onto the element at top of stack.
	 */
	memcpy(e->name, name, e->length);

	return e;
}

static void
char_start_element(void *user_data, const XML_Char *name, const XML_Char **atts)
{
	struct element *e;

	HUSH_UNUSED_ARGS2(user_data, atts);

	e = push_element(name);
	if (char_state == CHAR_STATE_SEARCH)
		if (char_byte_index == XML_GetCurrentByteIndex(parser)) {
			e->in_char_twig = 1;
			char_state = CHAR_STATE_EXTRACTING;
		}
}

static void
start_element(void *user_data, const XML_Char *name, const XML_Char **atts)
{
	char buf[MAX_MSG_SIZE];
	struct element *e;

	HUSH_UNUSED_ARGS2(user_data, atts);

	push_element(name);

	/*
	 *  Write prefix supplied by the caller.
	 *  Notice that no separator char is written.
	 */
	if (prefix_fields)
		do_write(prefix_fields, prefix_fields_length);

	/*
	 *  Write the requested document order.
	 */
	if (orders[0]) {
		int *o;

		o = orders;
		while (*o) {
			if (o - 1 > orders || prefix_fields)
				do_write(field_sep, field_sep_length);
			switch (*o++) {
			/*
			 *  Write the field separator if we've written
			 *  more than one order field or have written
			 *  an existing 
			 */
			case ORDER_BYTE:
				snprintf(buf, sizeof buf, "%llu",
					(unsigned long long)
					XML_GetCurrentByteIndex(parser));
				do_write(buf, strlen(buf));
				break;
			case ORDER_LINE:
				snprintf(buf, sizeof buf, "%llu",
					(unsigned long long)
					XML_GetCurrentLineNumber(parser));
				do_write(buf, strlen(buf));
				break;
			case ORDER_COLUMN:
				snprintf(buf, sizeof buf, "%llu",
					(unsigned long long)
					XML_GetCurrentColumnNumber(parser));
				do_write(buf, strlen(buf));
				break;
			case ORDER_ELEMENT:
				/*
				 *  Expat library only handles up to 
				 *  signed 64 bit, I (jmscott) believe.
				 */
				if (element_order == MAX_INT64)
					die(EXIT_I64OFLOW,
						"element order too big");
				snprintf(buf, sizeof buf, "%llu",element_order);
				do_write(buf, strlen(buf));
				element_order++;
				break;
			default:
				die(EXIT_PANIC, "impossible order index");
			}
		}
	}

	/*
	 *  Write the branch of the tree as a list of elements separated
	 *  by the path_sep characters.
	 */
	if (orders[0] || prefix_fields)
		do_write(field_sep, field_sep_length);
	for (e = elements;  e < element_next;  e++) {
		do_write(path_sep, path_sep_length);
		do_write(e->name, e->length);
	}

	/*
	 *   Write the row separator.
	 */
	do_write(row_sep, row_sep_length);
}

static void 
end_element(void *user_data, const XML_Char *name)
{
	HUSH_UNUSED_ARGS2(user_data, name);

	if (--element_next < elements)
		die(EXIT_PANIC, "end_element: element stack underflow");
}

static void 
char_end_element(void *user_data, const XML_Char *name)
{
	struct element *e;

	HUSH_UNUSED_ARGS2(user_data, name);

	if (--element_next < elements)
		die(EXIT_PANIC, "end_element: element stack underflow");

	/*
	 *  Are we closing a starting char element?
	 *  Can't the character_data() callback simply be removed?
	 */
	e = element_next;
	if (e->in_char_twig) {
		e->in_char_twig = 0;
		char_state = CHAR_STATE_DONE;
	}
}

static void
character_data(void *user_data, const XML_Char *data, int len)
{
	HUSH_UNUSED_ARGS1(user_data);

	/*
	 *  Only write while inside requested twig.
	 *  Why can' this callback simply be uninstalled when we leave the twig?
	 */
	if (char_state == CHAR_STATE_EXTRACTING)
		/*
		 *  XML_Char may greater than 1 one byte.  I (jmscott) am not
		 *  sure we can simply cast to char *  !!!
		 */
		do_write((char *)data, sizeof *data * len);
}

static int
scan_order(char *word)
{
	if (strcmp("byte", word) == 0)
		return ORDER_BYTE;
	if (strcmp("line", word) == 0)
		return ORDER_LINE;
	if (strcmp("column", word) == 0)
		return ORDER_COLUMN;
	if (strcmp("element", word) == 0)
		return ORDER_ELEMENT;
	die2(EXIT_CLIARG, "option --order: unknown order", word);
	/*NOTREACHED*/
	return 0;
}

int
main(int argc, char **argv)
{
	char buf[BUFSIZ];
	int i;
	int done;

	/*
	 *  Process command line arguments.
	 */
	for (i = 1;  i < argc;  i++) {
		char *a = argv[i];

		/*
		 *  We see either a file name or the "-" argument
		 *  followed by a file name.
		 */
		if (strcmp("--prefix-fields", a) == 0) {
			char *p;

			if (++i == argc)
				die(EXIT_CLIARG,
					"option --prefix-fields: missing id");
			p = argv[i];
			if (!p[0])
				die(EXIT_CLIARG,
					"option --prefix-fields: empty id");

			prefix_fields = strdup(p);
			if (prefix_fields == NULL)
				die(EXIT_OOM, "strdup(prefix-fields) failed");
			prefix_fields_length = strlen(prefix_fields);
		} else if (strcmp("--path-separator", a) == 0) {
			char *p;

			if (++i == argc)
				die(EXIT_CLIARG,
				     "option --path-separator: missing string");
			p = argv[i];
			if (!p[0])
				die(EXIT_CLIARG,
				       "option --path-separator: empty string");

			path_sep = strdup(p);
			if (path_sep == NULL)
				die(EXIT_OOM,"strdup(path separator) failed");
			path_sep_length = strlen(path_sep);
		} else if (strcmp("--order", a) == 0) {
			int j;
			char *p, *o;
			char a_order[25];

			if (++i == argc)
				die(EXIT_CLIARG,
				     "option --order: missing order list");
			p = argv[i];
			if (!p[0])
				die(EXIT_CLIARG,
					"option --order: empty order list");
			if (strlen(p) >= sizeof a_order / sizeof *a_order)
				/*
				 *  Note:
				 *  	Error message needs max list size!
				 */
				die(EXIT_CLIARG,
					"option --order: list too long");

			/*
			 *  Build the list of orders.  Looking for lists like
			 *
			 *  	byte,element
			 *  	line,char
			 */
			strcpy(a_order, p);
			p = a_order;
			for (j = 0;  j < 4;  j++) {
				if (!p[0])
					die2(EXIT_CLIARG,
					"option --order: unexpected order list",
							a_order);
				o = strchr(p, ',');
				if (!o)
					break;
				*o = 0;
				orders[j] = scan_order(p);
				p = o + 1;
			}
			orders[j] = scan_order(p);
		} else if (strcmp(a, "--char-byte-index") == 0) {
			static char o[] = "option --char-byte-index";
			char *p;

			if (++i == argc)
				die2(EXIT_CLIARG, o, "missing byte index");
			p = argv[i];
			if (!p[0])
				die2(EXIT_CLIARG, o, "empty byte index");

			/*
			 *  Unfortunatly we can pass a byte index > 2^64.
			 */
			if (sscanf(p, "%llu", &char_byte_index) != 1)
				die3(EXIT_CLIARG, o, "can't scan index", p);
			char_state = CHAR_STATE_SEARCH;
		} else if (strcmp(a, "--help") == 0)
			help();
		else if (strcmp(a, "--version") == 0) {
			do_write(FLATX_VERSION, strlen(FLATX_VERSION));
			do_write("\n", sizeof "\n");
			exit(0);
		} else
			die2(EXIT_CLIARG, "unknown option", a);
	}

	/*
	 *  The option --char-byte-index forbids other options.
	 */
	if (char_state == CHAR_STATE_SEARCH && argc != 3)
		die(EXIT_CLIARG, "option --char-byte-index: extra options");

	/*
	 *  Default order is byte index in document.
	 */
	if (!orders[0])
		orders[0] = ORDER_BYTE;
	
	/*
	 *  Set up the path, field, and row separator characters.
	 */
	if (!path_sep)
		path_sep = PATH_SEPARATOR_DEFAULT;
	path_sep_length = strlen(path_sep);

	if (!field_sep)
		field_sep = FIELD_SEPARATOR_DEFAULT;
	field_sep_length = strlen(field_sep);

	if (!row_sep)
		row_sep = ROW_SEPARATOR_DEFAULT;
	row_sep_length = strlen(row_sep);

	/*
	 *  Create element stack and intialize stack pointers.
	 */
	element_next = elements = (struct element *)calloc(
							INIT_ELEMENT_STACK_SIZE,
							sizeof *elements
						);
	if (element_next == NULL)
		die(EXIT_OOM, "calloc(element stack) failed");
	element_limit = &elements[INIT_ELEMENT_STACK_SIZE];

	/*
	 *   Create expat xml parser
	 */
	parser = XML_ParserCreate(NULL);
	if (parser == NULL)
		die(EXIT_PARSECREATE, "XML_ParserCreate(NULL) failed");

	/*
	 *  Install the normal element callbacks or the char data extractor
	 *  callbacks.
	 */
	if (char_state == CHAR_STATE_OFF)
		XML_SetElementHandler(parser, start_element, end_element);
	else {
		XML_SetElementHandler(parser, char_start_element,
							char_end_element);
		XML_SetCharacterDataHandler(parser, character_data);
	}
	do
	{
		/*
		 *  Read a buffer full.
		 */
		int length = do_read(buf, sizeof buf);
		done = (length == 0);

		if (length == 0 && done)
			break;

		/*
		 *  Parse the file, firing the registered callbacks.
		 */
		if (!XML_Parse(parser, buf, length, done)) 
			die2(EXIT_XML_PARSE, "xml parse", (char *)
				XML_ErrorString(XML_GetErrorCode(parser)));
	}
	while (!done);
	XML_ParserFree(parser);

	/*
	 *  Verify the requested starting char node exists.
	 */
	if (char_state != CHAR_STATE_OFF) {
		char buf[MAX_MSG_SIZE];

		snprintf(buf, sizeof buf, "byte index: %llu", char_byte_index);
		switch (char_state) {
		case CHAR_STATE_SEARCH:
			die(EXIT_NO_ELEMENT, buf);
			/*NOTREACHED*/

		/*
		 *  This error implies a syntactically incorrect document,
		 *  which should have already aborted.
		 */
		case CHAR_STATE_EXTRACTING:
			die(EXIT_PANIC, "impossible CHAR_STATE_EXTRACTING");
			/*NOTREACHED*/

		case CHAR_STATE_DONE:
			break;

		default:
			die(EXIT_PANIC, "impossible char state");
		}
	}
	exit(0);
}

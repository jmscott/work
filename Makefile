#
#  Synopsis:
#	Makefile for compiling jmscott tools from github.com/jmscott/work
#  See:
#	https://github.com/jmscott/work
#
COMPILED=idiff istext

all: $(COMPILED)

clean:
	rm -f $(COMPILED)

idiff: idiff.c
	cc -Wall -Wextra -o idiff idiff.c

istext: istext.c
	cc -Wall -Wextra -o istext istext.c

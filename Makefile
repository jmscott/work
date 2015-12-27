#
#  Synopsis:
#	Makefile for compiling jmscott tools from github.com/jmscott/work
#  See:
#	https://github.com/jmscott/work
#
COMPILED=idiff

all: $(COMPILED)

clean:
	rm -f $(COMPILED)

idiff: idiff.c
	cc -Wall -Wextra -o idiff idiff.c

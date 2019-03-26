#
#  Synopsis:
#	Makefile for compiling jmscott tools from github.com/jmscott/work
#  See:
#	https://github.com/jmscott/work
#  Note:
#	Need to add a recipe for "install".
#
COMPILED=fork-me halloc.o idiff istext

all: $(COMPILED)

clean:
	rm -f $(COMPILED)

idiff: idiff.c
	cc -Wall -Wextra -o idiff idiff.c

istext: istext.c
	cc -Wall -Wextra -o istext istext.c

halloc.o: halloc.c
	cc -Wall -Wextra -c halloc.c

fork-me: fork-me.c
	cc -Wall -Wextra -o fork-me fork-me.c

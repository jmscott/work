#
#  Synopsis:
#	Makefile for compiling jmscott tools from github.com/jmscott/work
#  See:
#	https://github.com/jmscott/work
#
COMPILED=fork-bench halloc.o idiff istext

all: $(COMPILED)

clean:
	rm -f $(COMPILED)

idiff: idiff.c
	cc -Wall -Wextra -o idiff idiff.c

istext: istext.c
	cc -Wall -Wextra -o istext istext.c

halloc.o: halloc.c
	cc -Wall -Wextra -c halloc.c

fork-bench: fork-bench.c
	cc -Wall -Wextra -o fork-bench fork-bench.c

#
#  Synopsis:
#	Makefile for compiling jmscott tools from github.com/jmscott/work
#  See:
#	https://github.com/jmscott/work
#  Note:
#	See local-<os>.mk.example
#
include local.mk
include jmscott.mk

COMPILED=fork-me halloc.o idiff istext

all: $(COMPILED)
install: all
	install -g $(DIST_GROUP) -o $(DIST_USER) -m u=rwx,go=rx		\
		-d $(DIST_ROOT)
	install -g $(DIST_GROUP) -o $(DIST_USER) -m u=rwx,go=rx		\
		-d $(DIST_ROOT)/bin
	install -g $(DIST_GROUP) -o $(DIST_USER) -m ugo=r		\
		idiff							\
		istext							\
		overwrite						\
		pdf-merge						\
		pg2pg							\
		rsunk							\
		rsync-fs						\
		ssh-host						\
		strip-cr						\
		strswap							\
		svn-commit-notify					\
		svn-ignore						\
		xtitle							\
		$(DIST_ROOT)/bin
	install -g $(DIST_GROUP) -o $(DIST_USER) -m u=rwx,go=rx		\
		-d $(DIST_ROOT)/src
	install -g $(DIST_GROUP) -o $(DIST_USER) -m ugo=r		\
		flatx.c							\
		fork-me.c						\
		halloc.c						\
		hexdump.c						\
		idiff.c							\
		istext.c						\
		$(DIST_ROOT)/src

clean:
	rm -f $(COMPILED)
distclean:
	rm -rf $(DIST_ROOT)/bin $(DIST_ROOT)/src 

idiff: idiff.c
	cc -Wall -Wextra -o idiff idiff.c

istext: istext.c
	cc -Wall -Wextra -o istext istext.c

halloc.o: halloc.c
	cc -Wall -Wextra -c halloc.c

fork-me: fork-me.c
	cc -Wall -Wextra -o fork-me fork-me.c

world:
	$(MAKE) $(MFLAGS) clean
	$(MAKE) $(MFLAGS) all
	$(MAKE) $(MFLAGS) distclean
	$(MAKE) $(MFLAGS) install

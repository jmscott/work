#
#  Synopsis:
#	Makefile for compiling jmscott tools into $PREFIX
#  See:
#	local-linux.mk.example
#	local-darwin.mk.example
#	https://github.com/jmscott/work
#  Note:
#	See local-<os>.mk.example
#
include local.mk
include jmscott.mk

COMPILED=fork-me halloc.o idiff istext stale-mtime

all: $(COMPILED)
	cd httpd2 && $(MAKE) $(MFLAGS) all
install: all
	install -g $(DIST_GROUP) -o $(DIST_USER) -m u=rwx,go=rx		\
		-d $(DIST_ROOT)
	install -g $(DIST_GROUP) -o $(DIST_USER) -m u=rwx,go=rx		\
		-d $(DIST_ROOT)/bin
	install -g $(DIST_GROUP) -o $(DIST_USER) -m ugo=xr		\
		idiff							\
		istext							\
		make-dist						\
		overwrite						\
		pdf-merge						\
		pg2pg							\
		rsunk							\
		rsync-fs						\
		ssh-host						\
		stale-mtime						\
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
		stale-mtime.c						\
		$(DIST_ROOT)/src
	cd httpd2 && $(MAKE) $(MFLAGS) install

clean:
	cd httpd2 && $(MAKE) $(MFLAGS) clean
	rm -f $(COMPILED)
distclean:
	cd httpd2 && $(MAKE) $(MFLAGS) distclean
	rm -rf $(DIST_ROOT)/bin $(DIST_ROOT)/src 

idiff: idiff.c
	cc -Wall -Wextra -o idiff idiff.c

istext: istext.c
	cc -Wall -Wextra -o istext istext.c

halloc.o: halloc.c
	cc -Wall -Wextra -c halloc.c

fork-me: fork-me.c
	cc -Wall -Wextra -o fork-me fork-me.c

stale-mtime: stale-mtime.c
	cc -Wall -Wextra -o stale-mtime stale-mtime.c

world:
	cd httpd2 && $(MAKE) $(MFLAGS) world
	$(MAKE) $(MFLAGS) clean
	$(MAKE) $(MFLAGS) all
	$(MAKE) $(MFLAGS) distclean
	$(MAKE) $(MFLAGS) install

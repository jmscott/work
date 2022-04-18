#
#  Synopsis:
#	Makefile for compiling jmscott tools into $PREFIX
#  See:
#	local-linux.mk.example
#	local-darwin.mk.example
#	https://github.com/jmscott/work
#  Note:
#	Add warning when running ./make-dist instead of previously installed
#	version in $JMSCOTT_ROOT/bin.
#
#	Install README in /usr/local/jmscott/README, describing github location
#	and digest of installed code (typically trunk).
#
#	Why put launched-log in $JMSCOTT_ROOT/sbin?
#	Seems like /bin/ better for permissons, since typically /sbin/
#	is only owned by a particular user.
#
#	Add recipe to install remote dependencies!
#
include local.mk
include jmscott.mk

$(shell test -h jmscott || ln -s clang jmscott)

_MAKE=$(MAKE) $(MFLAGS)

DIST=work.dist
COMPILEs := $(shell  (. ./$(DIST) && echo $$COMPILEs))
SBINs := $(shell  (. ./$(DIST) && echo $$SBINs))
LIBs := $(shell  (. ./$(DIST) && echo $$LIBs))
BINs := $(shell  (. ./$(DIST) && echo $$BINs))
SRCs := $(shell  (. ./$(DIST) && echo $$SRCs))

all: $(COMPILEs)
	cd clang && $(_MAKE) all
	cd www && $(_MAKE) all

install-dirs:
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)/bin
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)/lib
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)/include
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)/libexec
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)/sbin
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)/src
install: all
	$(_MAKE) install-dirs

	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m ugo=xr	\
		$(SBINs)						\
		$(JMSCOTT_PREFIX)/sbin
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m ugo=xr	\
		$(LIBs)							\
		$(JMSCOTT_PREFIX)/lib
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m ugo=xr	\
		$(BINs)							\
		$(JMSCOTT_PREFIX)/bin
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m ugo=r		\
		$(SRCs)							\
		$(JMSCOTT_PREFIX)/src
	cd make-dist && $(_MAKE) install
	cd clang && $(_MAKE) install
	cd pgsnap && $(_MAKE) install
	cd www && $(_MAKE) install

clean:
	cd clang && $(_MAKE) clean
	cd www && $(_MAKE) clean
	rm -f $(COMPILED)
distclean:
	cd clang && $(_MAKE) $(MFLAGS) distclean
	cd www && $(_MAKE) distclean
	cd pgsnap && $(_MAKE) distclean
	cd make-dist && $(_MAKE) distclean
	rm -rf $(JMSCOTT_PREFIX)/bin
	rm -rf $(JMSCOTT_PREFIX)/sbin
	rm -rf $(JMSCOTT_PREFIX)/src
	rm -rf $(JMSCOTT_PREFIX)/lib

RFC3339Nano: RFC3339Nano.c
	cc $(CFLAGS) -o RFC3339Nano RFC3339Nano.c

idiff: idiff.c
	cc $(CFLAGS) -o idiff idiff.c

duration-english: duration-english.c
	cc $(CFLAGS) -o duration-english duration-english.c

istext: istext.c
	cc $(CFLAGS) -o istext istext.c

fork-me: fork-me.c
	cc $(CFLAGS) -o fork-me fork-me.c

stale-mtime: stale-mtime.c
	cc $(CFLAGS) -o stale-mtime stale-mtime.c

stat-mtime: stat-mtime.c
	cc $(CFLAGS) -o stat-mtime stat-mtime.c

file-stat-size: file-stat-size.c
	cc $(CFLAGS) -o file-stat-size file-stat-size.c

flatx: flatx.c
	cc $(CFLAGS) -o flatx flatx.c -lexpat

escape-json-string: escape-json-string.c
	cc $(CFLAGS) -o escape-json-string escape-json-string.c

duration-mtime: duration-mtime.c
	cc $(CFLAGS) -o duration-mtime duration-mtime.c

tas-lock-fs: tas-lock-fs.c
	cc $(CFLAGS) -o tas-lock-fs tas-lock-fs.c

tas-unlock-fs: tas-unlock-fs.c
	cc $(CFLAGS) -o tas-unlock-fs tas-unlock-fs.c

pg_launchd: pg_launchd.c
	cc $(CFLAGS) -o pg_launchd pg_launchd.c

slice-file: slice-file.c
	cc $(CFLAGS) -I$(JMSCOTT_ROOT)/include -o slice-file slice-file.c

world:
	$(_MAKE) clean
	$(_MAKE) all
	$(_MAKE) distclean
	$(_MAKE) install
	cd make-dist && $(_MAKE) world
	cd clang && $(_MAKE) world
	cd pgsnap && $(_MAKE) pgsnap
	cd www && $(_MAKE) world
dist:
	make-dist $(DIST)

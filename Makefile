#
#  Synopsis:
#	Makefile for compiling jmscott tools into $PREFIX
#  See:
#	local-linux.mk.example
#	local-darwin.mk.example
#	https://github.com/jmscott/work
#  Note:
#	Probably time to retire flatx.
#
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

_MAKE=$(MAKE) $(MFLAGS)

DIST=work.dist
COMPILEs := $(shell  (. ./$(DIST) && echo $$COMPILEs))
SBINs := $(shell  (. ./$(DIST) && echo $$SBINs))
LIBs := $(shell  (. ./$(DIST) && echo $$LIBs))
BINs := $(shell  (. ./$(DIST) && echo $$BINs))
SRCs := $(shell  (. ./$(DIST) && echo $$SRCs))

JMSLIB=clib/libjmscott.a

all: $(COMPILEs)
	cd clib && $(_MAKE) all
	cd www && $(_MAKE) all

.PHONY: clean all distclean nstall install-dirs world

jmscott:
	test -e jmscott || ln -s clib jmscott
$(COMPILEs): jmscott

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
	cd clib && $(_MAKE) install
	cd pgsnap && $(_MAKE) install
	cd www && $(_MAKE) install

clean:
	cd clib && $(_MAKE) clean
	cd www && $(_MAKE) clean
	rm -f $(COMPILEs) jmscott
distclean:
	cd clib && $(_MAKE) $(MFLAGS) distclean
	cd www && $(_MAKE) distclean
	cd pgsnap && $(_MAKE) distclean
	cd make-dist && $(_MAKE) distclean
	rm -rf $(JMSCOTT_PREFIX)/bin
	rm -rf $(JMSCOTT_PREFIX)/sbin
	rm -rf $(JMSCOTT_PREFIX)/src
	rm -rf $(JMSCOTT_PREFIX)/lib

$JMSLIB:
	cd clib && $(_MAKE) libjmscott.a 

RFC3339Nano: RFC3339Nano.c $JMSLIB
	$(CCOMPILE) -o RFC3339Nano RFC3339Nano.c $(CLINK)

idiff: idiff.c $JMSLIB
	$(CCOMPILE) -o idiff idiff.c $(CLINK)

duration-english: duration-english.c $JMSLIB
	$(CCOMPILE) -o duration-english duration-english.c $(CLINK)

istext: istext.c $JMSLIB
	$(CCOMPILE)  -o istext istext.c $(CLINK)

fork-me: fork-me.c $JMSLIB
	$(CCOMPILE) -o fork-me fork-me.c $(CLINK)

stale-mtime: stale-mtime.c $JMSLIB
	$(CCOMPILE) -o stale-mtime stale-mtime.c $(CLINK)

stat-mtime: stat-mtime.c $JMSLIB
	$(CCOMPILE) -o stat-mtime stat-mtime.c $(CLINK)

file-stat-size: file-stat-size.c $JMSLIB
	$(CCOMPILE) -o file-stat-size file-stat-size.c $(CLINK)

flatx: flatx.c $JMSLIB
	$(CCOMPILE) -o flatx flatx.c -lexpat $(CLINK)

escape-json-string: escape-json-string.c $JMSLIB
	$(CCOMPILE) -o escape-json-string escape-json-string.c $(CLINK)

duration-mtime: duration-mtime.c $JMSLIB
	$(CCOMPILE) -o duration-mtime duration-mtime.c $(CLINK)

tas-lock-fs: tas-lock-fs.c $JMSLIB
	$(CCOMPILE) -o tas-lock-fs tas-lock-fs.c $(CLINK)

tas-unlock-fs: tas-unlock-fs.c $JMSLIB
	$(CCOMPILE) -o tas-unlock-fs tas-unlock-fs.c $(CLINK)

pg_launchd: pg_launchd.c $JMSLIB
	$(CCOMPILE) -o pg_launchd pg_launchd.c $(CLINK)

slice-file: slice-file.c $JMSLIB $JMSLIB
	$(CCOMPILE) -o slice-file slice-file.c $(CLINK)

world:
	$(_MAKE) clean
	$(_MAKE) all
	$(_MAKE) distclean
	$(_MAKE) install
	cd make-dist && $(_MAKE) world
	cd clib && $(_MAKE) world
	cd pgsnap && $(_MAKE) pgsnap
	cd www && $(_MAKE) world
dist:
	make-dist $(DIST)

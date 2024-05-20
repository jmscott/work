#
#  Synopsis:
#	Makefile for compiling jmscott tools into $PREFIX
#  See:
#	local-linux.mk.example
#	local-darwin.mk.example
#	https://github.com/jmscott/work
#  Note:
#	Makefile dependencies are still wrong!
#
#	Probably time to retire flatx.
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

MKMK=work.mkmk
COMPILEs := $(shell  (. ./$(MKMK) && echo $$COMPILEs))
SBINs := $(shell  (. ./$(MKMK) && echo $$SBINs))
LIBs := $(shell  (. ./$(MKMK) && echo $$LIBs))
BINs := $(shell  (. ./$(MKMK) && echo $$BINs))
SRCs := $(shell  (. ./$(MKMK) && echo $$SRCs))

JMSLIB=clib/libjmscott.a
JMSINC=clib/libjmscott.h

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
	cd make-make && $(_MAKE) install
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
	cd make-make && $(_MAKE) distclean
	rm -rf $(JMSCOTT_PREFIX)/bin
	rm -rf $(JMSCOTT_PREFIX)/sbin
	rm -rf $(JMSCOTT_PREFIX)/src
	rm -rf $(JMSCOTT_PREFIX)/lib

$(JMSLIB): $(JMSINC)
	cd clib && $(_MAKE) all

RFC3339Nano: RFC3339Nano.c $(JMSLIB) $(JMSINC)
	$(CCOMPILE) -o RFC3339Nano RFC3339Nano.c $(CLINK)

idiff: idiff.c $(JMSLIB) $(JMSINC)
	$(CCOMPILE) -o idiff idiff.c $(CLINK)

duration-english: duration-english.c $(JMSLIB) $(JMSINC)
	$(CCOMPILE) -o duration-english duration-english.c $(CLINK)

istext: istext.c $(JMSLIB) $(JMSINC)
	$(CCOMPILE)  -o istext istext.c $(CLINK)

fork-me: fork-me.c $(JMSLIB) $(JMSINC)
	$(CCOMPILE) -o fork-me fork-me.c $(CLINK)

file-stale-mtime: file-stale-mtime.c $(JMSLIB) $(JMSINC)
	$(CCOMPILE) -o file-stale-mtime file-stale-mtime.c $(CLINK)

file-stat-size: file-stat-size.c $(JMSLIB) $(JMSINC)
	$(CCOMPILE) -o file-stat-size file-stat-size.c $(CLINK)

byte-size-english: byte-size-english.c $(JMSLIB) $(JMSINC)
	$(CCOMPILE) -o byte-size-english byte-size-english.c $(CLINK)

file-stat-mtime: file-stat-mtime.c $(JMSLIB) $(JMSINC)
	$(CCOMPILE) -o file-stat-mtime file-stat-mtime.c $(CLINK)

flatx: flatx.c $(JMSLIB) $(JMSINC)
	$(CCOMPILE) -o flatx flatx.c -lexpat $(CLINK)

escape-json-string: escape-json-string.c $(JMSLIB) $(JMSINC)
	$(CCOMPILE) -o escape-json-string escape-json-string.c $(CLINK)

duration-mtime: duration-mtime.c $(JMSLIB) $(JMSINC)
	$(CCOMPILE) -o duration-mtime duration-mtime.c $(CLINK)

tas-lock-fs: tas-lock-fs.c $(JMSLIB) $(JMSINC)
	$(CCOMPILE) -o tas-lock-fs tas-lock-fs.c $(CLINK)

tas-unlock-fs: tas-unlock-fs.c $(JMSLIB) $(JMSINC)
	$(CCOMPILE) -o tas-unlock-fs tas-unlock-fs.c $(CLINK)

pg_launchd: pg_launchd.c $(JMSLIB) $(JMSINC)
	$(CCOMPILE) -o pg_launchd pg_launchd.c $(CLINK)

send-file-slice: send-file-slice.c $(JMSLIB) $(JMSINC)
	$(CCOMPILE) -o send-file-slice send-file-slice.c $(CLINK)

is-utf8wf: is-utf8wf.c $(JMSLIB) $(JMSINC)
	$(CCOMPILE) -o is-utf8wf is-utf8wf.c $(CLINK)

is-dir-empty: is-dir-empty.c $(JMSLIB) $(JMSINC)
	$(CCOMPILE) -o is-dir-empty is-dir-empty.c $(CLINK)

udp4-listen: udp4-listen.go
	$(GOEXE) build udp4-listen.go

udp4-reflect: udp4-reflect.go
	$(GOEXE) build udp4-reflect.go

env2json: env2json.go
	$(GOEXE) build env2json.go

stdin2go-byte: stdin2go-byte.go
	$(GOEXE) build stdin2go-byte.go

stdin2go-literal: stdin2go-literal.go
	$(GOEXE) build stdin2go-literal.go

world:
	cd make-make && $(_MAKE) world
	cd clib && $(_MAKE) world
	cd pgsnap && $(_MAKE) pgsnap
	cd www && $(_MAKE) world

	$(_MAKE) clean
	$(_MAKE) all
	$(_MAKE) distclean
	$(_MAKE) install
tar:
	make-make tar $(MKMK)

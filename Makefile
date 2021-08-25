#
#  Synopsis:
#	Makefile for compiling jmscott tools into $PREFIX
#  See:
#	local-linux.mk.example
#	local-darwin.mk.example
#	https://github.com/jmscott/work
#  Note:
#	Need to make a dist file for work/ !!!  See play/
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

COMPILED=								\
	RFC3339Nano							\
	duration-english						\
	duration-mtime							\
	escape-json-string						\
	flatx								\
	fork-me								\
	halloc.o							\
	idiff								\
	istext								\
	pg_launchd							\
	stale-mtime							\
	stat-mtime							\

all: $(COMPILED)
	cd www && $(MAKE) $(MFLAGS) all
install: all
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)/lib
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)/bin
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)/sbin
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m ugo=xr	\
		launchd-log						\
		pg_launchd						\
		$(JMSCOTT_PREFIX)/sbin
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m ugo=xr	\
		bash_login.example					\
		$(JMSCOTT_PREFIX)/lib
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m ugo=xr	\
		RFC3339Nano						\
		duration-english					\
		duration-mtime						\
		elapsed-english						\
		escape-json-string					\
		exec-logoff						\
		idiff							\
		isjson							\
		istext							\
		make-dist						\
		overwrite						\
		pdf-merge						\
		pg2pg							\
		pg_dump-Dow						\
		rsunk							\
		rsync-fs						\
		ssh-host						\
		stale-mtime						\
		stat-mtime						\
		strip-cr						\
		strswap							\
		svn-commit-notify					\
		svn-ignore						\
		xtitle							\
		zap-proc						\
		$(JMSCOTT_PREFIX)/bin
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)/src
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m ugo=r		\
		RFC3339Nano.c						\
		duration-english.c					\
		duration-mtime.c					\
		escape-json-string.c					\
		flatx.c							\
		fork-me.c						\
		halloc.c						\
		hexdump.c						\
		idiff.c							\
		istext.c						\
		pg_launchd.c						\
		stale-mtime.c						\
		$(JMSCOTT_PREFIX)/src
	cd pgsnap && $(MAKE) $(MFLAGS) install
	cd www && $(MAKE) $(MFLAGS) install

clean:
	cd www && $(MAKE) $(MFLAGS) clean
	rm -f $(COMPILED)
distclean:
	cd www && $(MAKE) $(MFLAGS) distclean
	cd pgsnap && $(MAKE) $(MFLAGS) distclean
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

halloc.o: halloc.c
	cc $(CFLAGS) -c halloc.c

fork-me: fork-me.c
	cc $(CFLAGS) -o fork-me fork-me.c

stale-mtime: stale-mtime.c
	cc $(CFLAGS) -o stale-mtime stale-mtime.c

stat-mtime: stat-mtime.c
	cc $(CFLAGS) -o stat-mtime stat-mtime.c

flatx: flatx.c
	cc $(CFLAGS) -o flatx flatx.c -lexpat

escape-json-string: escape-json-string.c
	cc $(CFLAGS) -o escape-json-string escape-json-string.c

duration-mtime: duration-mtime.c
	cc $(CFLAGS) -o duration-mtime duration-mtime.c

pg_launchd: pg_launchd.c
	cc $(CFLAGS) -o pg_launchd pg_launchd.c

world:
	cd www && $(MAKE) $(MFLAGS) world
	$(MAKE) $(MFLAGS) clean
	$(MAKE) $(MFLAGS) all
	$(MAKE) $(MFLAGS) distclean
	$(MAKE) $(MFLAGS) install

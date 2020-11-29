#
#  Synopsis:
#	Makefile for compiling jmscott tools into $PREFIX
#  See:
#	local-linux.mk.example
#	local-darwin.mk.example
#	https://github.com/jmscott/work
#  Note:
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
	duration-english						\
	flatx								\
	fork-me								\
	halloc.o							\
	idiff								\
	istext								\
	stale-mtime							\

all: $(COMPILED)
	cd www && $(MAKE) $(MFLAGS) all
install: all
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)/bin
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)/sbin
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m ugo=xr	\
		launchd-log						\
		$(JMSCOTT_PREFIX)/sbin
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m ugo=xr	\
		duration-english					\
		exec-logoff						\
		idiff							\
		isjson							\
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
		zap-proc						\
		$(JMSCOTT_PREFIX)/bin
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)/src
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m ugo=r		\
		flatx.c							\
		fork-me.c						\
		halloc.c						\
		hexdump.c						\
		idiff.c							\
		istext.c						\
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

flatx: flatx.c
	cc $(CFLAGS) -o flatx flatx.c -lexpat

world:
	cd www && $(MAKE) $(MFLAGS) world
	$(MAKE) $(MFLAGS) clean
	$(MAKE) $(MFLAGS) all
	$(MAKE) $(MFLAGS) distclean
	$(MAKE) $(MFLAGS) install

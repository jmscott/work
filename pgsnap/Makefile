#
#  Synopsis:
#	Makefile for pgsnap utility.
#  Note:
#	Move explicite directory creation in install recipe to
#	recipe in parent Makefile.
#
include ../local.mk
include ../jmscott.mk

install:
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)/bin
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)/lib/pgsnap

	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m ugo=xr	\
		pgsnap							\
		pgsnap-janitor						\
		$(JMSCOTT_PREFIX)/bin
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m ugo=xr	\
		crontab.example						\
		profile.example						\
		$(JMSCOTT_PREFIX)/lib/pgsnap
world:
	$(MAKE) $(MFLAGS) distclean
	$(MAKE) $(MFLAGS) install
distclean:
	rm -f $(JMSCOTT_PREFIX)/bin/pgsnap
	rm -f $(JMSCOTT_PREFIX)/bin/pgsnap-janitor
	rm -rf $(JMSCOTT_PREFIX)/lib/pgsnap

dev-links:
	test -e log || ln -s . log
	test -e etc || ln -s . etc
	test -e run || ln -s . run
	test -e spool || ln -s . spool
dist:
	make-dist dist-root.conf

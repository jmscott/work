#
#  Synopsis:
#	Makefile for pgsnap utility.
#  Note:
#	Move explicite directory creation in install recipe to
#	recipe in parent Makefile.
#
include ../local.mk
include ../jmscott.mk

_MAKE=$(MAKE) $(MFLAGS)

DIST=work-pgsnap.dist
BINs := $(shell  (. ./$(DIST) && echo $$BINs))
LIBs := $(shell  (. ./$(DIST) && echo $$LIBs))

install-dirs:
	cd .. && $(_MAKE) install-dirs

	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)/lib/pgsnap

install:
	$(_MAKE) install-dirs
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m ugo=xr	\
		$(BINs)							\
		$(JMSCOTT_PREFIX)/bin
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m ugo=xr	\
		$(LIBs)							\
		$(JMSCOTT_PREFIX)/lib/pgsnap
world:
	$(_MAKE) distclean
	$(_MAKE) install
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
	make-dist $(DIST)

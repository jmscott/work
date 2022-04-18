#
#  Synopsis:
#	Makefile for pgsnap utility.
#  Note:
#	Consider renaming script to "make-stuff".
#
include ../local.mk
include ../jmscott.mk

_MAKE=$(MAKE) $(MFLAGS)

DIST=work-make_dist.dist
BINs := $(shell  (. ./$(DIST) && echo $$BINs))
LIBs := $(shell  (. ./$(DIST) && echo $$LIBs))
LIBEXECs := $(shell  (. ./$(DIST) && echo $$LIBEXECs))

install-dirs:
	cd .. && $(_MAKE) install-dirs

	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)/libexec/pgsnap

install:
	$(_MAKE) install-dirs
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m ugo=xr	\
		$(BINs)							\
		$(JMSCOTT_PREFIX)/bin
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m ugo=xr	\
		$(LIBs)							\
		$(JMSCOTT_PREFIX)/lib
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m ugo=xr	\
		$(LIBEXECs)						\
		$(JMSCOTT_PREFIX)/libexec
world:
	$(_MAKE) distclean
	$(_MAKE) install

distclean:
	cd $(JMSCOTT_PREFIX)/bin && rm -f $(BINs)
	cd $(JMSCOTT_PREFIX)/libexec && rm -f $(LIBEXECs)
	cd $(JMSCOTT_PREFIX)/lib && rm -f $(LIBs)

dev-links:
	test -e log || ln -s . log
	test -e etc || ln -s . etc
	test -e run || ln -s . run
	test -e spool || ln -s . spool
dist:
	make-dist tar $(DIST)

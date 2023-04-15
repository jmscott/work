#
#  Synopsis:
#	Makefile for jmscott/bin/make-make utility.
#
include ../local.mk
include ../jmscott.mk

_MAKE=$(MAKE) $(MFLAGS)

MKMK=work-make-make.mkmk
BINs := $(shell  (. ./$(MKMK) && echo $$BINs))
LIBs := $(shell  (. ./$(MKMK) && echo $$LIBs))
LIBEXECs := $(shell  (. ./$(MKMK) && echo $$LIBEXECs))

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
	test ! -e $(JMSCOTT_PREFIX)/bin		||			\
		(cd $(JMSCOTT_PREFIX)/bin && rm -f $(BINs))
	test ! -e $(JMSCOTT_PREFIX)/libexec	||			\
		(cd $(JMSCOTT_PREFIX)/libexec && rm -f $(LIBEXECs))
	test ! -e $(JMSCOTT_PREFIX)/lib		||			\
		(cd $(JMSCOTT_PREFIX)/lib && rm -f $(LIBs))

dev-links:
	test -e log || ln -s . log
	test -e etc || ln -s . etc
	test -e run || ln -s . run
	test -e spool || ln -s . spool
tar:
	make-make tar $(MKMK)

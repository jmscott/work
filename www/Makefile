#
#  Synopsis:
#	Standalone install of $JMSCOTT_ROOT/www
#  Note:
#	File modes not set correctly for dirs {sbin,lib,htdocs}/
#
#	the process env JMSCOTT_COMPILE_WWW=1 determines if various
#	www tools are installed.  to disable build set JMSCOTT_COMPILE_WWW=0.
#	note that even when disable the "clean" and "distclean" still remove
#	files.
#
#	Makefile variables can be extracted and defined from the the
#	root.mkmk file with these recipes.  Example make code follows:
#
#		MKMK=product.mkmk
#
#		COMPILEs := $(shell (. ./$(MKMK) && echo $$COMPILEs))
#		SBINs := $(shell (. ./$(MKMK) && echo $$SBINs))
#		LIBs := $(shell (. ./$(MKMK) && echo $$LIBs))
#		SRCs := $(shell (. ./$(MKMK) && echo $$SRCs))
#		BINs := $(shell (. ./$(MKMK) && echo $$BINs))
#		HTDOCSs := $(shell (. ./$(MKMK) && echo $$HTDOCSs))
#		CGIBINs := $(shell (. ./$(MKMK) && echo $$CGIBINs))
#		LIBEXEC := $(shell (. ./$(MKMK) && echo $$LIBEXECs))
#

MKMK=work-www.mkmk

COMPILEs := $(shell (. ./$(MKMK) && echo $$COMPILEs))
LIBs := $(shell (. ./$(MKMK) && echo $$LIBs))
SRCs := $(shell (. ./$(MKMK) && echo $$SRCs))
BINs := $(shell (. ./$(MKMK) && echo $$BINs))
SBINs := $(shell (. ./$(MKMK) && echo $$SBINs))
HTDOCSs := $(shell (. ./$(MKMK) && echo $$HTDOCSs))
CGIBINs := $(shell (. ./$(MKMK) && echo $$CGIBINs))

include ../local.mk
include ../jmscott.mk

WWW_PREFIX?=$(JMSCOTT_PREFIX)/www
WWW_USER?=$(INSTALL_USER)
WWW_GROUP?=$(INSTALL_GROUP)

ifeq ($(JMSCOTT_COMPILE_WWW),1)
all: $(COMPILEs)
else
all:
	@echo 'all www: JMSCOTT_COMPILE_WWW !=1: skipping (ok)'
	@exit 0
endif

distclean:
	rm -rf $(WWW_PREFIX)/src
	rm -rf $(WWW_PREFIX)/lib
	rm -rf $(WWW_PREFIX)/bin
	rm -rf $(WWW_PREFIX)/sbin
	rm -rf $(WWW_PREFIX)/cgi-bin

ifeq ($(JMSCOTT_COMPILE_WWW),1)
install: all
	install -g $(WWW_GROUP) -o $(WWW_USER) -m u=wrx,go=rx		\
		-d $(JMSCOTT_PREFIX)

	install -g $(WWW_GROUP) -o $(WWW_USER) -m u=wrx,go=rx		\
		-d $(JMSCOTT_PREFIX)/bin
	install -g $(WWW_GROUP) -o $(WWW_USER) -m u=wrx,g=rx,o=		\
		-d $(WWW_PREFIX)
	install -g $(WWW_GROUP) -o $(WWW_USER) -m u=wrx,g=rx,o=		\
		-d $(WWW_PREFIX)/cgi-bin
	install -g $(WWW_GROUP) -o $(WWW_USER) -m u=wrx,g=rx,o=		\
		-d $(WWW_PREFIX)/src
	install -g $(WWW_GROUP) -o $(WWW_USER) -m u=wrx,g=rx,o=		\
		-d $(WWW_PREFIX)/lib
	install -g $(WWW_GROUP) -o $(WWW_USER) -m u=wrx,g=rx,o=		\
		-d $(WWW_PREFIX)/bin
	install -g $(WWW_GROUP) -o $(WWW_USER) -m u=wrx,g=rx,o=		\
		-d $(WWW_PREFIX)/sbin
	install -g $(WWW_GROUP) -o $(WWW_USER) -m u=wrx,g=rx,o=		\
		-d $(WWW_PREFIX)/htdocs

	rsync --relative $(LIBs) $(WWW_PREFIX)/lib
	rsync --relative $(BINs) $(WWW_PREFIX)/bin
	rsync --relative $(HTDOCs) $(WWW_PREFIX)/htdocs
	rsync --relative $(SBINs) $(WWW_PREFIX)/sbin
	rsync --relative $(CGIBINs) $(WWW_PREFIX)/cgi-bin
else
install:
	@echo 'install www: JMSCOTT_COMPILE_WWW !=1: skipping (ok)'
	@exit 0
endif

clean:
	rm -f $(COMPILEs)

#
#  Dump the process environment for debugging.
#
env env.d/help.pl: env.cgi
	./cgi2perl5 --source-path env.cgi

#
#  Dump contents of a POSTED form during debuging.
#
form form.d/help.pl: form.cgi
	./cgi2perl5 --source-path form.cgi

world: clean all distclean install

rfc7578_2json: rfc7578_2json.go
	$(GOEXE) build rfc7578_2json.go
tar: all
	make-make tar $(MKMK)

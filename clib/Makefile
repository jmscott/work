#
#  Synopsis:
#	Makefile for populating $JMSCOTT_ROOT/lib/clang.
#  Note:
#	Rename dir clib/ to libjmscott/
# 
#	Need to collapse c compiles into single recipe: *.o:*.c
#
include ../local.mk
include ../jmscott.mk

_MAKE=$(MAKE) $(MFLAGS)

DIST=work-clang.dist
COMPILEs := $(shell  (. ./$(DIST) && echo $$COMPILEs))
OBJs := $(shell  (. ./$(DIST) && echo $$OBJs))
LIBs := $(shell  (. ./$(DIST) && echo $$LIBs))
SRCs := $(shell  (. ./$(DIST) && echo $$SRCs))
INCLUDEs := $(shell  (. ./$(DIST) && echo $$INCLUDEs))

all: $(COMPILEs)

.PHONY: clean all distclean nstall install-dirs world

#  for c code #include "jmscott/libjmscott.h"

jmscott:
	test -e jmscott || ln -s . jmscott

#  Note: replace $(OBJs) with *.o and link jmscott never created.  why?
$(OBJs): jmscott

clean:
	rm -f $(COMPILEs) jmscott

install-dirs: 
	cd .. && $(_MAKE) install-dirs

	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)/include/jmscott
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)/src/libjmscott
install: all
	$(_MAKE) install-dirs

	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m ugo=r		\
		$(INCLUDEs)						\
		$(JMSCOTT_PREFIX)/include/jmscott
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m ugo=r		\
		$(LIBs)							\
		$(JMSCOTT_PREFIX)/lib
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m ugo=r		\
		$(SRCs)							\
		$(JMSCOTT_PREFIX)/src/libjmscott

distclean:
	rm -rf $(JMSCOTT_PREFIX)/include/jmscott
	rm -rf $(JMSCOTT_PREFIX)/src/libjmscott
world:
	$(_MAKE) clean
	$(_MAKE) all
	$(_MAKE) distclean
	$(_MAKE) install

json.o: json.c libjmscott.h
	cc $(CFLAGS) -Wno-implicit-fallthrough -c json.c

halloc.o: halloc.c libjmscott.h
	cc $(CFLAGS) -c halloc.c

string.o: string.c libjmscott.h
	cc $(CFLAGS) -c string.c

hexdump.o: hexdump.c libjmscott.h
	cc $(CFLAGS) -c hexdump.c

posio.o: posio.c libjmscott.h
	cc $(CFLAGS) -c posio.c

time.o: time.c libjmscott.h
	cc $(CFLAGS) -c time.c

die.o: die.c libjmscott.h
	cc $(CFLAGS) -c die.c

net.o: net.c libjmscott.h
	cc $(CFLAGS) -c net.c

ecpg.o: ecpg.c libjmscott.h
	cc $(CFLAGS) -I$(PGINC) -c ecpg.c

dist:
	make-dist tar $(DIST)

libjmscott.a: $(OBJs) libjmscott.h
	ar crs libjmscott.a $(OBJs)

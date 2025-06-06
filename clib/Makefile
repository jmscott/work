#
#  Synopsis:
#	Makefile for populating $JMSCOTT_ROOT/lib/clang.
#  Note:
#	For gcc compiler consider option -fno-strict-aliasing.  See linus rants
#
#		https://lwn.net/Articles/316126/
#		https://lkml.org/lkml/2003/2/26/158
#
#	Rename dir clib/ to libjmscott/
# 
#	Need to collapse c compiles into single recipe: *.o:*.c
#
include ../local.mk
include ../jmscott.mk

_MAKE=$(MAKE) $(MFLAGS)

MKMK=work-clang.mkmk
COMPILEs := $(shell  (. ./$(MKMK) && echo $$COMPILEs))
OBJs := $(shell  (. ./$(MKMK) && echo $$OBJs))
LIBs := $(shell  (. ./$(MKMK) && echo $$LIBs))
SRCs := $(shell  (. ./$(MKMK) && echo $$SRCs))
INCLUDEs := $(shell  (. ./$(MKMK) && echo $$INCLUDEs))

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

dir.o: dir.c libjmscott.h
	cc $(CFLAGS) -c dir.c

net.o: net.c libjmscott.h
	cc $(CFLAGS) -c net.c

ecpg.o: ecpg.c libjmscott.h
	cc $(CFLAGS) -DJMSCOTT_COMPILE_PG=$(JMSCOTT_COMPILE_PG) -I$(PGINC) -c ecpg.c

tar:
	make-make tar $(MKMK)

libjmscott.a: $(OBJs) libjmscott.h
	ar crs libjmscott.a $(OBJs)

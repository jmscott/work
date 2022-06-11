#
#  Synopsis:
#	Makefile for populating $JMSCOTT_ROOT/lib/clang.
#  Note:
#	Need to collapse c compiles into single recipe: *.o:*.c
#
include ../local.mk
include ../jmscott.mk

_MAKE=$(MAKE) $(MFLAGS)

DIST=work-clang.dist
COMPILEs := $(shell  (. ./$(DIST) && echo $$COMPILEs))
OBJs := $(shell  (. ./$(DIST) && echo $$OBJs))
LIBs := $(shell  (. ./$(DIST) && echo $$LIBs))
INCLUDEs := $(shell  (. ./$(DIST) && echo $$INCLUDEs))

all: $(COMPILEs) include_link

include_link:
	test -e jmscott || ln -s . jmscott
clean:
	rm -f $(COMPILEs)

install-dirs: 
	cd .. && $(_MAKE) install-dirs

	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)/include/jmscott
install: all
	$(_MAKE) install-dirs

	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m ugo=r		\
		$(INCLUDEs)						\
		$(JMSCOTT_PREFIX)/include/jmscott
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m ugo=r		\
		$(LIBs)							\
		$(JMSCOTT_PREFIX)/lib

distclean:
	rm -rf $(JMSCOTT_PREFIX)/include/jmscott
world:
	$(_MAKE) clean
	$(_MAKE) all
	$(_MAKE) distclean
	$(_MAKE) install

*.c: include_link

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

ecpg.o: ecpg.c libjmscott.h
	cc $(CFLAGS) -I$(PGINC) -c ecpg.c

dist:
	make-dist tar $(DIST)

libjmscott.a: $(OBJs) libjmscott.h
	ar crs libjmscott.a $(OBJs)

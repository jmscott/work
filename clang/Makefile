#
#  Synopsis:
#	Makefile for populating $JMSCOTT_ROOT/lib/clang.
#  Note:
#	Need to collapse c compiles into single recipe: *.o:*.c
#
include ../local.mk
include ../jmscott.mk

COMPILEs=							\
	die.c							\
	ecpg.c							\
	halloc.c						\
	hexdump.c						\
	json.c							\
	posio.c							\
	string.c						\
	time.c							\

#
#  Note:
#	ought to be OBJs=$(shell echo $(COMPILEs) | sed 's/\..c/.o/')
OBJs=								\
	die.o							\
	ecpg.o							\
	halloc.o						\
	hexdump.o						\
	json.o							\
	posio.o							\
	string.o						\
	time.o							\

all: $(OBJs) include_link

include_link:
	test -e jmscott || ln -s . jmscott
clean:
	rm -f $(OBJs)

install: 
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)/include
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)/include/jmscott
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m ugo=xr	\
		$(COMPILEs)						\
		$(JMSCOTT_PREFIX)/include/jmscott

distclean:
	rm -rf $(JMSCOTT_PREFIX)/include/jmscott
world:
	$(MAKE) $(MFLAGS) clean
	$(MAKE) $(MFLAGS) all
	$(MAKE) $(MFLAGS) distclean
	$(MAKE) $(MFLAGS) install

*.c: include_link

json.o: json.c
	cc $(CFLAGS) -c json.c

halloc.o: halloc.c
	cc $(CFLAGS) -c halloc.c

string.o: string.c
	cc $(CFLAGS) -c string.c

hexdump.o: hexdump.c
	cc $(CFLAGS) -c hexdump.c

posio.o: posio.c
	cc $(CFLAGS) -c posio.c

time.o: time.c
	cc $(CFLAGS) -c time.c

die.o: die.c
	cc $(CFLAGS) -c die.c

ecpg.o: ecpg.c
	cc $(CFLAGS) -I$(PGINC) -c ecpg.c

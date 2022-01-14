#
#  Synopsis:
#	Makefile for populating $JMSCOTT_ROOT/lib/clang.
#  Note:
#	Should a copy of *.c files go to $JMSCOTT_ROOT/src ?
#
include ../local.mk
include ../jmscott.mk

install:
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)/include
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m u=rwx,go=rx	\
		-d $(JMSCOTT_PREFIX)/include/jmscott
	install -g $(INSTALL_GROUP) -o $(INSTALL_USER) -m ugo=xr	\
		die.c							\
		ecpg.c							\
		halloc.c						\
		hexdump.c						\
		posio.c							\
		string.c						\
		$(JMSCOTT_PREFIX)/include/jmscott

distclean:
	rm -rf $(JMSCOTT_PREFIX)/include/jmscott

world:
	$(MAKE) $(MFLAGS) distclean
	$(MAKE) $(MFLAGS) install

#
#  Synopsis:
#	Development or Instalation Makefile
#

include local.mk

DIST_ROOT?=$(PREFIX)/jmscott

HTTPD2_ROOT?=$(DIST_ROOT)/httpd2
HTTPD2_LIB?=$(HTTPD2_ROOT)/lib

COMPILED=	env env.d/help.pl env.d/env.xml \
	form form.d/help.pl file.d/file.xml

distclean:
	rm -rf $(HTTPD2_LIB)
install: all
	install -g $(DIST_GROUP) -o $(DIST_USER)			\
		-d $(DIST_ROOT)
	install -g $(DIST_GROUP) -o $(DIST_USER)			\
		-d $(HTTPD2_ROOT)
	install -g $(DIST_GROUP) -o $(DIST_USER)			\
		-d $(HTTPD2_LIB)
	install -g $(DIST_GROUP) -o $(DIST_USER)			\
		-d $(HTTPD2_LIB)/httpd2.d
	install -g $(DIST_GROUP) -o $(DIST_USER)			\
		-d $(HTTPD2_LIB)/env.d

	install -g $(DIST_GROUP) -o $(DIST_USER) -m ug=r,o=		\
		local-macosx.mk.example					\
		env.cgi							\
		httpd2.cgi						\
		$(HTTPD2_LIB)

all: $(COMPILED)

clean:
	rm -f $(COMPILED)

dev-links:
	test ! -s lib && ln -s . lib;		true

#
#  Dump the process environment for debugging.
#
env env.d/help.pl: env.cgi
	cgi2perl5 --source-path env.cgi

#
#  Dump contents of a POSTED form during debuging.
#
form form.d/help.pl: form.cgi
	cgi2perl5 --source-path form.cgi

dist-lib:
	make-dist dist-lib.conf

#
#  Synopsis:
#	Development or Instalation Makefile
#

include ../local.mk
include ../jmscott.mk

COMPILED=	env env.d/help.pl 					\
		form form.d/help.pl

all: $(COMPILED)

distclean:
	rm -f $(INSTALL_PREFIX)/lib/env.cgi
	rm -rf $(INSTALL_PREFIX)/lib/env.d
	rm -rf $(INSTALL_PREFIX)/cgi-bin/env

	rm -f $(INSTALL_PREFIX)/lib/form.cgi
	rm -rf $(INSTALL_PREFIX)/lib/form.d
	rm -rf $(INSTALL_PREFIX)/cgi-bin/form

	rm -rf $(INSTALL_PREFIX)/lib/httpd2.d
	rm -f $(INSTALL_PREFIX)/bin/cgi-inherit
	rm -f $(INSTALL_PREFIX)/bin/cgi-inherit-merge
	rm -f $(INSTALL_PREFIX)/bin/cgi-manifest
	rm -f $(INSTALL_PREFIX)/bin/cgi-template
	rm -f $(INSTALL_PREFIX)/bin/cgi2perl5

install: all
	install -g $(DIST_GROUP) -o $(DIST_USER)			\
		-d $(INSTALL_PREFIX)
	install -g $(DIST_GROUP) -o $(DIST_USER)			\
		-d $(INSTALL_PREFIX)/bin
	install -g $(DIST_GROUP) -o $(DIST_USER)			\
		-d $(INSTALL_PREFIX)/cgi-bin
	install -g $(DIST_GROUP) -o $(DIST_USER)			\
		-d $(INSTALL_PREFIX)/lib
	install -g $(DIST_GROUP) -o $(DIST_USER)			\
		-d $(INSTALL_PREFIX)/lib/httpd2.d
	install -g $(DIST_GROUP) -o $(DIST_USER)			\
		-d $(INSTALL_PREFIX)/lib/env.d
	install -g $(DIST_GROUP) -o $(DIST_USER)			\
		-d $(INSTALL_PREFIX)/lib/form.d

	install -g $(DIST_GROUP) -o $(DIST_USER) -m ugo=r		\
		local-linux.mk.example					\
		local-macosx.mk.example					\
		env.cgi							\
		form.cgi						\
		httpd2.cgi						\
		$(INSTALL_PREFIX)/lib

	install -g $(DIST_GROUP) -o $(DIST_USER) -m ugo=r		\
		env.d/div.pl						\
		env.d/dl.pl						\
		env.d/help.pl						\
		env.d/input.pl						\
		env.d/text.pl						\
		$(INSTALL_PREFIX)/lib/env.d

	install -g $(DIST_GROUP) -o $(DIST_USER) -m ugo=r		\
		httpd2.d/GET.pl						\
		httpd2.d/POST.pl					\
		httpd2.d/common.pl					\
		httpd2.d/make-work-dir.pl				\
		httpd2.d/multipart-form-data.pl				\
		httpd2.d/system.pl					\
		httpd2.d/www-form-urlencoded.pl				\
		$(INSTALL_PREFIX)/lib/httpd2.d

	install -g $(DIST_GROUP) -o $(DIST_USER) -m ugo=r		\
		form.d/post.dump.pl					\
		form.d/religion.pl					\
		form.d/help.pl						\
		$(INSTALL_PREFIX)/lib/form.d

	install -g $(DIST_GROUP) -o $(DIST_USER) -m ugo=r		\
		env.d/div.pl						\
		env.d/dl.pl						\
		env.d/help.pl						\
		env.d/input.pl						\
		env.d/text.pl						\
		$(INSTALL_PREFIX)/lib/env.d

	install -g $(DIST_GROUP) -o $(DIST_USER) -m u=rwx,go=rx		\
		cgi-inherit						\
		cgi-inherit-merge					\
		cgi-manifest						\
		cgi-template						\
		cgi2perl5						\
		$(INSTALL_PREFIX)/bin

	install -g $(DIST_GROUP) -o $(DIST_USER) -m u=rwx,go=rx		\
		env							\
		form							\
		$(INSTALL_PREFIX)/cgi-bin

clean:
	rm -f $(COMPILED)

dev-links:
	test ! -s lib && ln -s . lib;		true

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

dist-lib:
	make-dist dist-lib.conf

world: clean all distclean install

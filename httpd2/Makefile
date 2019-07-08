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
	rm -f $(DIST_ROOT)/lib/env.cgi
	rm -rf $(DIST_ROOT)/lib/env.d
	rm -rf $(DIST_ROOT)/cgi-bin/env

	rm -f $(DIST_ROOT)/lib/form.cgi
	rm -rf $(DIST_ROOT)/lib/form.d
	rm -rf $(DIST_ROOT)/cgi-bin/form

	rm -rf $(DIST_ROOT)/lib/httpd2.d
	rm -f $(DIST_ROOT)/bin/cgi-inherit
	rm -f $(DIST_ROOT)/bin/cgi-inherit-merge
	rm -f $(DIST_ROOT)/bin/cgi-manifest
	rm -f $(DIST_ROOT)/bin/cgi-template
	rm -f $(DIST_ROOT)/bin/cgi2perl5

install: all
	install -g $(DIST_GROUP) -o $(DIST_USER)			\
		-d $(DIST_ROOT)
	install -g $(DIST_GROUP) -o $(DIST_USER)			\
		-d $(DIST_ROOT)/bin
	install -g $(DIST_GROUP) -o $(DIST_USER)			\
		-d $(DIST_ROOT)/cgi-bin
	install -g $(DIST_GROUP) -o $(DIST_USER)			\
		-d $(DIST_ROOT)/lib
	install -g $(DIST_GROUP) -o $(DIST_USER)			\
		-d $(DIST_ROOT)/lib/httpd2.d
	install -g $(DIST_GROUP) -o $(DIST_USER)			\
		-d $(DIST_ROOT)/lib/env.d
	install -g $(DIST_GROUP) -o $(DIST_USER)			\
		-d $(DIST_ROOT)/lib/form.d

	install -g $(DIST_GROUP) -o $(DIST_USER) -m ugo=r		\
		local-linux.mk.example					\
		local-macosx.mk.example					\
		env.cgi							\
		form.cgi						\
		httpd2.cgi						\
		$(DIST_ROOT)/lib

	install -g $(DIST_GROUP) -o $(DIST_USER) -m ugo=r		\
		env.d/div.pl						\
		env.d/dl.pl						\
		env.d/help.pl						\
		env.d/input.pl						\
		env.d/text.pl						\
		$(DIST_ROOT)/lib/env.d

	install -g $(DIST_GROUP) -o $(DIST_USER) -m ugo=r		\
		httpd2.d/GET.pl						\
		httpd2.d/POST.pl					\
		httpd2.d/common.pl					\
		httpd2.d/make-work-dir.pl				\
		httpd2.d/multipart-form-data.pl				\
		httpd2.d/system.pl					\
		httpd2.d/www-form-urlencoded.pl				\
		$(DIST_ROOT)/lib/httpd2.d

	install -g $(DIST_GROUP) -o $(DIST_USER) -m ugo=r		\
		form.d/post.dump.pl					\
		form.d/religion.pl					\
		form.d/help.pl						\
		$(DIST_ROOT)/lib/form.d

	install -g $(DIST_GROUP) -o $(DIST_USER) -m ugo=r		\
		env.d/div.pl						\
		env.d/dl.pl						\
		env.d/help.pl						\
		env.d/input.pl						\
		env.d/text.pl						\
		$(DIST_ROOT)/lib/env.d

	install -g $(DIST_GROUP) -o $(DIST_USER) -m u=rwx,go=rx		\
		cgi-inherit						\
		cgi-inherit-merge					\
		cgi-manifest						\
		cgi-template						\
		cgi2perl5						\
		$(DIST_ROOT)/bin

	install -g $(DIST_GROUP) -o $(DIST_USER) -m u=rwx,go=rx		\
		env							\
		form							\
		$(DIST_ROOT)/cgi-bin

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

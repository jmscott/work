#
#  Synopsis:
#	Standalone install of $JMSCOTT_ROOT/www
#

include ../local.mk
include ../jmscott.mk

WWW_PREFIX?=$(JMSCOTT_PREFIX)/www
WWW_USER?=$(INSTALL_USER)
WWW_GROUP?=$(INSTALL_GROUP)

COMPILED=								\
		bust-http-rfc2388					\
		env env.d/help.pl 					\
		form form.d/help.pl

all: $(COMPILED)

distclean:
	rm -rf $(WWW_PREFIX)/lib
	rm -rf $(WWW_PREFIX)/bin
	rm -rf $(WWW_PREFIX)/cgi-bin

install: all
	install -g $(WWW_GROUP) -o $(WWW_USER)				\
		-d $(JMSCOTT_PREFIX)
	install -g $(WWW_GROUP) -o $(WWW_USER)				\
		-d $(JMSCOTT_PREFIX)/bin

	install -g $(WWW_GROUP) -o $(WWW_USER)				\
		-d $(WWW_PREFIX)
	install -g $(WWW_GROUP) -o $(WWW_USER)				\
		-d $(WWW_PREFIX)/bin
	install -g $(WWW_GROUP) -o $(WWW_USER)				\
		-d $(WWW_PREFIX)/cgi-bin
	install -g $(WWW_GROUP) -o $(WWW_USER)				\
		-d $(WWW_PREFIX)/src
	install -g $(WWW_GROUP) -o $(WWW_USER)				\
		-d $(WWW_PREFIX)/lib

	#
	#  create the symlink, so in apache
	#
	#	PERL5LIB=$(WWW_PREFIX)/lib:$PERL5LIB
	#
	#  and in perl code, namespace jmscott code via
	#
	#	require jmscott/dbi-pg.pl
	#
	test -h $(WWW_PREFIX)/lib/jmscott				||\
		ln -s $(WWW_PREFIX)/lib $(WWW_PREFIX)/lib/jmscott
	install -g $(WWW_GROUP) -o $(WWW_USER)				\
		-d $(WWW_PREFIX)/lib/env.d
	install -g $(WWW_GROUP) -o $(WWW_USER)				\
		-d $(WWW_PREFIX)/lib/form.d
	install -g $(WWW_GROUP) -o $(WWW_USER)				\
		-d $(WWW_PREFIX)/lib/httpd2.d

	install -g $(WWW_GROUP) -o $(WWW_USER) -m ugo=r			\
		common-json.pl						\
		dbi-pg.pl						\
		env.cgi							\
		form.cgi						\
		httpd2.cgi						\
		local-linux.mk.example					\
		local-macosx.mk.example					\
		property.pl						\
		$(WWW_PREFIX)/lib

	install -g $(WWW_GROUP) -o $(WWW_USER) -m ugo=r			\
		httpd2.d/GET.pl						\
		httpd2.d/POST.pl					\
		httpd2.d/common.pl					\
		httpd2.d/make-work-dir.pl				\
		httpd2.d/multipart-form-data.pl				\
		httpd2.d/system.pl					\
		httpd2.d/www-form-urlencoded.pl				\
		$(WWW_PREFIX)/lib/httpd2.d

	install -g $(WWW_GROUP) -o $(WWW_USER) -m ugo=r			\
		form.d/post.dump.pl					\
		form.d/religion.pl					\
		form.d/help.pl						\
		$(WWW_PREFIX)/lib/form.d

	install -g $(WWW_GROUP) -o $(WWW_USER) -m ugo=r			\
		env.d/dl.pl						\
		env.d/help.pl						\
		env.d/input.pl						\
		env.d/text.pl						\
		$(WWW_PREFIX)/lib/env.d

	install -g $(WWW_GROUP) -o $(WWW_USER) -m u=rwx,go=rx		\
		cgi-inherit						\
		cgi-inherit-merge					\
		cgi-manifest						\
		cgi-template						\
		cgi2perl5						\
		$(JMSCOTT_PREFIX)/bin

	install -g $(WWW_GROUP) -o $(WWW_USER) -m u=rwx,go=rx		\
		env							\
		form							\
		$(WWW_PREFIX)/cgi-bin

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

bust-http-rfc2388: bust-http-rfc2388.go
	$(GODIST)/bin/go build bust-http-rfc2388.go

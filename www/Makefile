#
#  Synopsis:
#	Standalone install of $JMSCOTT_ROOT/www
#  Note:
#	Convert configs to to make-make.
#
#	the process env JMSCOTT_COMPILE_WWW=1 determines if various
#	www tools are installed.  to disable build set JMSCOTT_COMPILE_WWW=0.
#	note that even when disable the "clean" and "distclean" still remove
#	files.
#

include ../local.mk
include ../jmscott.mk

WWW_PREFIX?=$(JMSCOTT_PREFIX)/www
WWW_USER?=$(INSTALL_USER)
WWW_GROUP?=$(INSTALL_GROUP)

COMPILED=								\
		rfc7578_2json						\
		env env.d/help.pl 					\
		form form.d/help.pl

ifeq ($(JMSCOTT_COMPILE_WWW),1)
all: $(COMPILED)
else
all:
	@echo 'all www: JMSCOTT_COMPILE_WWW !=1: skipping (ok)'
	@exit 0
endif

distclean:
	rm -rf $(WWW_PREFIX)/src
	rm -rf $(WWW_PREFIX)/lib
	rm -rf $(WWW_PREFIX)/bin
	rm -rf $(WWW_PREFIX)/cgi-bin

ifeq ($(JMSCOTT_COMPILE_WWW),1)
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
	install -g $(WWW_GROUP) -o $(WWW_USER)				\
		-d $(WWW_PREFIX)/bin
	install -g $(WWW_GROUP) -o $(WWW_USER)				\
		-d $(WWW_PREFIX)/htdocs

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
	install -g $(WWW_GROUP) -o $(WWW_USER) -m ugo=rx		\
		rfc7578_2json						\
		$(WWW_PREFIX)/bin
	install -g $(WWW_GROUP) -o $(WWW_USER) -m ugo=rx		\
		env.shtml						\
		footer.shtml						\
		header.shtml						\
		index.shtml						\
		$(WWW_PREFIX)/htdocs

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
else
install:
	@echo 'install www: JMSCOTT_COMPILE_WWW !=1: skipping (ok)'
	@exit 0
endif

clean:
	rm -f $(COMPILED)

dev-links:
	test -s lib || ln -s . lib
	test -s htdocs || ln -s . htdocs
	test -s cgi-bin || ln -s . cgi-bin
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

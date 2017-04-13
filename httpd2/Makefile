#
#  Synopsis:
#	Development or Instalation Makefile
#

include local.mk

DIST_ROOT?=$(PREFIX)/jmscott

HTTPD2_ROOT?=$(DIST_ROOT)/httpd2
HTTPD2_LIB?=$(HTTPD2_ROOT)/lib

COMPILED=	env env.d/help.pl 					\
		form form.d/help.pl
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
	install -g $(DIST_GROUP) -o $(DIST_USER)			\
		-d $(HTTPD2_LIB)/form.d

	install -g $(DIST_GROUP) -o $(DIST_USER) -m ugo=r		\
		local-linux.mk.example					\
		local-macosx.mk.example					\
		env.cgi							\
		form.cgi						\
		httpd2.cgi						\
		$(HTTPD2_LIB)

	install -g $(DIST_GROUP) -o $(DIST_USER) -m ugo=r		\
		env.d/div.pl						\
		env.d/dl.pl						\
		env.d/help.pl						\
		env.d/input.pl						\
		env.d/text.pl						\
		$(HTTPD2_LIB)/env.d

	install -g $(DIST_GROUP) -o $(DIST_USER) -m ugo=r		\
		httpd2.d/GET.pl						\
		httpd2.d/POST.pl					\
		httpd2.d/common.pl					\
		httpd2.d/make-work-dir.pl				\
		httpd2.d/multipart-form-data.pl				\
		httpd2.d/system.pl					\
		httpd2.d/www-form-urlencoded.pl				\
		$(HTTPD2_LIB)/httpd2.d

	install -g $(DIST_GROUP) -o $(DIST_USER) -m ugo=r		\
		form.d/post.dump.pl					\
		form.d/religion.pl					\
		$(HTTPD2_LIB)/httpd2.d

all: $(COMPILED)

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

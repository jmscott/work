#
#  Synopsis:
#	Development or Instalation Makefile
#

include local.mk

DIST_ROOT=$(PREFIX)/jmscott
HTTPD2_ROOT=$(DIST_ROOT)/httpd2

CGI=	env env.d/help.pl env.d/env.xml \
	file file.d/help.pl file.d/file.xml \
	form form.d/help.pl file.d/file.xml \
	manifest manifest.d/help.pl manifest.d/manifest.xml
NOW="`date`"
CGI2PERL5=cgi2perl5 \
		--when $(NOW)

install: all
	install -g $(DIST_GROUP) -o $(DIST_USER)			\
		-d $(DIST_ROOT)
	install -g $(DIST_GROUP) -o $(DIST_USER)			\
		-d $(HTTPD2_ROOT)
	install -g $(DIST_GROUP) -o $(DIST_USER)			\
		-d $(HTTPD2_ROOT)/httpd2.d
	install -g $(DIST_GROUP) -o $(DIST_USER)			\
		-d $(HTTPD2_ROOT)/env.d

	install -g $(DIST_GROUP) -o $(DIST_USER) -m ug=r,o=		\
		local-macosx.mk.example					\
		env.cgi							\
		httpd2.cgi						\
		$(HTTPD2_ROOT)

	
all: $(CGI)
clean-cgi:
	rm -f $(CGI)
find-cgi:
	@find $(CGI) -print | fgrep -v /
clean:
	make clean-cgi
clean-links:
	rm -f htdocs cgi-bin lib bin etc
links:
	test ! -s htdocs && ln -s . htdocs;	true
	test ! -s cgi-bin && ln -s . cgi-bin;	true
	test ! -s lib && ln -s . lib;		true
	test ! -s bin && ln -s . bin;		true
	test ! -s etc && ln -s . etc;		true

#
#  env
#
env env.d/help.pl: env.d/env.xml
	cgi2perl5 --source-path $<
env.d/env.xml: env.cgi
	cgi-inherit $< httpd2.cgi >env.d/env.xml

#
#  file
#
file file.d/help.pl: file.d/file.xml
	cgi2perl5 \
		--source-path $<
file.d/file.xml: file.cgi
	cgi-inherit $< httpd2.cgi >file.d/file.xml
#
#  form
#
form form.d/help.pl: form.d/form.xml
	cgi2perl5 \
		--source-path $<
form.d/form.xml: form.cgi
	cgi-inherit $< httpd2.cgi >form.d/form.xml

#
#  manifest
#
manifest manifest.d/help.pl: manifest.d/manifest.xml
	cgi2perl5 \
		--source-path $<
manifest.d/manifest.xml: manifest.cgi
	cgi-inherit $< httpd2.cgi >manifest.d/manifest.xml

dist-lib:
	make-dist dist-lib.conf

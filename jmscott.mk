#
#  Synopsis:
#	Static make rules for jmscott work tools.
#
UNAME=$(shell uname)

#  Strict compilation flags

CFLAGS?=-Wall -Wextra -Werror
CLINK?=-L. -Lclang -ljmscott

CCBUILD?=cc $(CFLAGS) $(CLINK) 

ifeq "$(UNAME)" "Linux"
	CFLAGS+=-std=gnu99
endif

#  Setting SHELL forces gmake to disable optimizing of single line productions,
#  forcing make to honor $PATH.  Also, overrides ubuntu20 legacy /bin/sh
#  (unlike fedora).

SHELL=/bin/bash

INSTALL_USER?=$(USER)
ifeq "$(UNAME)" "Darwin"
	INSTALL_GROUP?=staff
else
	INSTALL_GROUP?=$(USER)
endif

JMSCOTT_PREFIX?=$(INSTALL_PREFIX)/jmscott

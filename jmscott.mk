#
#  Synopsis:
#	Static make rules for jmscott work tools.
#

#  Note: why set SHELL unbder linux?
SHELL=/bin/bash

UNAME=$(shell uname)

#  Strict compilation flags

CFLAGS?=-Wall -Wextra -Werror
CLINK?=-L. -Lclib -ljmscott

CCOMPILE?=cc $(CFLAGS)

ifeq "$(UNAME)" "Linux"
	CFLAGS+=-std=gnu99
endif

INSTALL_USER?=$(USER)
ifeq "$(UNAME)" "Darwin"
	INSTALL_GROUP?=staff
else
	INSTALL_GROUP?=$(USER)
endif

JMSCOTT_PREFIX?=$(INSTALL_PREFIX)/jmscott

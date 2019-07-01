#
#  Synopsis:
#	Static make rules for jmscott work tools.
#
UNAME=$(shell uname)

#  Strict compilation flags

CFLAGS?=-Wall -Wextra -Werror

ifeq "$(UNAME)" "Linux"
	CFLAGS+=-std=gnu99
endif

#  Setting SHELL forces gmake to disable optimizing of single line productions,
#  forcing make to honor $PATH

SHELL?=/bin/bash

DIST_USER?=$(USER)
ifeq "$(UNAME)" "Darwin"
	DIST_GROUP?=staff
else
	DIST_GROUP?=$(USER)
endif

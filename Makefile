#
# Cross platform Makefile
#

TARGETS = all build clean install debug play

$(TARGETS):
	make -f Makefile.`uname` $@


#
# Makefile for the Adventure interpreter.
# 	(c) Ken Wellsch 1985
# Modified for Atari ST make B. Bray -- 86/09/17
#

BIN = \bin
INCLUDE =\lclib
SHELL = \sh.tos

adv.prg: adv.bin error.bin advglob.bin cache.bin command.bin condtio.bin \
 exec.bin process.bin savgm.bin say.bin showop.bin symtab.bin \
 util.bin values.bin webster.bin
	$BIN\link.ttp -with adv -nolist

clean:
	$SHELL rm *.bin

*.bin: *.c
	$BIN\lc1.ttp -i$INCLUDE\ -cw $*
	$BIN\lc2.ttp $*

adv.bin: adv.c adefs.h
error.bin: error.c
advglob.bin: advglob.c adefs.h
cache.bin: cache.c adefs.h
command.bin: command.c adefs.h
condtio.bin: condtio.c adefs.h
exec.bin: exec.c adefs.h
process.bin: process.c adefs.h
savgm.bin: savgm.c adefs.h
say.bin: say.c adefs.h
showop.bin: showop.c adefs.h
symtab.bin: symtab.c adefs.h
util.bin: util.c adefs.h
values.bin: values.c adefs.h
webster.bin: webster.c adefs.h


#
# Makefile for the Adventure Compiler.
# 	(c) Ken Wellsch 1985
#
BIN = \bin
SHELL = \sh.tos
INCLUDE =\lclib

munge.prg: munge.bin buffers.bin compile.bin fio.bin glob.bin \
 majors.bin parse.bin symtab.bin util.bin
	$BIN\link.ttp -with munge -nolist

clean:
	$SHELL rm *.bin

*.bin: *.c
	$BIN\lc1.ttp -i$INCLUDE\ -cw $*
	$BIN\lc2.ttp $*

munge.bin: munge.c
buffers.bin:buffers.c
compile.bin: compile.c
fio.bin: fio.c
glob.bin:glob.c
majors.bin: majors.c
parse.bin:parse.c
symtab.bin:symtab.c
util.bin:util.c


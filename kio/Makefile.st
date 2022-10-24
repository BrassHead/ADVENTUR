#
# Makefile for the Adventure interpreter.
# 	(c) Ken Wellsch 1985
# Modified for Atari ST make B. Bray -- 86/09/17
#

BIN = \bin
INCLUDE =\lclib
SHELL = \sh.tos

compile: kioglob.bin GetBlk.bin GetRec.bin MakBlk.bin MakNam.bin \
 MapBlk.bin MapKey.bin PutBlk.bin PutRec.bin \
 closek.bin creatk.bin dupk.bin openk.bin readk.bin writek.bin 

clean:
	$SHELL rm *.bin

*.bin: *.c
	$BIN\lc1.ttp -i$INCLUDE\ -cw $*
	$BIN\lc2.ttp $*

kioglob.bin: kioglob.c
GetBlk.bin: getblk.c
GetRec.bin: getrec.c
MakBlk.bin: makblk.c
MakNam.bin: maknam.c
MapBlk.bin: mapblk.c
MapKey.bin: mapkey.c
PutBlk.bin: putblk.c
PutRec.bin: putrec.c
closek.bin: closek.c
creatk.bin: creatk.c
dupk.bin: dupk.c
openk.bin: openk.c
readk.bin: readk.c
writek.bin: writek.c
 


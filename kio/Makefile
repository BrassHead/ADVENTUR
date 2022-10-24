#
# Makefile for the Adventure interpreter.
# 	(c) Ken Wellsch 1985
# Modified for Atari ST make B. Bray -- 86/09/17
# Modified for C7 Quicklib   B. Bray -- 92/05/16
#
# Usage:     NMAKE option (build)
#    or:     NMAKE clean  (erase all compiled files)
#
# option:    DEBUG=[0|1]  (DEBUG not defined is equivalent to DEBUG=0)
#
#	     Note upper case DEBUG.
#

CPPFLAGS= /AL /W3 /Zp /Mq /G2 /Za

!if "$(DEBUG)"=="1"
CPPFLAGS=/D_DEBUG $(CPPFLAGS) /Od /Zi /f
!else
CPPFLAGS=$(CPPFLAGS) /Oselg /Gs
!endif

OBJS = kioglob.obj GetBlk.obj GetRec.obj MakBlk.obj MakNam.obj \
	MapBlk.obj MapKey.obj PutBlk.obj PutRec.obj \
	closek.obj creatk.obj dupk.obj openk.obj readk.obj writek.obj 

kio.lib: $(OBJS)
	lib @<<
kio /noi
-+kioglob.obj -+GetBlk.obj -+GetRec.obj -+MakBlk.obj -+MakNam.obj &
-+MapBlk.obj -+MapKey.obj -+PutBlk.obj -+PutRec.obj &
-+closek.obj -+creatk.obj -+dupk.obj -+openk.obj -+readk.obj -+writek.obj ;
<<

clean:
	del *.obj
	del kio.lib

.c.obj:
	cl $(CPPFLAGS) /c $*.c

kioglob.obj: kioglob.c

GetBlk.obj: getblk.c

GetRec.obj: getrec.c

MakBlk.obj: makblk.c

MakNam.obj: maknam.c

MapBlk.obj: mapblk.c

MapKey.obj: mapkey.c

PutBlk.obj: putblk.c

PutRec.obj: putrec.c

closek.obj: closek.c

creatk.obj: creatk.c

dupk.obj: dupk.c

openk.obj: openk.c

readk.obj: readk.c

writek.obj: writek.c



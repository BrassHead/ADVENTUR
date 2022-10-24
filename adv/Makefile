#
# Makefile for the Adventure interpreter.
# 	(c) Ken Wellsch 1985
# Modified for Atari ST make B. Bray -- 86/09/17
# Modified for C7 Quickwin   B. Bray -- 92/05/13
#
# Usage:     NMAKE option (build)
#    or:     NMAKE clean  (erase all compiled files)
#
# option:    DEBUG=[0|1]  (DEBUG not defined is equivalent to DEBUG=0)
#
#	     Note upper case DEBUG.
#

CPPFLAGS= /AL /W3 /Zp /Mq /G2 /Za
LINKFLAGS=/NOD /NOI /ONERROR:NOEXE

!if "$(DEBUG)"=="1"
CPPFLAGS=/D_DEBUG $(CPPFLAGS) /Od /Zi /f
LINKFLAGS=$(LINKFLAGS) /COD
LIBS=llibcewq libw ..\kio\kio
!else
CPPFLAGS=$(CPPFLAGS) /Oselg /Gs
LINKFLAGS=$(LINKFLAGS)  
LIBS=llibcewq libw  ..\kio\kio
!endif

OBJS = adv.obj error.obj advglob.obj cache.obj command.obj condtio.obj \
 exec.obj process.obj savgm.obj say.obj showop.obj symtab.obj \
 util.obj values.obj webster.obj


adv.exe: $(OBJS)
	link $(LINKFLAGS) @<<
    $(OBJS),
    adv.exe,
    NUL,
    $(LIBS),
    adv.def
<<

clean:
	del *.obj
	del adv.exe

.c.obj:
	cl $(CPPFLAGS) /c $*.c

adv.obj: adv.c adefs.h

error.obj: error.c

advglob.obj: advglob.c adefs.h

cache.obj: cache.c adefs.h

command.obj: command.c adefs.h

condtio.obj: condtio.c adefs.h

exec.obj: exec.c adefs.h

process.obj: process.c adefs.h

savgm.obj: savgm.c adefs.h

say.obj: say.c adefs.h

showop.obj: showop.c adefs.h

symtab.obj: symtab.c adefs.h

util.obj: util.c adefs.h

values.obj: values.c adefs.h

webster.obj: webster.c adefs.h


#
# Makefile for the Adventure Compiler.
# 	(c) Ken Wellsch 1985
# Modified for C7  BrianBr  92/11/23
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

OBJS = munge.obj buffers.obj compile.obj fio.obj glob.obj \
 majors.obj parse.obj symtab.obj util.obj

.c.obj:
	cl $(CPPFLAGS) /c $*.c


munge.prg: $(OBJS)
	link $(LINKFLAGS) @<<
    $(OBJS),
    munge.exe,
    NUL,
    $(LIBS),
    munge.def
<<

clean:
	del *.obj
	del munge.exe

munge.obj: munge.c

buffers.obj: buffers.c

compile.obj: compile.c

fio.obj: fio.c

glob.obj: glob.c

majors.obj: majors.c

parse.obj: parse.c

symtab.obj: symtab.c

util.obj: util.c


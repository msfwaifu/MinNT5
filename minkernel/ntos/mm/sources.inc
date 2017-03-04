MAJORCOMP=ntos
MINORCOMP=mm

TARGETNAME=mm
TARGETTYPE=LIBRARY
TARGETPATH=obj

BUILD_PRODUCES=ntosmm$(NT_UP)

INCLUDES=..;..\..\inc;..\..\ke;$(BASE_INC_PATH);$(MINWIN_INC_PATH)

MIPS_OPTIONS=-nodwalign
GPSIZE=32

MSC_WARNING_LEVEL=/W4 /WX

C_DEFINES=$(C_DEFINES) -D_NTSYSTEM_

SOURCES=..\acceschk.c \
        ..\addrsup.c  \
        ..\allocpag.c \
        ..\allocvm.c  \
        ..\checkpfn.c \
        ..\checkpte.c \
        ..\creasect.c \
        ..\deleteva.c \
        ..\dmpaddr.c  \
        ..\extsect.c  \
        ..\flushbuf.c \
        ..\flushsec.c \
        ..\forksup.c  \
        ..\freevm.c   \
        ..\iosup.c    \
        ..\lockvm.c   \
        ..\mapcache.c \
        ..\mapview.c  \
        ..\miglobal.c \
        ..\mmfault.c  \
        ..\mminit.c   \
        ..\mmsup.c    \
        ..\mmquota.c  \
        ..\modwrite.c \
        ..\pagfault.c \
        ..\pfndec.c   \
        ..\pfnlist.c  \
        ..\procsup.c  \
        ..\protect.c  \
        ..\querysec.c \
        ..\queryvm.c  \
        ..\readwrt.c  \
        ..\sectsup.c  \
        ..\shutdown.c \
        ..\sysload.c  \
        ..\sysptes.c  \
        ..\umapview.c \
        ..\vadtree.c  \
        ..\wslist.c   \
        ..\wsmanage.c \
        ..\wstree.c   \
        ..\wrtfault.c \
        ..\zeropage.c

PRECOMPILED_INCLUDE=..\mi.h
PRECOMPILED_PCH=mi.pch
PRECOMPILED_OBJ=mi.obj

SOURCES_USED=..\sources.inc

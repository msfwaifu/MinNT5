MAJORCOMP=ntos
MINORCOMP=ps

TARGETNAME=ps
TARGETTYPE=LIBRARY
TARGETPATH=obj

BUILD_PRODUCES=ntosps$(NT_UP)

MSC_WARNING_LEVEL=/W4 /WX

INCLUDES=..;..\..\inc;..\..\ke;$(BASE_INC_PATH);$(MINWIN_INC_PATH)

C_DEFINES=$(C_DEFINES) -D_NTSYSTEM_

SOURCES=..\psspnd.c   \
        ..\pscid.c    \
        ..\psimpers.c \
        ..\psinit.c   \
        ..\psdelete.c \
        ..\psquery.c  \
        ..\create.c   \
        ..\kulookup.c \
        ..\psquota.c  \
        ..\psctx.c    \
        ..\psopen.c   \
        ..\security.c

PRECOMPILED_INCLUDE=..\psp.h
PRECOMPILED_PCH=psp.pch
PRECOMPILED_OBJ=psp.obj

UMTYPE=nt

SOURCES_USED=..\sources.inc

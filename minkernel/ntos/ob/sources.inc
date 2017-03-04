MAJORCOMP=ntos
MINORCOMP=ob

TARGETNAME=ob
TARGETTYPE=LIBRARY
TARGETPATH=obj

BUILD_PRODUCES=ntosob$(NT_UP)

INCLUDES=..;..\..\inc;$(BASE_INC_PATH);$(MINWIN_INC_PATH)

MIPS_OPTIONS=-nodwalign
GPSIZE=32

MSC_WARNING_LEVEL=/W4 /WX

C_DEFINES=$(C_DEFINES) -D_NTSYSTEM_

SOURCES=..\obinit.c   \
        ..\obcreate.c \
        ..\obhandle.c \
        ..\obinsert.c \
        ..\obref.c    \
        ..\obse.c     \
        ..\obtype.c   \
        ..\obdir.c    \
        ..\oblink.c   \
        ..\obclose.c  \
        ..\obquery.c  \
        ..\obsdata.c  \
        ..\obwait.c

PRECOMPILED_INCLUDE=..\obp.h
PRECOMPILED_PCH=obp.pch
PRECOMPILED_OBJ=obp.obj

OPTIONAL_NTTEST=tob
UMTEST=uob

SOURCES_USED=..\sources.inc

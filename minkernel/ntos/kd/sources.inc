MAJORCOMP=ntos
MINORCOMP=kd

TARGETNAME=kd
TARGETTYPE=LIBRARY
TARGETPATH=obj

BUILD_PRODUCES=ntoskd$(NT_UP)

INCLUDES=..;..\..\inc;..\..\ke;..\$(TARGET_DIRECTORY);$(BASE_INC_PATH);$(MINWIN_INC_PATH)

MSC_WARNING_LEVEL=/W4 /WX

C_DEFINES=$(C_DEFINES) -D_NTSYSTEM_

SOURCES=..\kdinit.c  \
        ..\kdbreak.c \
        ..\kdlock.c  \
        ..\kdmove.c  \
        ..\kddbgio.c \
        ..\kdapi.c   \
        ..\kdcomio.c \
        ..\kddata.c

PRECOMPILED_INCLUDE=..\kdp.h
PRECOMPILED_PCH=kdp.pch
PRECOMPILED_OBJ=kdp.obj

SOURCES_USED=..\sources.inc

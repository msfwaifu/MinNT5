MAJORCOMP=ntos
MINORCOMP=FsRtl

TARGETNAME=FsRtl
TARGETTYPE=LIBRARY
TARGETPATH=obj

BUILD_PRODUCES=ntosfsrtl$(NT_UP)

INCLUDES=..;..\..\inc;$(BASE_INC_PATH);$(MINWIN_INC_PATH)

GPSIZE=32

MSC_WARNING_LEVEL=/W4 /WX

C_DEFINES=$(C_DEFINES) -D_NTSYSTEM_

SOURCES=..\DbcsName.c \
        ..\FastIo.c   \
        ..\FaultTol.c \
        ..\FileLock.c \
        ..\Filter.c   \
        ..\FsRtlPc.c  \
        ..\LargeMcb.c \
        ..\Name.c     \
        ..\Notify.c   \
        ..\Oplock.c   \
        ..\Unc.c      \
        ..\Tunnel.c   \
        ..\StackOvf.c

PRECOMPILED_INCLUDE=..\fsrtlp.h
PRECOMPILED_PCH=fsrtlp.pch
PRECOMPILED_OBJ=fsrtlp.obj

SOURCES_USED=..\sources.inc

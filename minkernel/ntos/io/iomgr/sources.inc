MAJORCOMP=ntos
MINORCOMP=io

TARGETNAME=iomgr
TARGETTYPE=LIBRARY
TARGETPATH=obj

BUILD_PRODUCES=ntosiomgr$(NT_UP)

INCLUDES=..;..\..;..\..\..\inc;;$(BASE_INC_PATH);$(MINWIN_INC_PATH)

MIPS_OPTIONS=-nodwalign
GPSIZE=32

SOURCES=..\arcsec.c   \
        ..\complete.c \
        ..\create.c   \
        ..\devctrl.c  \
        ..\dir.c      \
        ..\dumpctl.c  \
        ..\errorlog.c \
        ..\filter.c   \
        ..\fsctrl.c   \
        ..\internal.c \
        ..\iodata.c   \
        ..\ioinit.c   \
        ..\iosubs.c   \
        ..\loadunld.c \
        ..\lock.c     \
        ..\misc.c     \
        ..\objsup.c   \
        ..\open.c     \
        ..\parse.c    \
        ..\qsea.c     \
        ..\qsfs.c     \
        ..\qsinfo.c   \
        ..\query.c    \
        ..\read.c     \
        ..\write.c

NTTEST=
OPTIONAL_NTTEST=tio

PRECOMPILED_INCLUDE=..\iomgr.h
PRECOMPILED_PCH=iomgr.pch
PRECOMPILED_OBJ=iomgr.obj

SOURCES_USED=..\sources.inc

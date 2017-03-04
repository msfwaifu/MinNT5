MAJORCOMP=ntos
MINORCOMP=io

TARGETNAME=pnpmgr
TARGETTYPE=LIBRARY
TARGETPATH=obj

BUILD_PRODUCES=ntospnpmgr$(NT_UP)

GPSIZE=32

INCLUDES=..;..\..;..\..\..\inc;..\..\..\ke;$(BASE_INC_PATH);$(MINWIN_INC_PATH)

SOURCES=..\assign.c   \
        ..\busenum.c  \
        ..\control.c  \
        ..\devices.c  \
        ..\enumsys.c  \
        ..\internal.c \
        ..\pnpdata.c  \
        ..\pnpdd.c    \
        ..\pnpevent.c \
        ..\pnpinit.c  \
        ..\pnpioapi.c \
        ..\pnpsubs.c  \
        ..\report.c   \

NTTEST=

PRECOMPILED_INCLUDE=..\pnpmgrp.h
PRECOMPILED_PCH=pnpmgrp.pch
PRECOMPILED_OBJ=pnpmgrp.obj

SOURCES_USED=..\sources.inc

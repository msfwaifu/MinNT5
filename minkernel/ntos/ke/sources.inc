MAJORCOMP=ntos
MINORCOMP=ke

TARGETNAME=ke
TARGETTYPE=LIBRARY
TARGETPATH=obj

BUILD_PRODUCES=ntoske$(NT_UP)

INCLUDES=..;..\..\inc;$(BASE_INC_PATH);$(MINWIN_INC_PATH)

MSC_WARNING_LEVEL=/W4 /WX

C_DEFINES=$(C_DEFINES) -D_NTSYSTEM_

SOURCES=..\apcobj.c   \
        ..\apcsup.c   \
        ..\balmgr.c   \
        ..\bugcheck.c \
        ..\channel.c  \
        ..\config.c   \
        ..\debug.c    \
        ..\devquobj.c \
        ..\dpcobj.c   \
        ..\dpcsup.c   \
        ..\eventobj.c \
        ..\kernldat.c \
        ..\kiinit.c   \
        ..\miscc.c    \
        ..\mutntobj.c \
        ..\procobj.c  \
        ..\profobj.c  \
        ..\queueobj.c \
        ..\raisexcp.c \
        ..\semphobj.c \
        ..\suspend.c  \
        ..\thredobj.c \
        ..\thredsup.c \
        ..\timerobj.c \
        ..\timersup.c \
        ..\wait.c     \
        ..\waitsup.c  \
        ..\xipi.c     \
        ..\yield.c

NTTARGETFILES=

PRECOMPILED_INCLUDE=..\ki.h
PRECOMPILED_PCH=ki.pch
PRECOMPILED_OBJ=ki.obj

SOURCES_USED=..\sources.inc

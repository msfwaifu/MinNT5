MAJORCOMP=ntos
MINORCOMP=ex

TARGETNAME=ex
TARGETTYPE=LIBRARY
TARGETPATH=obj

BUILD_PRODUCES=ntosex$(NT_UP)

INCLUDES=..;..\..\inc;..\..\ke;$(BASE_INC_PATH);$(MINWIN_INC_PATH)

MSC_WARNING_LEVEL=/W4 /WX

C_DEFINES=$(C_DEFINES) -D_NTSYSTEM_

SOURCES=..\callback.c  \
        ..\callperf.c  \
        ..\dbgctrl.c   \
        ..\ddkresrc.c  \
        ..\delay.c     \
        ..\event.c     \
        ..\eventpr.c   \
        ..\exatom.c    \
        ..\exdata.c    \
        ..\exinfo.c    \
        ..\exinit.c    \
        ..\exdata.c    \
        ..\handle.c    \
        ..\harderr.c   \
        ..\lookasid.c  \
        ..\luid.c      \
        ..\memprint.c  \
        ..\mutant.c    \
        ..\pool.c      \
        ..\profile.c   \
        ..\raise.c     \
        ..\resource.c  \
        ..\semphore.c  \
        ..\sysenv.c    \
        ..\sysevent.c  \
        ..\sysinfo.c   \
        ..\systime.c   \
        ..\timer.c     \
        ..\worker.c    \
        ..\zone.c      \
        ..\uuid.c      \
        ..\win32.c

PRECOMPILED_INCLUDE=..\exp.h
PRECOMPILED_PCH=exp.pch
PRECOMPILED_OBJ=exp.obj

SOURCES_USED=..\sources.inc

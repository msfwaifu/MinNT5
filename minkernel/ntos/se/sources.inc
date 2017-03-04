MAJORCOMP=ntos
MINORCOMP=se

TARGETNAME=se
TARGETTYPE=LIBRARY
TARGETPATH=obj

BUILD_PRODUCES=ntosse$(NT_UP)

MIPS_OPTIONS=-nodwalign
GPSIZE=32

INCLUDES=..;..\..\inc;$(DS_INC_PATH);$(BASE_INC_PATH);$(MINWIN_INC_PATH)

MSC_WARNING_LEVEL=/W4 /WX

C_DEFINES=-D_NTSYSTEM_

SOURCES=              \
        ..\accessck.c \
        ..\capture.c  \
        ..\privileg.c \
        ..\rmlogon.c  \
        ..\rmmain.c   \
        ..\rmvars.c   \
        ..\seassign.c \
        ..\seaudit.c  \
        ..\sepaudit.c \
        ..\seclient.c \
        ..\seglobal.c \
        ..\seinit.c   \
        ..\semethod.c \
        ..\sep.c      \
        ..\subject.c  \
        ..\seastate.c \
        ..\token.c    \
        ..\tokenadj.c \
        ..\tokendup.c \
        ..\tokenopn.c \
        ..\tokenqry.c \
        ..\tokenset.c \
        ..\adtlog.c   \
        ..\adtinit.c  \
        ..\adtvars.c  \
        ..\rmaudit.c

SOURCES_USED=..\sources.inc

! IF 0

UMTEST=uttoken*utrtl*utseacc*utseqos*utaccess
UMTYPE=console
UMLIBS=$(SDK_LIB_PATH)\ntdll.lib

!ENDIF

MAJORCOMP=ntos
MINORCOMP=raw

TARGETNAME=raw
TARGETTYPE=LIBRARY
TARGETPATH=obj

BUILD_PRODUCES=ntosraw$(NT_UP)

INCLUDES=..;..\..\inc;$(BASE_INC_PATH);$(MINWIN_INC_PATH)

MIPS_OPTIONS=-nodwalign

MSC_WARNING_LEVEL=/W4 /WX

C_DEFINES=$(C_DEFINES) -D_NTSYSTEM_

SOURCES=..\Cleanup.c  \
        ..\Close.c    \
        ..\Create.c   \
        ..\FileInfo.c \
        ..\FsCtrl.c   \
        ..\StrucSup.c \
        ..\RawDisp.c  \
        ..\RawInit.c  \
        ..\ReadWrit.c \
        ..\VolInfo.c

SOURCES_USED=..\sources.inc

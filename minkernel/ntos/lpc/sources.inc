MAJORCOMP=ntos
MINORCOMP=lpc

TARGETNAME=lpc
TARGETTYPE=LIBRARY
TARGETPATH=obj

BUILD_PRODUCES=ntoslpc$(NT_UP)

INCLUDES=..;..\..\inc;$(BASE_INC_PATH);$(MINWIN_INC_PATH)

C_DEFINES=$(C_DEFINES) -D_NTSYSTEM_

SOURCES=..\lpcinit.c  \
        ..\lpccreat.c \
        ..\lpclistn.c \
        ..\lpcconn.c  \
        ..\lpccompl.c \
        ..\lpcsend.c  \
        ..\lpcrecv.c  \
        ..\lpcreply.c \
        ..\lpcquery.c \
        ..\lpcqueue.c \
        ..\lpcclose.c \
        ..\lpcpriv.c

PRECOMPILED_INCLUDE=..\lpcp.h
PRECOMPILED_PCH=lpcp.pch
PRECOMPILED_OBJ=lpcp.obj

UMTYPE=console
UMTEST=userver*uclient

NTTARGETFILES=

SOURCES_USED=..\sources.inc

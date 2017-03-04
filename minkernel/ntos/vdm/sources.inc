MAJORCOMP=ntos
MINORCOMP=vdm

TARGETNAME=vdm
TARGETTYPE=LIBRARY
TARGETPATH=obj

BUILD_PRODUCES=ntosvdm$(NT_UP)

MIPS_OPTIONS=-nodwalign
GPSIZE=32

INCLUDES=..;..\..\inc;$(BASE_INC_PATH);$(MINWIN_INC_PATH)

!IF $(X86)
INCLUDES=$(INCLUDES);..\x86
!ENDIF

MSC_WARNING_LEVEL=/W4 /WX

C_DEFINES=$(C_DEFINES) -D_NTSYSTEM_

SOURCES=..\vdm.c

X86_SOURCES=   ..\x86\vdmentry.c         \
               ..\x86\vdminit.c          \
               ..\x86\vdmnpx.c           \
               ..\x86\vdmop0f.asm        \
               ..\x86\vdmoprnd.asm       \
               ..\x86\vdmmisc.asm        \
               ..\x86\vdmfault.c         \
               ..\x86\vdmprint.c         \
               ..\x86\strtexec.c         \
               ..\x86\rdwr.c             \
               ..\x86\vdmints.c

UMTYPE=console

SOURCES_USED=..\sources.inc

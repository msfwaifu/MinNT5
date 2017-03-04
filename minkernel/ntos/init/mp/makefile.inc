$(O)\ntoskrnl.prf: $(TARGET_DIRECTORY).prf
    copy $(TARGET_DIRECTORY).prf ..\ntkrnlmp.prf

!INCLUDE ..\up\makefile.inc

$(_OBJ_DIR)\alpha\ntkrnlmp.obj: ..\ntoskrnl.c

$(_OBJ_DIR)\x86\ntkrnlmp.obj: ..\ntoskrnl.c

$(_OBJ_DIR)\mips\ntkrnlmp.obj: ..\ntoskrnl.c

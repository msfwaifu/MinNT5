theap.c: ..\heap.c ..\heapdbg.c ..\heapdll.c ..\trace.c

t.c: ..\handle.c ..\atom.c

..\error.c: ..\error.h

..\error.h: ..\generr.c
    set PASS0ONLY=
    nmake $(O)\generr.obj USE_CRTDLL=1
    -link -out:$(O)\generr.exe @<<
-machine:$(TARGET_DIRECTORY)
-base:@$(COFFBASE_TXT_FILE),usermode
-subsystem:console
-entry:mainCRTStartup
-ignore:4001,4037,4039,4065,4070,4078,4087,4089
-force
-nodefaultlib
$(O)\generr.obj
$(SDK_LIB_PATH)\crtdll.lib
<<
    -$(O)\generr.exe $@
    -erase $(O)\error.obj
    -erase $(O)\generr.obj
    -erase $(O)\generr.exe

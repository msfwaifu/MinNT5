!IF 0

Copyright (c) 1989-2000  Microsoft Corporation

!ENDIF

!include $(_PROJECT_MK_PATH)\..\project.mk

MSC_WARNING_LEVEL=/W3 /WX

INCLUDES=$(_PROJECT_MK_PATH)\..\ntos\inc;$(_PROJECT_MK_PATH)\..\ntos\ke;$(_PROJECT_MK_PATH)\..\ntos\io;$(HALKIT_INC_PATH);$(BASE_INC_PATH);$(MINWIN_INC_PATH)

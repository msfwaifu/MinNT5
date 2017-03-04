/*++ BUILD Version: 0002    // Increment this if a change has global effects

Copyright (c) 1989  Microsoft Corporation

Module Name:

    init.h

Abstract:

    Header file for the INIT subcomponent of NTOS

Author:

    Steve Wood (stevewo) 31-Mar-1989

Revision History:

--*/

#ifndef _INIT_
#define _INIT_

#define INIT_SYSTEMROOT_LINKNAME "\\SystemRoot"
#define INIT_SYSTEMROOT_DLLPATH  "\\SystemRoot\\System32"
#define INIT_SYSTEMROOT_BINPATH  "\\SystemRoot\\System32"

#define INIT_WINPEMODE_NONE                 0x00000000
#define INIT_WINPEMODE_REGULAR              0x00000001
#define INIT_WINPEMODE_INRAM                0x80000000
#define INIT_WINPEMODE_READONLY_MEDIA       0x00000100
#define INIT_WINPEMODE_REMOVABLE_MEDIA      0x00000200  

extern UNICODE_STRING NtSystemRoot;
extern ULONG NtBuildNumber;
extern ULONG NtMajorVersion;
extern ULONG NtMinorVersion;
extern ULONG CmNtCSDVersion;
extern UNICODE_STRING CmVersionString;
extern UNICODE_STRING CmCSDVersionString;

extern NLSTABLEINFO InitTableInfo;
extern ULONG InitNlsTableSize;
extern PVOID InitNlsTableBase;
extern ULONG InitAnsiCodePageDataOffset;
extern ULONG InitOemCodePageDataOffset;
extern ULONG InitUnicodeCaseTableDataOffset;
extern PVOID InitNlsSectionPointer;

extern BOOLEAN InitIsWinPEMode;
extern ULONG InitWinPEModeType;

#ifdef _M_IX86

VOID
KiSystemStartup(
    IN PVOID LoaderBlock
    );

#else

VOID
KiSystemStartup( VOID );

#endif

VOID
Phase1Initialization(
    IN PVOID Context
    );

typedef
BOOLEAN
(*PTESTFCN)( VOID );

extern PTESTFCN TestFunction;
extern ULONG InitializationPhase;

#if DBG
extern BOOLEAN ForceNonPagedPool;
extern ULONG MmDebug;
#endif // DBG

#endif // _INIT_

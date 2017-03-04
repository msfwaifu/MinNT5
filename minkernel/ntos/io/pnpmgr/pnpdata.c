/*++

Copyright (c) 1995  Microsoft Corporation

Module Name:

    pnpsubs.c

Abstract:

    This module contains the plug-and-play data

Author:

    Shie-Lin Tzong (shielint) 30-Jan-1995

Environment:

    Kernel mode


Revision History:


--*/

#include "pnpmgrp.h"

#ifdef ALLOC_DATA_PRAGMA
#pragma data_seg("INIT")
#endif

//
// only available during phase-1 PnP initialization.
//
PVOID PiScratchBuffer = NULL;

PVOID IopPnpScratchBuffer1 = NULL;
PVOID IopPnpScratchBuffer2 = NULL;

#if _PNP_POWER_

ULONG IopPeripheralCount[MaximumType + 1] = {
            0,                  // ArcSystem
            0,                  // CentralProcessor",
            0,                  // FloatingPointProcessor",
            0,                  // PrimaryICache",
            0,                  // PrimaryDCache",
            0,                  // SecondaryICache",
            0,                  // SecondaryDCache",
            0,                  // SecondaryCache",
            0,                  // EisaAdapter", (8)
            0,                  // TcAdapter",   (9)
            0,                  // ScsiAdapter",
            0,                  // DtiAdapter",
            0,                  // MultifunctionAdapter", (12)
            0,                  // DiskController", (13)
            0,                  // TapeController",
            0,                  // CdRomController",
            0,                  // WormController",
            0,                  // SerialController",
            0,                  // NetworkController",
            0,                  // DisplayController",
            0,                  // ParallelController",
            0,                  // PointerController",
            0,                  // KeyboardController",
            0,                  // AudioController",
            0,                  // OtherController",
            0,                  // DiskPeripheral",
            0,                  // FloppyDiskPeripheral",
            0,                  // TapePeripheral",
            0,                  // ModemPeripheral",
            0,                  // MonitorPeripheral",
            0,                  // PrinterPeripheral",
            0,                  // PointerPeripheral",
            0,                  // KeyboardPeripheral",
            0,                  // TerminalPeripheral",
            0,                  // OtherPeripheral",
            0,                  // LinePeripheral",
            0,                  // NetworkPeripheral",
            0,                  // SystemMemory",
            0                   // Undefined"
            };

#endif // _PNP_POWER_

#ifdef ALLOC_DATA_PRAGMA
#pragma  data_seg()
#endif


//
// The following resource is used to control access to device-related, Plug and Play-specific
// portions of the registry. These portions are:
//
//   HKLM\System\Enum
//   HKLM\System\CurrentControlSet\Hardware Profiles
//   HKLM\System\CurrentControlSet\Services\<service>\Enum
//
// It allows exclusive access for writing, as well as shared access for reading.
// The resource is initialized by the PnP manager initialization code during phase 0
// initialization.
//

ERESOURCE  PpRegistryDeviceResource;

#if _PNP_POWER_

//
// Persistent data
//
// The following resource is used to control access to the Plug and Play manager's
// bus database.  It allows exclusive access to the PnP bus instance list for
// adding/removing buses, as well as shared access for querying bus information.
// The resource is initialized by the PnP manager initialization code during phase 0
// initialization.
//

ERESOURCE  PpBusResource;

//
// The following list header contains the list of installed Plug and Play bus types
// (i.e., their bus extender has been installed). Each bus type for which bus
// instances have been found will contain a list of those instances.
// Access to this list is protected by PpBusResource.
//

LIST_ENTRY PpBusListHead;

#endif // _PNP_POWER_

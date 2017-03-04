
/*++

Copyright (c) 1995  Microsoft Corporation

Module Name:

    pnpsubs.c

Abstract:

    This module contains the plug-and-play initialization
    subroutines for the I/O system.


Author:

    Shie-Lin Tzong (shielint) 30-Jan-1995

Environment:

    Kernel mode


Revision History:

    Mason Back (masonly) 02-Mar-2017:
        Merged io\pnpinit.c and pnp\pnpinit.c.
		
--*/

#include "pnpmgrp.h"

#if _PNP_POWER_

extern ULONG IopPeripheralCount[];

BOOLEAN
IopIsDuplicatedResourceLists(
    IN PCM_RESOURCE_LIST Configuration1,
    IN PCM_RESOURCE_LIST Configuration2
    );

NTSTATUS
IopInitializeHardwareConfiguration(
    IN PLOADER_PARAMETER_BLOCK LoaderBlock
    );

NTSTATUS
IopSetupConfigurationTree(
     IN PCONFIGURATION_COMPONENT_DATA CurrentEntry,
     IN HANDLE Handle,
     IN PUNICODE_STRING ParentName,
     IN INTERFACE_TYPE InterfaceType,
     IN BUS_DATA_TYPE BusDataType,
     IN ULONG BusNumber
     );

NTSTATUS
IopInitializeRegistryNode(
    IN PCONFIGURATION_COMPONENT_DATA CurrentEntry,
    IN HANDLE EnumHandle,
    IN PUNICODE_STRING ParentKeyName,
    IN PUNICODE_STRING KeyName,
    IN ULONG Instance,
    IN INTERFACE_TYPE InterfaceType,
    IN BUS_DATA_TYPE BusDataType,
    IN ULONG BusNumber
    );
	
BOOLEAN
PiAddPlugPlayBusEnumeratorToList(
    IN     HANDLE ServiceKeyHandle,
    IN     PUNICODE_STRING ServiceName,
    IN OUT PVOID Context
    );

NTSTATUS
PiRegisterBuiltInBuses(
    VOID
    );

NTSTATUS
PiAddBuiltInBusToEnumRoot (
    IN  PUNICODE_STRING PlugPlayIdString,
    IN  ULONG BusNumber,
    IN  INTERFACE_TYPE InterfaceType,
    IN  BUS_DATA_TYPE BusDataType,
    OUT PUNICODE_STRING DeviceInstancePath
    );

#endif // _PNP_POWER_


//
// Prototype functions internal to this file.
//
BOOLEAN
PiInitPhase0(
    VOID
    );

BOOLEAN
PiInitPhase1(
    VOID
    );

NTSTATUS
PiInitializeSystemEnum(
    VOID
    );

NTSTATUS
PiInitializeSystemEnumSubKeys(
    IN HANDLE CurrentKeyHandle,
    IN PUNICODE_STRING ValueName
    );

NTSTATUS
IopInitServiceEnumList (
    VOID
    );

#if 0
BOOLEAN
IopInitializeBusKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING KeyName,
    IN OUT PVOID WorkName
    );
#endif
BOOLEAN
IopInitializeDeviceKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING KeyName,
    IN OUT PVOID WorkName
    );

BOOLEAN
IopInitializeDeviceInstanceKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING KeyName,
    IN OUT PVOID WorkName
    );

#ifdef ALLOC_PRAGMA
#if _PNP_POWER_
#pragma alloc_text(INIT, IopIsDuplicatedResourceLists)
#pragma alloc_text(INIT, IopInitializeHardwareConfiguration)
#pragma alloc_text(INIT, IopSetupConfigurationTree)
#pragma alloc_text(INIT, IopInitializeRegistryNode)
#pragma alloc_text(INIT,PiAddPlugPlayBusEnumeratorToList)
#pragma alloc_text(INIT,PiRegisterBuiltInBuses)
#pragma alloc_text(INIT,PiAddBuiltInBusToEnumRoot)
#endif // _PNP_POWER_
#pragma alloc_text(INIT, IopInitServiceEnumList)
#pragma alloc_text(INIT,PpInitSystem)
#pragma alloc_text(INIT,PiInitPhase0)
#pragma alloc_text(INIT,PiInitPhase1)
#pragma alloc_text(INIT,PiInitializeSystemEnum)
#pragma alloc_text(INIT,PiInitializeSystemEnumSubKeys)
#if 0
#pragma alloc_text(INIT, IopInitializeBusKey)
#endif
#pragma alloc_text(INIT, IopInitializeDeviceKey)
#pragma alloc_text(INIT, IopInitializeDeviceInstanceKey)
#pragma alloc_text(INIT, IopInitializePlugPlayServices)
#endif



BOOLEAN
PpInitSystem (
    VOID
    )

/*++

Routine Description:

    This function performs initialization of the kernel-mode Plug and Play
    Manager.  It is called during phase 0 and phase 1 initialization.  Its
    function is to dispatch to the appropriate phase initialization routine.

Arguments:

    None.

Return Value:

    TRUE  - Initialization succeeded.

    FALSE - Initialization failed.

--*/

{

    switch ( InitializationPhase ) {

    case 0 :
        return PiInitPhase0();
    case 1 :
        return PiInitPhase1();
    default:
        KeBugCheck(UNEXPECTED_INITIALIZATION_CALL);
    }
}

BOOLEAN
PiInitPhase0(
    VOID
    )

/*++

Routine Description:

    This function performs Phase 0 initializaion of the Plug and Play Manager
    component of the NT system. It initializes the PnP registry and bus list
    resources, and initializes the bus list head to empty.

Arguments:

    None.

Return Value:

    TRUE  - Initialization succeeded.

    FALSE - Initialization failed.

--*/

{
#if _PNP_POWER_

    //
    // Initialize the Plug and Play bus list resource.
    //

    ExInitializeResource( &PpBusResource );

    //
    // Initialize the Plug and Play bus list head.
    //
    InitializeListHead( &PpBusListHead );

#endif // _PNP_POWER_

    //
    // Initialize the device-specific, Plug and Play registry resource.
    //
    ExInitializeResource( &PpRegistryDeviceResource );

    return TRUE;
}

BOOLEAN
PiInitPhase1(
    VOID
    )

/*++

Routine Description:

    This function performs Phase 1 initializaion of the Plug and Play Manager
    component of the NT system. It performs the following tasks:

    (1) performs initialization of value entries under all subkeys of
        HKLM\System\Enum (e.g., resetting 'FoundAtEnum' to FALSE).
    (2) initializes bus enumerator structures for all built-in bus extenders
        provided by the HAL.
    (3) builds up a list of all installed bus extenders.


Arguments:

    None.

Return Value:

    TRUE  - Initialization succeeded.

    FALSE - Initialization failed.

--*/

{
#if _PNP_POWER_
    NTSTATUS Status;
#endif

    PiScratchBuffer = ExAllocatePool(PagedPool, PNP_LARGE_SCRATCH_BUFFER_SIZE);
    if(!PiScratchBuffer) {
        return FALSE;
    }

    //
    // Since we'll be writing to PnP device sections in the registry, acquire
    // the PnP device registry resource for exclusive access. We'll also be
    // initializing the bus enumerator list, so we need exclusive access to
    // the PnP bus enumerator list as well.
    //
    KeEnterCriticalRegion();

#if _PNP_POWER_
    ExAcquireResourceExclusive(&PpBusResource, TRUE);
#endif // _PNP_POWER_

    ExAcquireResourceExclusive(&PpRegistryDeviceResource, TRUE);

    //
    // Initialize all 'FoundAtEnum' value entries in the HKLM\System\Enum tree
    // to FALSE.
    //
    PiInitializeSystemEnum();

#if _PNP_POWER_
    //
    // Initialize our bus enumerator list with all bus instances under the control
    // of the HAL's built-in bus extenders.
    //
    Status = PiRegisterBuiltInBuses();

    if(!NT_SUCCESS(Status)) {
#if DBG
        DbgPrint("PiInitPhase1: Couldn't register built-in buses\n");
#endif
        return FALSE;
    }

    //
    // Enumerate all services in the registry, building up a list of all
    // installed bus extenders.
    //
    Status = IopApplyFunctionToSubKeys(NULL,
                                       &CmRegistryMachineSystemCurrentControlSetServices,
                                       KEY_READ,
                                       TRUE,
                                       PiAddPlugPlayBusEnumeratorToList,
                                       NULL
                                      );
    if(!NT_SUCCESS(Status)) {
#if DBG
        DbgPrint("PiInitPhase1: Couldn't build bus enumerator list\n");
#endif
        return FALSE;
    }
#endif // _PNP_POWER_

    ExReleaseResource(&PpRegistryDeviceResource);

#if _PNP_POWER_
    ExReleaseResource(&PpBusResource);
#endif // _PNP_POWER_

    KeLeaveCriticalRegion();

    ExFreePool(PiScratchBuffer);

    return TRUE;
}

NTSTATUS
PiInitializeSystemEnum (
    VOID
    )

/*++

Routine Description:

    This routine scans through HKLM\System\CurrentControlSet\Enum subtree and initializes
    "FoundAtEnum=" entry for each key to FALSE such that subsequent
    initialization code can conditionally set it back to true.

Arguments:

    None.

Return Value:

   The function value is the final status of the operation.

--*/

{
    NTSTATUS Status;
    HANDLE SystemEnumHandle;
    UNICODE_STRING FoundAtEnumName;

    //
    // Open System\Enum key and call worker routine to recursively
    // scan through the subkeys.
    //

    Status = IopOpenRegistryKey(&SystemEnumHandle,
                                NULL,
                                &CmRegistryMachineSystemCurrentControlSetEnumName,
                                KEY_ALL_ACCESS,
                                FALSE
                                );

    if(NT_SUCCESS(Status)) {

        PiWstrToUnicodeString(&FoundAtEnumName, REGSTR_VALUE_FOUNDATENUM);
        Status = PiInitializeSystemEnumSubKeys(SystemEnumHandle, &FoundAtEnumName);

        NtClose(SystemEnumHandle);
        return Status;

    } else {
        return STATUS_SUCCESS;
    }
}

NTSTATUS
PiInitializeSystemEnumSubKeys(
    IN HANDLE CurrentKeyHandle,
    IN PUNICODE_STRING ValueName
    )

/*++

Routine Description:

    This routine checks to see if the key whose handle was passed in contains
    a value whose name was specified by the ValueName argument.  If so, it
    resets this value to a REG_DWORD 0. It then enumerates all subkeys under
    the current key, and recursively calls itself for each one.

Arguments:

    CurrentKeyHandle - Supplies a handle to the key which will be enumerated.

    ValueName - Supplies a pointer to the value entry name to be initialized.

Return Value:

    None.

--*/

{
    NTSTATUS Status;
    PKEY_BASIC_INFORMATION KeyInformation;
    PKEY_VALUE_FULL_INFORMATION KeyValueInformation;
    USHORT i;
    ULONG TempValue, ResultLength;
    UNICODE_STRING UnicodeName;
    HANDLE WorkHandle;

    //
    // Set "FoundAtEnum=" entry of current key to FALSE, if exists.
    //
    Status = IopGetRegistryValue(CurrentKeyHandle,
                                 ValueName->Buffer,
                                 &KeyValueInformation
                                );
    if(NT_SUCCESS(Status)) {
        ExFreePool(KeyValueInformation);
        TempValue = 0;
        Status = NtSetValueKey(CurrentKeyHandle,
                               ValueName,
                               TITLE_INDEX_VALUE,
                               REG_DWORD,
                               &TempValue,
                               sizeof(TempValue)
                              );

        if(!NT_SUCCESS(Status)) {
            return Status;
        }
    }

    //
    // Enumerate current node's children and apply ourselves to each one
    //
    KeyInformation = (PKEY_BASIC_INFORMATION)PiScratchBuffer;

    for(i = 0; TRUE; i++) {
        Status = NtEnumerateKey(CurrentKeyHandle,
                                i,
                                KeyBasicInformation,
                                KeyInformation,
                                PNP_LARGE_SCRATCH_BUFFER_SIZE,
                                &ResultLength
                               );

        if(Status == STATUS_NO_MORE_ENTRIES) {
            break;
        } else if(!NT_SUCCESS(Status)) {
            continue;
        }
        UnicodeName.Length = (USHORT)KeyInformation->NameLength;
        UnicodeName.MaximumLength = (USHORT)KeyInformation->NameLength;
        UnicodeName.Buffer = KeyInformation->Name;

        Status = IopOpenRegistryKey(&WorkHandle,
                                    CurrentKeyHandle,
                                    &UnicodeName,
                                    KEY_ALL_ACCESS,
                                    FALSE
                                    );
        if(!NT_SUCCESS(Status)) {
            continue;
        }

        Status = PiInitializeSystemEnumSubKeys(WorkHandle, ValueName);
        NtClose(WorkHandle);
        if(!NT_SUCCESS(Status)) {
            return Status;
        }
    }
    return STATUS_SUCCESS;
}
#if _PNP_POWER_

BOOLEAN
PiAddPlugPlayBusEnumeratorToList(
    IN     HANDLE ServiceKeyHandle,
    IN     PUNICODE_STRING ServiceName,
    IN OUT PVOID Context
    )

/*++

Routine Description:

    This routine is a callback function for IopApplyFunctionToSubKeys.
    It is called for each service key under
    HKLM\System\CurrentControlSet\Services. Its purpose is to take each
    service entry representing a bus enumerator, and create a corresponding
    bus enumerator entry in the Plug&Play bus enumerator list.

    NOTE: The PnP Bus Enumerator list resource must be held for exclusive
    (write) access before invoking this routine. The PnP device-specific
    registry resource must be held for shared (read) access.

Arguments:

    ServiceKeyHandle - Supplies a handle to a service entry key.

    ServiceName - Supplies the name of this service.

    Context - Not used.

Returns:

    TRUE to continue the enumeration.
    FALSE to abort it.

--*/

{
    UNICODE_STRING ValueName;
    NTSTATUS Status;
    PKEY_VALUE_FULL_INFORMATION KeyValueInformation;
    ULONG RequiredLength;
    BOOLEAN IsBusExtender = FALSE;
    PUNICODE_STRING DeviceIDList;
    ULONG DeviceIDCount;
    PPLUGPLAY_BUS_ENUMERATOR BusEnumerator;

    //
    // First, see if this service is for a bus extender.  (Since we have
    // scratch space, use it, so we don't do memory allocations when checking
    // each service.)
    //
    RtlInitUnicodeString(&ValueName, REGSTR_VALUE_PLUGPLAY_SERVICE_TYPE);
    Status = NtQueryValueKey(ServiceKeyHandle,
                             &ValueName,
                             KeyValueFullInformation,
                             PiScratchBuffer,
                             PNP_LARGE_SCRATCH_BUFFER_SIZE,
                             &RequiredLength
                            );
    if(NT_SUCCESS(Status)) {
        if((((PKEY_VALUE_FULL_INFORMATION)PiScratchBuffer)->Type == REG_DWORD) &&
           (((PKEY_VALUE_FULL_INFORMATION)PiScratchBuffer)->DataLength >= sizeof(ULONG))) {

            IsBusExtender = (PlugPlayServiceBusExtender == (PLUGPLAY_SERVICE_TYPE)
                    (*(PULONG)KEY_VALUE_DATA((PKEY_VALUE_FULL_INFORMATION)PiScratchBuffer)));
        }
    }

    if(!IsBusExtender) {
        return TRUE;
    }

    //
    // We have a bus extender, so allocate a node for it.
    //
    BusEnumerator = (PPLUGPLAY_BUS_ENUMERATOR)ExAllocatePool(PagedPool,
                                                             sizeof(PLUGPLAY_BUS_ENUMERATOR));
    if(!BusEnumerator) {
        return TRUE;
    }

    if(!IopConcatenateUnicodeStrings(&(BusEnumerator->ServiceName),
                                     ServiceName,
                                     NULL
                                    )) {
        ExFreePool(BusEnumerator);
        return TRUE;
    }

    //
    // Now, retrieve the list of compatible device IDs from the service's
    // DeviceIDs REG_MULTI_SZ value entry.
    //
    Status = IopGetRegistryValue(ServiceKeyHandle,
                                 REGSTR_VALUE_DEVICE_IDS,
                                 &KeyValueInformation
                                );
    if(!NT_SUCCESS(Status)) {
        //
        // Since we couldn't retrieve any compatible IDs, assume there are none,
        // and initialize the BusEnumerator structure with a device ID count of zero.
        //
        DeviceIDList = NULL;
        DeviceIDCount = 0;
    } else {

        Status = IopRegMultiSzToUnicodeStrings(KeyValueInformation,
                                               &DeviceIDList,
                                               &DeviceIDCount
                                              );
        ExFreePool(KeyValueInformation);

        if(!NT_SUCCESS(Status)) {
            ExFreePool(BusEnumerator->ServiceName.Buffer);
            ExFreePool(BusEnumerator);
            return TRUE;
        }
    }

    //
    // Finish filling in the bus enumerator node and insert it into the tail of the list.
    //
    InitializeListHead(&(BusEnumerator->BusInstanceListEntry));
    BusEnumerator->PlugPlayIDs = DeviceIDList;
    BusEnumerator->PlugPlayIDCount = DeviceIDCount;
    RtlInitUnicodeString(&(BusEnumerator->DriverName), NULL);
    //BusEnumerator->DriverObject = NULL;

    InsertTailList(&PpBusListHead, &(BusEnumerator->BusEnumeratorListEntry));

    return TRUE;
}

NTSTATUS
PiRegisterBuiltInBuses(
    VOID
    )

/*++

Routine Description:

    This routine processes each bus instance handled by the built-in bus extender
    provided by the HAL (as returned from a call to HalQuerySystemInformation for
    information class HalInstalledBusInformation). This call is made during
    Phase-1 initialization of the Plug & Play manager, before the I/O system has
    initialized (and therefore, before any installed bus extenders have initialized).

    If a non-empty set of buses are returned in the HAL_BUS_INFORMATION array, then
    a bus enumerator node is created for the built-in HAL extender. A
    PLUGPLAY_BUS_INSTANCE_FULL_DESCRIPTOR is then created for each reported bus, and
    is added to the bus enumerator's BusInstanceListEntry list. In addition, each bus
    instance is recorded in a made-up key in the registry under HKLM\System\Enum\Root.
    These keys are volatile, since there is no need to maintain information about these
    buses across boots. (In fact, it is undesirable to do so, since we'd have to go
    and clean up entries made in previous boots before doing our work here.)

    The PnP device registry and bus list resources must have both been acquired for
    exclusive (write) access before calling this routine.

Arguments:

    None.

Returns:

    NT status indicating whether the routine was successful.

--*/

{
    NTSTATUS Status;
    PHAL_BUS_INFORMATION NewBusInfo;
    ULONG NewBusCount, i, BusNameLength;
    PPLUGPLAY_BUS_ENUMERATOR BusEnumerator;
    PPLUGPLAY_BUS_INSTANCE_FULL_DESCRIPTOR BusInstanceNode;
    PPLUGPLAY_BUS_INSTANCE BusInstanceInformation;
    UNICODE_STRING TempUnicodeString, PlugPlayIdString;
    HANDLE CCSHandle, BusValuesHandle;

    //
    // First, get the current list of installed buses
    //
    Status = PiGetInstalledBusInformation(&NewBusInfo, &NewBusCount);
    if(!(NT_SUCCESS(Status) && NewBusCount)) {
        return Status;
    }

    //
    // Create/initialize a bus enumerator node
    //
    BusEnumerator = (PPLUGPLAY_BUS_ENUMERATOR)ExAllocatePool(PagedPool,
                                                             sizeof(PLUGPLAY_BUS_ENUMERATOR)
                                                            );
    if(!BusEnumerator) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto PrepareForReturn0;
    }

    //
    // Built-in bus extenders don't have a service name.
    //
    RtlInitUnicodeString(&(BusEnumerator->ServiceName), NULL);

    RtlInitUnicodeString(&(BusEnumerator->DriverName), NULL);
    InitializeListHead(&(BusEnumerator->BusInstanceListEntry));
//    BusEnumerator->DriverObject = NULL;
    //
    // We don't need to keep track of the made-up PnP IDs that we add to this list, since
    // there's no way to add more bus instances to built-in bus extenders.
    //
    BusEnumerator->PlugPlayIDs = NULL;
    BusEnumerator->PlugPlayIDCount = 0;

    //
    // Insert this bus enumerator node into our bus list.
    //
    InsertTailList(&PpBusListHead, &(BusEnumerator->BusEnumeratorListEntry));

    //
    // Open a handle to HKLM\System\CurrentControlSet\Control\SystemResources\BusValues
    // so that we can retrieve friendly names for the buses we've found.
    //
    Status = IopOpenRegistryKey(&CCSHandle,
                                NULL,
                                &CmRegistryMachineSystemCurrentControlSet,
                                KEY_READ,
                                FALSE
                               );
    if(NT_SUCCESS(Status)) {
        PiWstrToUnicodeString(&TempUnicodeString, REGSTR_PATH_SYSTEM_RESOURCES_BUS_VALUES);
        Status = IopOpenRegistryKey(&BusValuesHandle,
                                    CCSHandle,
                                    &TempUnicodeString,
                                    KEY_READ,
                                    FALSE
                                   );
        NtClose(CCSHandle);
    }

    if(!NT_SUCCESS(Status)) {
#if DBG
        DbgPrint("PiRegisterBuiltInBuses: Couldn't open CCS\\Control\\SystemResources\\BusValues.\n");
#endif
        BusValuesHandle = NULL;
    }

    //
    // Register each bus instance.
    //
    for(i = 0; i < NewBusCount; i++) {
        //
        // Create a bus instance node and fill it in.
        //
        BusInstanceNode = (PPLUGPLAY_BUS_INSTANCE_FULL_DESCRIPTOR)
                ExAllocatePool(PagedPool, sizeof(PLUGPLAY_BUS_INSTANCE_FULL_DESCRIPTOR));

        if(!BusInstanceNode) {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto PrepareForReturn1;
        }

        BusInstanceNode->AssociatedConfigurationSpace = NewBusInfo[i].ConfigurationType;
        //
        // All built-in buses are considered 'root' buses.
        //
        BusInstanceNode->RootBus = TRUE;
        //
        // Built-in buses have no associated service, hence, no service instance ordinal.
        //
        BusInstanceNode->ServiceInstanceOrdinal = PLUGPLAY_NO_INSTANCE;

        BusInstanceInformation = &(BusInstanceNode->BusInstanceInformation);
        //
        // Retrieve the friendly name for this bus.
        //
        if(BusValuesHandle) {
            Status = IopLookupBusStringFromID(BusValuesHandle,
                                              NewBusInfo[i].BusType,
                                              PiScratchBuffer,
                                              PNP_LARGE_SCRATCH_BUFFER_SIZE,
                                              NULL
                                             );
        } else {
            //
            // Just set some error so we'll know we need to create a made-up name.
            //
            Status = STATUS_INVALID_HANDLE;
        }

        if(NT_SUCCESS(Status)) {

            BusNameLength = 0;
            do {
                if(BusInstanceInformation->BusName[BusNameLength] =
                       ((PWCHAR)PiScratchBuffer)[BusNameLength]) {

                    BusNameLength++;
                } else {
                    break;
                }
            } while(BusNameLength < MAX_BUS_NAME);

            if(BusNameLength == MAX_BUS_NAME) {
                BusInstanceInformation->BusName[--BusNameLength] = UNICODE_NULL;
            }
        } else {
            //
            // We couldn't retrieve a friendly name for this bus--no big deal.
            // We'll just make up one.
            //
            swprintf(BusInstanceInformation->BusName,
                     REGSTR_VALUE_INTERFACE_TYPE_FORMAT,
                     (ULONG)(NewBusInfo[i].BusType)
                    );
        }
        BusInstanceInformation->BusNumber = NewBusInfo[i].BusNumber;
        BusInstanceInformation->BusType.BusClass = SystemBus;
        BusInstanceInformation->BusType.SystemBusType = NewBusInfo[i].BusType;

        //
        // Generate a made-up Plug & Play ID for this bus instance.  This ID will be of
        // the form "*BIB<BusType>," where <BusType> is the bus's interface type, represented
        // as a 4-digit hexadecimal number (i.e., conforming to the EISA id format).
        // (BIB stands for "Built-In Bus")
        //
        BusNameLength = (ULONG)swprintf((PWCHAR)PiScratchBuffer,
                                        REGSTR_KEY_BIB_FORMAT,
                                        (ULONG)(NewBusInfo[i].BusType)
                                       );
        PlugPlayIdString.Length = (USHORT)CWC_TO_CB(BusNameLength);
        PlugPlayIdString.MaximumLength = PNP_LARGE_SCRATCH_BUFFER_SIZE;
        PlugPlayIdString.Buffer = (PWCHAR)PiScratchBuffer;

        //
        // Register this bus device instance under HKLM\System\Enum\Root.  Use its
        // bus number for the device instance name.
        //
        Status = PiAddBuiltInBusToEnumRoot(&PlugPlayIdString,
                                           NewBusInfo[i].BusNumber,
                                           NewBusInfo[i].BusType,
                                           NewBusInfo[i].ConfigurationType,
                                           &(BusInstanceNode->DeviceInstancePath)
                                          );

        if(!NT_SUCCESS(Status)) {
            ExFreePool(BusInstanceNode);
            goto PrepareForReturn1;
        }

        //
        // This bus instance has been successfully registered, so add it to our bus list.
        //
        InsertTailList(&(BusEnumerator->BusInstanceListEntry),
                       &(BusInstanceNode->BusInstanceListEntry)
                      );
    }

PrepareForReturn1:

    if(BusValuesHandle) {
        NtClose(BusValuesHandle);
    }

PrepareForReturn0:

    ExFreePool(NewBusInfo);

    return Status;
}

NTSTATUS
PiAddBuiltInBusToEnumRoot (
    IN  PUNICODE_STRING PlugPlayIdString,
    IN  ULONG BusNumber,
    IN  INTERFACE_TYPE InterfaceType,
    IN  BUS_DATA_TYPE BusDataType,
    OUT PUNICODE_STRING DeviceInstancePath
    )

/*++

Routine Description:

    This routine registers the specified Plug and Play device ID representing
    a bus controlled by a HAL-provided bus extender.  A volatile key under
    HKLM\System\Enum\Root is created with this device ID name. (The key is
    made volatile because there is no state information that needs to be stored
    across boots, and this keeps us from having to go clean up on next boot.)
    The device instance subkey is created as a 4-digit, base-10 number representing
    the specified bus number.  The full device instance path (relative to
    HKLM\System\Enum) is returned.

    It is the caller's responsibility to free the (PagedPool) memory allocated for
    the unicode string buffer returned in DeviceInstancePath.

    The PnP device registry resource must have been acquired for exclusive (write)
    access before calling this routine.

Arguments:

    PlugPlayIdString - (made-up) Plug and Play ID for this built-in bus device.

    BusNumber - Supplies the ordinal of this bus instance.

    InterfaceType - Supplies the interface type of this bus.

    BusDataType - Supplies the configuration space for this bus.

    DeviceInstancePath - Receives the resulting path (relative to HKLM\System\Enum)
        where this device instance was registered.

Return Value:

    NT status indicating whether the routine was successful.

--*/

{
    NTSTATUS Status;
    HANDLE EnumHandle, EnumRootHandle, DeviceHandle, DevInstHandle;
    UNICODE_STRING UnicodeString;
    ULONG StringLength, CurStringLocation, TmpDwordValue;
    UNICODE_STRING InstanceKeyName, ValueName;

    //
    // Create the full device instance path (relative to HKLM\System\Enum to be returned
    // in the DeviceInstancePath parameter.
    //
    StringLength = sizeof(REGSTR_KEY_ROOTENUM)   // includes terminating NULL
                   + PlugPlayIdString->Length
                   + 12;    // 4-digit instance name & 2 backslashes
    DeviceInstancePath->Buffer = (PWCHAR)ExAllocatePool(PagedPool, StringLength);
    if(!DeviceInstancePath->Buffer) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    DeviceInstancePath->Length = (DeviceInstancePath->MaximumLength = (USHORT)StringLength)
                                    - sizeof(UNICODE_NULL);

    RtlMoveMemory(DeviceInstancePath->Buffer,
                  REGSTR_KEY_ROOTENUM,
                  (CurStringLocation = sizeof(REGSTR_KEY_ROOTENUM) - sizeof(WCHAR))
                 );
    CurStringLocation = CB_TO_CWC(CurStringLocation);
    DeviceInstancePath->Buffer[CurStringLocation++] = OBJ_NAME_PATH_SEPARATOR;
    RtlMoveMemory(&(DeviceInstancePath->Buffer[CurStringLocation]),
                  PlugPlayIdString->Buffer,
                  PlugPlayIdString->Length
                 );
    CurStringLocation += CB_TO_CWC(PlugPlayIdString->Length);
    DeviceInstancePath->Buffer[CurStringLocation++] = OBJ_NAME_PATH_SEPARATOR;
    //
    // Add the device instance name to the path, and while we're at it, initialize
    // a unicode string with this name as well.
    //
    PiUlongToInstanceKeyUnicodeString(&InstanceKeyName,
                                      &(DeviceInstancePath->Buffer[CurStringLocation]),
                                      StringLength - CWC_TO_CB(CurStringLocation),
                                      BusNumber
                                     );

    //
    // Next, open HKLM\System\Enum key, then Root sukey, creating these as persistent
    // keys if they don't already exist.
    //
    Status = IopOpenRegistryKeyPersist(&EnumHandle,
                                       NULL,
                                       &CmRegistryMachineSystemCurrentControlSetEnumName,
                                       KEY_ALL_ACCESS,
                                       TRUE,
                                       NULL
                                      );
    if(!NT_SUCCESS(Status)) {
#if DBG
        DbgPrint("PiAddBuiltInBusToEnumRoot: Couldn't open/create HKLM\\System\\Enum (%x).\n",
                 Status
                );
#endif
        goto PrepareForReturn;
    }

    PiWstrToUnicodeString(&UnicodeString, REGSTR_KEY_ROOTENUM);
    Status = IopOpenRegistryKeyPersist(&EnumRootHandle,
                                       EnumHandle,
                                       &UnicodeString,
                                       KEY_ALL_ACCESS,
                                       TRUE,
                                       NULL
                                      );
    NtClose(EnumHandle);
    if(!NT_SUCCESS(Status)) {
#if DBG
        DbgPrint("PiAddBuiltInBusToEnumRoot: Couldn't open/create HKLM\\System\\Enum\\Root (%x).\n",
                 Status
                );
#endif
        goto PrepareForReturn;
    }

    //
    // Now create a volatile key under HKLM\System\Enum\Root for this bus device.
    //
    Status = IopOpenRegistryKey(&DeviceHandle,
                                EnumRootHandle,
                                PlugPlayIdString,
                                KEY_ALL_ACCESS,
                                TRUE
                               );
    NtClose(EnumRootHandle);
    if(!NT_SUCCESS(Status)) {
#if DBG
        DbgPrint("PiAddBuiltInBusToEnumRoot: Couldn't create bus device key\n");
        DbgPrint("                           %wZ. (Status %x).\n",
                 PlugPlayIdString,
                 Status
                );
#endif
        goto PrepareForReturn;
    }

    //
    // Fill in default value entries under the device key (ignore status returned from
    // ZwSetValueKey).
    //
#if 0
    // NewDevice = REG_DWORD : 0
    //
    PiWstrToUnicodeString(&ValueName, REGSTR_VALUE_NEWDEVICE);
    TmpDwordValue = 0;
    ZwSetValueKey(DeviceHandle,
                  &ValueName,
                  TITLE_INDEX_VALUE,
                  REG_DWORD,
                  &TmpDwordValue,
                  sizeof(TmpDwordValue)
                 );
#endif

    //
    // Create a device instance key under this device key.
    //
    Status = IopOpenRegistryKey(&DevInstHandle,
                                DeviceHandle,
                                &InstanceKeyName,
                                KEY_ALL_ACCESS,
                                TRUE
                               );
    NtClose(DeviceHandle);
    if(!NT_SUCCESS(Status)) {
#if DBG
        DbgPrint("PiAddBuiltInBusToEnumRoot: Couldn't create bus device key\n");
        DbgPrint("                           %wZ. (Status %x).\n",
                 PlugPlayIdString,
                 Status
                );
#endif
        goto PrepareForReturn;
    }

    //
    // Fill in default value entries under the device instance key (ignore
    // status returned from ZwSetValueKey).
    //
    // NewInstance = REG_DWORD : 0
    // FountAtEnum = REG_DWORD : 1
    // InterfaceType = REG_DWORD : <InterfaceType>
    // BusDataType = REG_DWORD : <BusDataType>
    // SystemBusNumber = REG_DWORD : <BusNumber>
    // Class = REG_SZ : "System"
    //
#if 0
    PiWstrToUnicodeString(&ValueName, REGSTR_VALUE_NEWINSTANCE);
    ZwSetValueKey(DevInstHandle,
                  &ValueName,
                  TITLE_INDEX_VALUE,
                  REG_DWORD,
                  &TmpDwordValue,
                  sizeof(TmpDwordValue)
                 );
#endif
    TmpDwordValue = 1;
    PiWstrToUnicodeString(&ValueName, REGSTR_VALUE_FOUNDATENUM);
    ZwSetValueKey(DevInstHandle,
                  &ValueName,
                  TITLE_INDEX_VALUE,
                  REG_DWORD,
                  &TmpDwordValue,
                  sizeof(TmpDwordValue)
                 );

    TmpDwordValue = (ULONG)InterfaceType;
    PiWstrToUnicodeString(&ValueName, REGSTR_VALUE_INTERFACETYPE);
    ZwSetValueKey(DevInstHandle,
                  &ValueName,
                  TITLE_INDEX_VALUE,
                  REG_DWORD,
                  &TmpDwordValue,
                  sizeof(TmpDwordValue)
                 );

    TmpDwordValue = (ULONG)BusDataType;
    PiWstrToUnicodeString(&ValueName, REGSTR_VALUE_BUSDATATYPE);
    ZwSetValueKey(DevInstHandle,
                  &ValueName,
                  TITLE_INDEX_VALUE,
                  REG_DWORD,
                  &TmpDwordValue,
                  sizeof(TmpDwordValue)
                 );

    TmpDwordValue = BusNumber;
    PiWstrToUnicodeString(&ValueName, REGSTR_VALUE_SYSTEMBUSNUMBER);
    ZwSetValueKey(DevInstHandle,
                  &ValueName,
                  TITLE_INDEX_VALUE,
                  REG_DWORD,
                  &TmpDwordValue,
                  sizeof(TmpDwordValue)
                 );

    PiWstrToUnicodeString(&ValueName, REGSTR_VALUE_CLASS);
    ZwSetValueKey(DevInstHandle,
                  &ValueName,
                  TITLE_INDEX_VALUE,
                  REG_SZ,
                  REGSTR_KEY_SYSTEM,
                  sizeof(REGSTR_KEY_SYSTEM)
                 );

    NtClose(DevInstHandle);

PrepareForReturn:

    if(!NT_SUCCESS(Status)) {
        ExFreePool(DeviceInstancePath->Buffer);
    }

    return Status;
}
#endif // _PNP_POWER_



#if _PNP_POWER_
BOOLEAN
IopIsDuplicatedResourceLists(
    IN PCM_RESOURCE_LIST Configuration1,
    IN PCM_RESOURCE_LIST Configuration2
    )

/*++

Routine Description:

    This routine compares two set of resource lists to
    determine if they are exactly the same.

Arguments:

    Configuration1 - Supplies a pointer to the first set of resource.

    Configuration2 - Supplies a pointer to the second set of resource.

Return Value:

    returns TRUE if the two set of resources are the same;
    otherwise a value of FALSE is returned.

--*/

{
    ULONG resource1Size, resource2Size;
    BOOLEAN sameResource = FALSE;

    resource1Size = IopDetermineResourceListSize(Configuration1);
    resource2Size = IopDetermineResourceListSize(Configuration2);
    if (resource1Size == resource2Size) {
        if (!memcmp(Configuration1, Configuration2, resource1Size)) {
            sameResource = TRUE;
        }
    }
    return sameResource;
}

NTSTATUS
IopInitializeHardwareConfiguration(
    IN PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine creates \\Registry\Machine\Sysem\Enum\Root node in
    the registry and calls worker routine to put the hardware
    information detected by arc firmware/ntdetect to the Enum\Root
    branch.
    This routine and its worker routines use both pnp scratch buffer1
    and scratch buffer2.

Arguments:

    LoaderBlock - supplies a pointer to the LoaderBlock passed in from the
        OS Loader.

Returns:

    NTSTATUS code for sucess or reason of failure.

--*/
{
    NTSTATUS status;
    HANDLE baseHandle;
    UNICODE_STRING unicodeName, rootName;
    PCONFIGURATION_COMPONENT_DATA currentEntry;
    PCONFIGURATION_COMPONENT component;
    INTERFACE_TYPE interfaceType;
    BUS_DATA_TYPE busDataType;
    ULONG busNumber, i;

    unicodeName.Length = 0;
    unicodeName.MaximumLength = PNP_LARGE_SCRATCH_BUFFER_SIZE;
    unicodeName.Buffer = IopPnpScratchBuffer1;
    PiWstrToUnicodeString(&rootName, REGSTR_KEY_ROOTENUM);
    RtlAppendStringToString((PSTRING)&unicodeName,
                            (PSTRING)&rootName);
    currentEntry = (PCONFIGURATION_COMPONENT_DATA)LoaderBlock->ConfigurationRoot;

    if (currentEntry) {

        //
        // Open\Create \\Registry\Machine\System\CurrentControlSet\Enum\Root and use the
        // returned handle as the BaseHandle to build the Arc keys.
        //

        status = IopOpenRegistryKey(&baseHandle,
                                    NULL,
                                    &CmRegistryMachineSystemCurrentControlSetEnumRootName,
                                    KEY_ALL_ACCESS,
                                    FALSE
                                    );
        if (!NT_SUCCESS(status)) {
            return status;
        }

        currentEntry = currentEntry->Child;

        while (currentEntry != NULL) {

            component = &currentEntry->ComponentEntry;

            //
            // We are only interested in isa, or internal bus component.
            // For other busses, they will be picked up by bus enumerators.
            //

            if (component->Class == AdapterClass) {
                if (component->Type == MultiFunctionAdapter) {
                    _strupr(component->Identifier);
                    if (!strstr(component->Identifier, "PNP")) {
                        if (!_stricmp(component->Identifier, "ISA")) {
                            interfaceType = Isa;
                            busNumber = 0;
                            busDataType = MaximumBusDataType;
                        } else if (!_stricmp(component->Identifier, "INTERNAL")) {
                            interfaceType = Internal;
                            busNumber = 0;
                            busDataType = MaximumBusDataType;
#if defined(_X86_)
                        } else if (!_stricmp(component->Identifier, "MCA")) {
                            interfaceType = MicroChannel;
                            busNumber = 0;
                            busDataType = Pos;
#endif
                        } else {
                            currentEntry = currentEntry->Sibling;
                            continue;
                        }
                    }
                }
#if defined(_X86_)
                  else if (component->Type == EisaAdapter) {
                      interfaceType = Eisa;
                      busNumber = 0;
                      busDataType = EisaConfiguration;
                } else {
                    currentEntry = currentEntry->Sibling;
                    continue;
                }
#endif
#if 0
                //
                // Reset peripheral count before processing a bus.
                //

                for (i = 0; i <= MaximumType; i++) {
                     IopPeripheralCount[i] = 0;
                }
#endif
                status = IopSetupConfigurationTree(currentEntry->Child,
                                                   baseHandle,
                                                   &unicodeName,
                                                   interfaceType,
                                                   busDataType,
                                                   busNumber
                                                   );
            }
            currentEntry = currentEntry->Sibling;
        }
        NtClose(baseHandle);
        return(status);
    } else {
        return STATUS_SUCCESS;
    }
}

NTSTATUS
IopSetupConfigurationTree(
     IN PCONFIGURATION_COMPONENT_DATA CurrentEntry,
     IN HANDLE Handle,
     IN PUNICODE_STRING WorkName,
     IN INTERFACE_TYPE InterfaceType,
     IN BUS_DATA_TYPE BusDataType,
     IN ULONG BusNumber
     )
/*++

Routine Description:

    This routine traverses loader configuration tree and register
    desired hardware information to System\Enuk\Root registry data base.

Arguments:

    CurrentEntry - Supplies a pointer to a loader configuration
        tree or subtree.

    Handle - Supplies the handle to the registry where we can create new key.

    WorkName - Supplies a pointer to a unicode string to specify the
        parent key name of current entry.

    InterfaceType - Specify the Interface type of the bus that the
        CurrentEntry component resides.

    BusDataType - Specify the data/configuration type of the bus that the
        CurrentEntry component resides.

    BusNumber - Specify the Bus Number of the bus that the CurrentEntry
        component resides.  If Bus number is -1, it means InterfaceType
        and BusNumber are meaningless for this component.

Returns:

    NTSTATUS

--*/
{
    NTSTATUS status;
    PCONFIGURATION_COMPONENT component;
    UNICODE_STRING keyName;
    UNICODE_STRING unicodeName;
    static ULONG peripheralCount = 0, controllerCount = 0;
    BOOLEAN freeKeyName = FALSE, nameMapped;
    // BUGBUG (shielint): initialize pnpId for now to avoid compiler warning.
    PWSTR pnpId = NULL;
    STRING stringName;
    USHORT namePosition;

    //
    // Process current entry first
    //

    if (CurrentEntry) {
        component = &CurrentEntry->ComponentEntry;
        nameMapped = FALSE;
#if 0
        switch (component->Type) {
        case DiskController:
             pnpId = L"DiskController";
             break;
        case TapeController:
             pnpId = L"PNP0100";
             break;
        case CdromController:
             pnpId = L"PNP0200";
             break;
        case WormController:
             pnpId = L"PNP0300";
             break;
        case SerialController:
             pnpId = L"PNP0400";
             break;
        case NetworkController:
             pnpId = L"PNP0500";
             break;
        case DisplayController:
             pnpId = L"PNP0600";
             break;
        case ParallelController:
             pnpId = L"PNP0700";
             break;
        case PointerController:
             pnpId = L"PNP0800";
             break;
        case KeyboardController:
             pnpId = L"PNP0900";
             break;
        case AudioController:
             pnpId = L"PNP1000";
             break;
        case OtherController:
             pnpId = L"PNP1100";
             break;
        case DiskPeripheral:
             pnpId = L"PNP1200";
             break;
        case FloppyDiskPeripheral:
             pnpId = L"PNP1300";
             break;
        case TapePeripheral:
             pnpId = L"PNP1400";
             break;
        case ModemPeripheral:
             pnpId = L"PNP1500";
             break;
        case MonitorPeripheral:
             pnpId = L"PNP1600";
             break;
        case PrinterPeripheral:
             pnpId = L"PNP1700";
             break;
        case PointerPeripheral:
             pnpId = L"PNP1800";
             break;
        case KeyboardPeripheral:
             pnpId = L"PNP1900";
             break;
        case TerminalPeripheral:
             pnpId = L"PNP2000";
             break;
        case OtherPeripheral:
             pnpId = L"PNP2100";
             break;
        case LinePeripheral:
             pnpId = L"PNP2200";
             break;
        case NetworkPeripheral:
             pnpId = L"PNP2300";
             break;
        }
#endif
        //
        // if we did NOT successfully mapped a PNP id for the component, we
        // will create a special key name and the key will be processed by
        // user mode inf file later.
        //

        if (nameMapped) {
            RtlCreateUnicodeString(&keyName, pnpId);
        } else {
            RtlInitUnicodeString(&unicodeName, L"Arc");
            IopConcatenateUnicodeStrings(&keyName,
                                         &unicodeName,
                                         &CmTypeName[component->Type]);
            freeKeyName = TRUE;
        }

        //
        // Initialize and copy current component to Enum\Root if it is
        // the one we're insterested in.
        //

        namePosition = WorkName->Length;
        status = IopInitializeRegistryNode(
                         CurrentEntry,
                         Handle,
                         WorkName,
                         &keyName,
                         IopPeripheralCount[component->Type]++,
                         InterfaceType,
                         BusDataType,
                         BusNumber
                         );

        if (freeKeyName) {
            RtlFreeUnicodeString(&keyName);
        }
        if (!NT_SUCCESS(status)) {
            return status;
        }

        //
        // Process the child entry of current entry
        //

        status = IopSetupConfigurationTree(CurrentEntry->Child,
                                           Handle,
                                           WorkName,
                                           InterfaceType,
                                           BusDataType,
                                           BusNumber
                                           );

        WorkName->Length = namePosition;

        if (!NT_SUCCESS(status)) {
            return status;
        }

        //
        // Process all the Siblings of current entry
        //

        status = IopSetupConfigurationTree(CurrentEntry->Sibling,
                                           Handle,
                                           WorkName,
                                           InterfaceType,
                                           BusDataType,
                                           BusNumber
                                           );

        return(status);
    } else {
        return(STATUS_SUCCESS);
    }
}

NTSTATUS
IopInitializeRegistryNode(
    IN PCONFIGURATION_COMPONENT_DATA CurrentEntry,
    IN HANDLE EnumRootHandle,
    IN PUNICODE_STRING WorkName,
    IN PUNICODE_STRING KeyName,
    IN ULONG Instance,
    IN INTERFACE_TYPE InterfaceType,
    IN BUS_DATA_TYPE BusDataType,
    IN ULONG BusNumber
    )

/*++

Routine Description:

    This routine creates a node for the current firmware component
    and puts component data to the data part of the node.

Arguments:

    CurrentEntry - Supplies a pointer to a configuration component.

    EnumRootHandle - Supplies the handle of Enum key under which we will build
        our new key.

    WorkName - Supplies a point to a unicode string which is the name of
        its parent node.

    KeyName - Suppiles a pointer to a UNICODE string which will be the name
        of the new key.

    Instance - Supplies an instance number of KeyName.

    InterfaceType - Specify the Interface type of the bus that the
        CurrentEntry component resides. (See BusNumber also)

    BusDataType - Specifies the configuration type of the bus.

    BusNumber - Specify the Bus Number of the bus that the CurrentEntry
        component resides on.  If Bus number is -1, it means InterfaceType
        and BusNumber are meaningless for this component.

Returns:

    None.

--*/
{

    NTSTATUS status;
    HANDLE handle, keyHandle;
    UNICODE_STRING unicodeName, unicodeValueName;
    PCONFIGURATION_COMPONENT component;
    PKEY_VALUE_FULL_INFORMATION keyValueInformation, serviceInfo = NULL;
    PWSTR service = (PWSTR)NULL, p;
    ULONG disposition, foundAlready, dataLength = 0;
    ULONG serviceLength = 0, tmpValue, emptyResource = 0;
    BOOLEAN newKey = FALSE;
    PCM_RESOURCE_LIST dataArea, configuration1;
    CHAR unicodeBuffer[20];
    PUCHAR resourceBuffer = IopPnpScratchBuffer2;
    BOOLEAN freeDataArea = FALSE, isDuplicated;

    component = &CurrentEntry->ComponentEntry;

    //
    // Open/Create a key under Enum/Root bransh. If fails,
    // exit (nothing we can do.)
    //

    status = IopOpenRegistryKeyPersist (
                     &keyHandle,
                     EnumRootHandle,
                     KeyName,
                     KEY_ALL_ACCESS,
                     TRUE,
                     &disposition
                     );
    if (!NT_SUCCESS(status)) {
        return status;
    }

#if 0  // not sure if we need it

    //
    // If the key is newly created, set its NewDevice value to TRUE so that
    // user mode Pnp mgr can initiate a device installation.  The NewDevice
    // value will be reset by user mode Pnp mgr.  SO , we don't touch it here.
    //

    if (disposition == REG_CREATED_NEW_KEY) {
        newKey = TRUE;
        PiWstrToUnicodeString(&unicodeValueName, REGSTR_VALUE_NEWDEVICE);
        tmpValue = 1;
        NtSetValueKey(
                    keyHandle,
                    &unicodeValueName,
                    TITLE_INDEX_VALUE,
                    REG_DWORD,
                    &tmpValue,
                    sizeof (tmpValue)
                    );

        PiWstrToUnicodeString(&unicodeValueName, REGSTR_VALUE_STATIC);
        NtSetValueKey(
                    keyHandle,
                    &unicodeValueName,
                    TITLE_INDEX_VALUE,
                    REG_DWORD,
                    &tmpValue,
                    sizeof(tmpValue)
                    );
    }

#endif

    if (component->Type > OtherController) {

        //
        // The current component is a peripheral.
        //

        //
        // Create a new instance key under KeyName
        //

        PiUlongToInstanceKeyUnicodeString(&unicodeName, unicodeBuffer, 20, Instance);
        status = IopOpenRegistryKeyPersist (
                     &handle,
                     keyHandle,
                     &unicodeName,
                     KEY_ALL_ACCESS,
                     TRUE,
                     &disposition
                     );
        NtClose(keyHandle);
        if (!NT_SUCCESS(status)) {
            goto init_Exit;
        }


        //
        //
        // Create all the default value entry for the newly created key.
        // Service =  (do NOT create)
        // BaseDevicePath = WorkName
        // FoundAtEnum = 1
        // InterfaceType = InterfaceType     (only for bus device)
        // SystemBusNumber = BusNumber       (only for bus device)
        // BusDataType = BusDataType         (only for bus device)
        //

        PiWstrToUnicodeString(&unicodeValueName, REGSTR_VALUE_BASEDEVICEPATH);
        p = WorkName->Buffer;
        p += WorkName->Length / sizeof(WCHAR);
        *p = UNICODE_NULL;
        NtSetValueKey(
                    handle,
                    &unicodeValueName,
                    TITLE_INDEX_VALUE,
                    REG_SZ,
                    WorkName->Buffer,
                    WorkName->Length + sizeof (UNICODE_NULL)
                    );

        PiWstrToUnicodeString(&unicodeValueName, REGSTR_VALUE_FOUNDATENUM);
        tmpValue = 1;
        NtSetValueKey(
                    handle,
                    &unicodeValueName,
                    TITLE_INDEX_VALUE,
                    REG_DWORD,
                    &tmpValue,
                    sizeof(tmpValue)
                    );

        PiWstrToUnicodeString(&unicodeValueName, REGSTR_VALUE_SYSTEMBUSNUMBER);
        tmpValue = BusNumber;
        NtSetValueKey(
                    handle,
                    &unicodeValueName,
                    TITLE_INDEX_VALUE,
                    REG_DWORD,
                    &tmpValue,
                    sizeof(tmpValue)
                    );

        PiWstrToUnicodeString(&unicodeValueName, REGSTR_VALUE_INTERFACETYPE);
        tmpValue = InterfaceType;
        NtSetValueKey(
                    handle,
                    &unicodeValueName,
                    TITLE_INDEX_VALUE,
                    REG_DWORD,
                    &tmpValue,
                    sizeof(tmpValue)
                    );

        PiWstrToUnicodeString(&unicodeValueName, REGSTR_VALUE_BUSDATATYPE);
        tmpValue = BusDataType;
        NtSetValueKey(
                    handle,
                    &unicodeValueName,
                    TITLE_INDEX_VALUE,
                    REG_DWORD,
                    &tmpValue,
                    sizeof(tmpValue)
                    );

        NtClose(handle);

        //
        // Append keyanme and instance key name to workname
        // (In fact, we don't need to do this because there
        // is nothing under peripheral component.)
        //
        //
        // Append KeyName to workname
        //

        p = WorkName->Buffer;
        p += WorkName->Length / sizeof(WCHAR);
        *p = OBJ_NAME_PATH_SEPARATOR;
        WorkName->Length += sizeof (WCHAR);
        RtlAppendStringToString((PSTRING)WorkName,
                                (PSTRING)KeyName);

    } else {

        //
        // Current component is a controller
        //

        //
        // Append KeyName to workname
        //

        p = WorkName->Buffer;
        p += WorkName->Length / sizeof(WCHAR);
        *p = OBJ_NAME_PATH_SEPARATOR;
        WorkName->Length += sizeof (WCHAR);
        RtlAppendStringToString((PSTRING)WorkName,
                                (PSTRING)KeyName);

        //
        // We need to convert the h/w tree configuration data format from
        // CM_PARTIAL_RESOURCE_DESCRIPTIOR to CM_RESOURCE_LIST.
        //

        if (CurrentEntry->ConfigurationData) {

            //
            // This component has configuration data, we copy the data
            // to our work area, add some more data items and copy the new
            // configuration data to the registry.
            //

            dataLength = component->ConfigurationDataLength +
                          FIELD_OFFSET(CM_FULL_RESOURCE_DESCRIPTOR,
                          PartialResourceList) +
                          FIELD_OFFSET(CM_RESOURCE_LIST, List);
            dataArea = (PCM_RESOURCE_LIST)resourceBuffer;

            //
            // Make sure our reserved area is big enough to hold the data.
            //

            if (dataLength > PNP_LARGE_SCRATCH_BUFFER_SIZE) {

                //
                // If reserved area is not big enough, we resize our reserved
                // area.  If, unfortunately, the reallocation fails, we simply
                // loss the configuration data of this particular component.
                //

                dataArea = (PCM_RESOURCE_LIST)ExAllocatePool(
                                                PagedPool,
                                                dataLength
                                                );

                if (dataArea) {
                    freeDataArea = TRUE;
                }
            }
            if (dataArea) {
                RtlMoveMemory((PUCHAR)&dataArea->List->PartialResourceList.Version,
                              CurrentEntry->ConfigurationData,
                              component->ConfigurationDataLength
                              );
                dataArea->Count = 1;
                dataArea->List[0].InterfaceType = InterfaceType;
                dataArea->List[0].BusNumber = BusNumber;
            }
        }

        if (CurrentEntry->ConfigurationData == NULL || !dataArea) {

            //
            // This component has NO configuration data (or we can't resize
            // our reserved area to hold the data), we simple add whatever
            // is required to set up a CM_FULL_RESOURCE_LIST.
            //

            dataArea = (PCM_RESOURCE_LIST)&emptyResource;
            dataLength = FIELD_OFFSET(CM_RESOURCE_LIST, List);
        }

        if (!newKey) {

            //
            // If the key exists already, we need to check if current entry
            // already being converted (most likely the answer is yes.).  If it already
            // converted, we simply set "FoundAtEnum=" to TRUE.  Otherwise, we will
            // create it.
            //

            tmpValue = 0;
            PiUlongToInstanceKeyUnicodeString(&unicodeName, unicodeBuffer, 20, tmpValue);
            status = IopOpenRegistryKey (&handle,
                                         keyHandle,
                                         &unicodeName,
                                         KEY_ALL_ACCESS,
                                         FALSE
                                         );
            while (NT_SUCCESS(status)) {

                //
                // if the current key has been Found/Enum'ed already, we need
                // to skip it.
                //

                foundAlready = 0;
                status = IopGetRegistryValue (handle,
                                              REGSTR_VALUE_FOUNDATENUM,
                                              &keyValueInformation);
                if (NT_SUCCESS(status)) {
                    if (keyValueInformation->DataLength != 0) {
                        foundAlready = *(PULONG)KEY_VALUE_DATA(keyValueInformation);
                    }
                    ExFreePool(keyValueInformation);
                }

                if (!foundAlready) {
                    keyValueInformation = NULL;
                    status = IopGetRegistryValue (handle,
                                                  REGSTR_VALUE_DETECTSIGNATURE,
                                                  &keyValueInformation);
                    if (NT_SUCCESS(status) && keyValueInformation->DataLength != 0) {
                        configuration1 = (PCM_RESOURCE_LIST)KEY_VALUE_DATA(keyValueInformation);
                    } else if (status == STATUS_OBJECT_NAME_NOT_FOUND ||
                               keyValueInformation->DataLength == 0) {

                        //
                        // If no "DetectSignature =" value entry, we set up an empty
                        // CM_RESOURCE_LIST.
                        //

                        configuration1 = (PCM_RESOURCE_LIST)&emptyResource;
                    }

                    //
                    // To detect ARC duplicated components, we should be able
                    // to simply compare the RAW resource list.  If they are the
                    // same *most likely* they are duplicates.  This includes
                    // the case that both resource list are empty.  (if they
                    // are empty, we should be able to simply pick up the
                    // key and use it.)
                    //

                    isDuplicated = IopIsDuplicatedResourceLists(
                                       configuration1,
                                       dataArea);
                    if (!isDuplicated) {

                        //
                        // BUGBUG We should also check for bus info.
                        //

                        isDuplicated = IopIsDuplicatedDevices(
                                               configuration1,
                                               dataArea,
                                               NULL,
                                               NULL
                                               );
                    }

                    if (keyValueInformation) {
                        ExFreePool(keyValueInformation);
                    }
                    if (isDuplicated) {
                        PiWstrToUnicodeString( &unicodeValueName, REGSTR_VALUE_FOUNDATENUM);
                        tmpValue = 1;
                        status = NtSetValueKey(handle,
                                               &unicodeValueName,
                                               TITLE_INDEX_VALUE,
                                               REG_DWORD,
                                               &tmpValue,
                                               sizeof(tmpValue)
                                               );
                        NtClose(handle);
                        NtClose(keyHandle);
                        goto init_Exit0;
                    }
                }
                NtClose(handle);
                tmpValue++;
                PiUlongToInstanceKeyUnicodeString(&unicodeName,
                                                  unicodeBuffer,
                                                  20,
                                                  tmpValue);
                status = IopOpenRegistryKey (&handle,
                                             keyHandle,
                                             &unicodeName,
                                             KEY_ALL_ACCESS,
                                             FALSE
                                             );
            }

            Instance = tmpValue;
        }

        //
        // We need to create the new instance key if we can come here...
        //

        PiUlongToInstanceKeyUnicodeString(&unicodeName, unicodeBuffer, 20, Instance);
        status = IopOpenRegistryKeyPersist (
                     &handle,
                     keyHandle,
                     &unicodeName,
                     KEY_ALL_ACCESS,
                     TRUE,
                     NULL
                     );
        NtClose(keyHandle);
        PNP_ASSERT(NT_SUCCESS(status), "IopInitRegistryNode: Fail to create new key.");
        if (!NT_SUCCESS(status)) {
            goto init_Exit;
        }

        //
        // Newly created key --
        //
        // Create all the default value entry for the newly created key.
        // Service =
#if 0
        // NewInstance = 1
#endif
        // FoundAtEnum = 1
        // Configuration =
        // InterfaceType = InterfaceType
        // SystemBusNumber = BusNumber
        // BusDataType = BusDataType
        //

#if 0
        PiWstrToUnicodeString(&unicodeValueName, REGSTR_VALUE_NEWINSTANCE);
        tmpValue = 1;
        NtSetValueKey(
                    handle,
                    &unicodeValueName,
                    TITLE_INDEX_VALUE,
                    REG_DWORD,
                    &tmpValue,
                    sizeof(tmpValue)
                    );
#endif

        PiWstrToUnicodeString(&unicodeValueName, REGSTR_VALUE_FOUNDATENUM);
        tmpValue = 1;
        NtSetValueKey(
                    handle,
                    &unicodeValueName,
                    TITLE_INDEX_VALUE,
                    REG_DWORD,
                    &tmpValue,
                    sizeof(tmpValue)
                    );

#if 0

        //
        // SystemBusNumber, InterfaceType and BusDataType are for Bus
        // devices only. For ntdetect/arc detected devices, we don't set
        // up bus devices.
        //

        PiWstrToUnicodeString(&unicodeValueName, REGSTR_VALUE_SYSTEMBUSNUMBER);
        tmpValue = BusNumber;
        NtSetValueKey(
                    handle,
                    &unicodeValueName,
                    TITLE_INDEX_VALUE,
                    REG_DWORD,
                    &tmpValue,
                    sizeof(tmpValue)
                    );

        PiWstrToUnicodeString(&unicodeValueName, REGSTR_VALUE_INTERFACETYPE);
        tmpValue = InterfaceType;
        NtSetValueKey(
                    handle,
                    &unicodeValueName,
                    TITLE_INDEX_VALUE,
                    REG_DWORD,
                    &tmpValue,
                    sizeof(tmpValue)
                    );

        PiWstrToUnicodeString(&unicodeValueName, REGSTR_VALUE_BUSDATATYPE);
        tmpValue = BusDataType;
        NtSetValueKey(
                    handle,
                    &unicodeValueName,
                    TITLE_INDEX_VALUE,
                    REG_DWORD,
                    &tmpValue,
                    sizeof(tmpValue)
                    );
#endif

        PiWstrToUnicodeString(&unicodeValueName, REGSTR_VALUE_CONFIGURATION);
        NtSetValueKey(
                    handle,
                    &unicodeValueName,
                    TITLE_INDEX_VALUE,
                    REG_RESOURCE_LIST,
                    dataArea,
                    dataLength
                    );

        PiWstrToUnicodeString(&unicodeValueName, REGSTR_VALUE_DETECTSIGNATURE);
        NtSetValueKey(
                    handle,
                    &unicodeValueName,
                    TITLE_INDEX_VALUE,
                    REG_RESOURCE_LIST,
                    dataArea,
                    dataLength
                    );
        NtClose(handle);
    }
    status = STATUS_SUCCESS;
init_Exit0:
    p = WorkName->Buffer;
    p += WorkName->Length / sizeof(WCHAR);
    *p = OBJ_NAME_PATH_SEPARATOR;
    WorkName->Length += sizeof (WCHAR);
    RtlAppendStringToString((PSTRING)WorkName,
                            (PSTRING)&unicodeName);
init_Exit:
    if (freeDataArea) {
        ExFreePool(dataArea);
    }
    if (serviceInfo) {
        ExFreePool(serviceInfo);
    }
    return(status);

}

#endif // _PNP_POWER_
NTSTATUS
IopInitServiceEnumList (
    VOID
    )

/*++

Routine Description:

    This routine scans through System\Enum\Root subtree and assigns device
    instances to their corresponding Service\name\Enum\Root entries.  Basically,
    this routine establishes the Enum branches of service list.

Arguments:

    None.

Return Value:

   The function value is the final status of the operation.

--*/

{
    NTSTATUS status;
    HANDLE baseHandle;
    UNICODE_STRING workName, tmpName;

    //
    // Open System\CurrentControlSet\Enum key and call worker routine to recursively
    // scan through the subkeys.
    //

    status = IopOpenRegistryKeyPersist(&baseHandle,
                                       NULL,
                                       &CmRegistryMachineSystemCurrentControlSetEnumRootName,
                                       KEY_READ,
                                       TRUE,
                                       NULL
                                       );

    if (NT_SUCCESS(status)) {

        workName.Buffer = (PWSTR)IopPnpScratchBuffer1;
        RtlFillMemory((PUCHAR)IopPnpScratchBuffer1, PNP_LARGE_SCRATCH_BUFFER_SIZE, 0);
        workName.MaximumLength = PNP_LARGE_SCRATCH_BUFFER_SIZE;
        workName.Length = 0;
#if 1   // only look at ROOT key
        PiWstrToUnicodeString(&tmpName, REGSTR_KEY_ROOTENUM);
        RtlAppendStringToString((PSTRING)&workName, (PSTRING)&tmpName);
#endif

        //
        // Enumerate all subkeys under the System\CCS\Enum\Root.
        //

        status = IopApplyFunctionToSubKeys(baseHandle,
                                           NULL,
                                           KEY_ALL_ACCESS,
                                           TRUE,
#if 0
                                           IopInitializeBusKey,
#else
                                           IopInitializeDeviceKey,
#endif
                                           &workName
                                           );
        NtClose(baseHandle);
    }
    return status;
}
#if 0

BOOLEAN
IopInitializeBusKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING KeyName,
    IN OUT PVOID WorkName
    )

/*++

Routine Description:

    This routine is a callback function for IopApplyFunctionToSubKeys.
    It is called for each subkey under HKLM\System\Enum.

Arguments:

    KeyHandle - Supplies a handle to this key.

    KeyName - Supplies the name of this key.

    WorkName - points to the unicodestring which describes the path up to
        this key.

Returns:

    TRUE to continue the enumeration.
    FALSE to abort it.

--*/
{
    USHORT length;
    PWSTR p;
    PUNICODE_STRING unicodeName = WorkName;
    NTSTATUS status;

    length = unicodeName->Length;

    p = unicodeName->Buffer;
    if ( unicodeName->Length / sizeof(WCHAR) != 0) {
        p += unicodeName->Length / sizeof(WCHAR);
        *p = OBJ_NAME_PATH_SEPARATOR;
        unicodeName->Length += sizeof (WCHAR);
    }

    RtlAppendStringToString((PSTRING)unicodeName, (PSTRING)KeyName);

    //
    // Enumerate all subkeys under the System\Enum.
    //

    status = IopApplyFunctionToSubKeys(KeyHandle,
                                       NULL,
                                       KEY_ALL_ACCESS,
                                       TRUE,
                                       IopInitializeDeviceKey,
                                       WorkName
                                       );
    unicodeName->Length = length;      // Should be zero
    return TRUE;
}
#endif  // 0

BOOLEAN
IopInitializeDeviceKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING KeyName,
    IN OUT PVOID WorkName
    )

/*++

Routine Description:

    This routine is a callback function for IopApplyFunctionToSubKeys.
    It is called for each subkey under HKLM\System\CCS\Enum\BusKey.

Arguments:

    KeyHandle - Supplies a handle to this key.

    KeyName - Supplies the name of this key.

    WorkName - points to the unicodestring which describes the path up to
        this key.

Returns:

    TRUE to continue the enumeration.
    FALSE to abort it.

--*/
{
    USHORT length;
    PWSTR p;
    PUNICODE_STRING unicodeName = WorkName;

    length = unicodeName->Length;

    p = unicodeName->Buffer;
    if ( unicodeName->Length / sizeof(WCHAR) != 0) {
        p += unicodeName->Length / sizeof(WCHAR);
        *p = OBJ_NAME_PATH_SEPARATOR;
        unicodeName->Length += sizeof (WCHAR);
    }

    RtlAppendStringToString((PSTRING)unicodeName, (PSTRING)KeyName);

    //
    // Enumerate all subkeys under the current device key.
    //

    IopApplyFunctionToSubKeys(KeyHandle,
                              NULL,
                              KEY_ALL_ACCESS,
                              TRUE,
                              IopInitializeDeviceInstanceKey,
                              WorkName
                              );
    unicodeName->Length = length;
    return TRUE;
}

BOOLEAN
IopInitializeDeviceInstanceKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING KeyName,
    IN OUT PVOID WorkName
    )

/*++

Routine Description:

    This routine is a callback function for IopApplyFunctionToSubKeys.
    It is called for each subkey under HKLM\System\Enum\BusKey\DeviceKey.

Arguments:

    KeyHandle - Supplies a handle to this key.

    KeyName - Supplies the name of this key.

    WorkName - points to the unicodestring which describes the path up to
        this key.

Returns:

    TRUE to continue the enumeration.
    FALSE to abort it.

--*/
{
    UNICODE_STRING unicodeName, serviceName;
    PKEY_VALUE_FULL_INFORMATION keyValueInformation;
    NTSTATUS status;
    BOOLEAN duplicate = FALSE;
    ULONG foundAtEnum, deviceFlags, instance, tmpValue1, tmpValue2;
    USHORT length;
    PUNICODE_STRING pUnicode;

    //
    // Get the "Problem" value entry to determine what we need to do with
    // the device instance key.
    //

    deviceFlags = 0;
    status = IopGetRegistryValue ( KeyHandle,
                                   REGSTR_VALUE_PROBLEM,
                                   &keyValueInformation
                                   );
    if (NT_SUCCESS(status)) {
        if ((keyValueInformation->Type == REG_DWORD) &&
            (keyValueInformation->DataLength >= sizeof(ULONG))) {
            deviceFlags = *(PULONG)KEY_VALUE_DATA(keyValueInformation);
        }
        ExFreePool(keyValueInformation);
    }

    if (deviceFlags == CM_PROB_MOVED) {

        //
        // If the device instance was moved, we simply delete the key.
        // The key will be deleted once the caller close the open handle.
        //

        NtDeleteKey(KeyHandle);
        return TRUE;
    }

    //
    // The device instance key exists.  We need to propagate the ConfigFlag
    // to problem and StatusFlags
    //

    deviceFlags = 0;
    status = IopGetRegistryValue(KeyHandle,
                                 REGSTR_VALUE_CONFIG_FLAGS,
                                 &keyValueInformation);
    if (NT_SUCCESS(status)) {
        if ((keyValueInformation->Type == REG_DWORD) &&
            (keyValueInformation->DataLength >= sizeof(ULONG))) {
            deviceFlags = *(PULONG)KEY_VALUE_DATA(keyValueInformation);
        }
        ExFreePool(keyValueInformation);
    }
    if (deviceFlags & CONFIGFLAG_REINSTALL) {

        tmpValue1 = CM_PROB_REINSTALL;      // Problem
        tmpValue2 = DN_HAS_PROBLEM;         // StatusFlags
    } else {
        tmpValue1 = tmpValue2 = 0;
    }
    PiWstrToUnicodeString(&unicodeName, REGSTR_VALUE_PROBLEM);
    NtSetValueKey(KeyHandle,
                  &unicodeName,
                  TITLE_INDEX_VALUE,
                  REG_DWORD,
                  &tmpValue1,
                  sizeof(tmpValue1)
                  );

    PiWstrToUnicodeString(&unicodeName, REGSTR_VALUE_STATUSFLAGS);
    NtSetValueKey(KeyHandle,
                  &unicodeName,
                  TITLE_INDEX_VALUE,
                  REG_DWORD,
                  &tmpValue2,
                  sizeof(tmpValue2)
                  );

    //
    // Get the "DuplicateOf" value entry to determine if the device instance
    // should be registered.  If the device instance is duplicate, We don't
    // add it to its service key's enum branch.
    //

    status = IopGetRegistryValue ( KeyHandle,
                                   REGSTR_VALUE_DUPLICATEOF,
                                   &keyValueInformation
                                   );
    if (NT_SUCCESS(status)) {
        if ((keyValueInformation->Type == REG_SZ) &&
            (keyValueInformation->DataLength > 0)) {
            duplicate = TRUE;
        }
        ExFreePool(keyValueInformation);
    }

    if (!duplicate) {

        //
        // Combine WorkName and KeyName to form device instance path
        // and register this device instance by
        // constructing new value entry for ServiceKeyName\Enum key.
        // i.e., <Number> = <PathToSystemEnumBranch>
        //

        pUnicode = (PUNICODE_STRING)WorkName;
        length = pUnicode->Length;                  // Save WorkName
        if (pUnicode->Buffer[pUnicode->Length / sizeof(WCHAR) - 1] != OBJ_NAME_PATH_SEPARATOR) {
            pUnicode->Buffer[pUnicode->Length / sizeof(WCHAR)] = OBJ_NAME_PATH_SEPARATOR;
            pUnicode->Length += 2;
        }
        RtlAppendStringToString((PSTRING)pUnicode, (PSTRING)KeyName);
        PpDeviceRegistration(pUnicode, TRUE);
        pUnicode->Length = length;                  // Restore WorkName
    }

    //
    // Get the "Service=" value entry from KeyHandle
    //

    keyValueInformation = NULL;
    serviceName.Length = 0;
    status = IopGetRegistryValue ( KeyHandle,
                                   REGSTR_VALUE_SERVICE,
                                   &keyValueInformation
                                   );
    if (NT_SUCCESS(status)) {

        //
        // Append the new instance to its corresponding
        // Service\Name\Enum.
        //

        if ((keyValueInformation->Type == REG_SZ) &&
            (keyValueInformation->DataLength != 0)) {

            //
            // Set up ServiceKeyName unicode string
            //

            IopRegistryDataToUnicodeString(
                              &serviceName,
                              (PWSTR)KEY_VALUE_DATA(keyValueInformation),
                              keyValueInformation->DataLength
                              );
        }

        //
        // Do not Free keyValueInformation
        //

    }

    //
    // The Pnp mgr set FoundAtEnum to 0 for everything under system\ccs\enum.
    // For the stuff under Root we need to set them to 1 except if their
    // CsConfigFlags was set to CSCONFIGFLAG_DO_NOT_CREATE.
    //

    foundAtEnum = 1;
    status = RtlUnicodeStringToInteger(KeyName, 10, &instance);
    if (NT_SUCCESS(status)) {
        if (serviceName.Length != 0) {
            status = IopGetDeviceInstanceCsConfigFlags(
                         &serviceName,
                         instance,
                         &deviceFlags
                         );

            if (NT_SUCCESS(status) && (deviceFlags & CSCONFIGFLAG_DO_NOT_CREATE)) {
                foundAtEnum = 0;
            }
        }
    }
    if (keyValueInformation) {
        ExFreePool(keyValueInformation);
    }
    PiWstrToUnicodeString(&unicodeName, REGSTR_VALUE_FOUNDATENUM);
    NtSetValueKey(KeyHandle,
                  &unicodeName,
                  TITLE_INDEX_VALUE,
                  REG_DWORD,
                  &foundAtEnum,
                  sizeof(foundAtEnum)
                  );

    //
    // Clean up "NtLogicalDevicePaths=" and "NtPhysicalDevicePaths=" of this key
    //

    PiWstrToUnicodeString(&unicodeName, REGSTR_VALUE_NT_PHYSICAL_DEVICE_PATHS);
    NtDeleteValueKey(KeyHandle, &unicodeName);

    PiWstrToUnicodeString(&unicodeName, REGSTR_VALUE_NT_LOGICAL_DEVICE_PATHS);
    NtDeleteValueKey(KeyHandle, &unicodeName);

    return TRUE;
}

NTSTATUS
IopInitializePlugPlayServices(
    IN PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine initializes kernel mode Plug and Play services.

Arguments:

    LoaderBlock - supplies a pointer to the LoaderBlock passed in from the
        OS Loader.

Returns:

    NTSTATUS code for sucess or reason of failure.

--*/
{
    NTSTATUS status;
    HANDLE hTreeHandle, parentHandle, handle;
    UNICODE_STRING unicodeName;
    ULONG foundAtEnum;

    //
    // Allocate two one-page scratch buffers to be used by our
    // initialization code.  This avoids constant pool allocations.
    //

    IopPnpScratchBuffer1 = ExAllocatePool(PagedPool, PNP_LARGE_SCRATCH_BUFFER_SIZE);
    if (!IopPnpScratchBuffer1) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    IopPnpScratchBuffer2 = ExAllocatePool(PagedPool, PNP_LARGE_SCRATCH_BUFFER_SIZE);
    if (!IopPnpScratchBuffer2) {
        ExFreePool(IopPnpScratchBuffer1);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Next open/create System\CurrentControlSet\Enum\Root key.
    //

    status = IopOpenRegistryKey (
                 &parentHandle,
                 NULL,
                 &CmRegistryMachineSystemCurrentControlSet,
                 KEY_ALL_ACCESS,
                 FALSE
                 );
    if (!NT_SUCCESS(status)) {
        goto init_Exit;
    }

    PiWstrToUnicodeString(&unicodeName, REGSTR_KEY_ENUM);
    status = IopOpenRegistryKeyPersist (
                 &handle,
                 parentHandle,
                 &unicodeName,
                 KEY_ALL_ACCESS,
                 TRUE,
                 NULL
                 );
    NtClose(parentHandle);
    if (!NT_SUCCESS(status)) {
        goto init_Exit;
    }

    parentHandle = handle;
    PiWstrToUnicodeString(&unicodeName, REGSTR_KEY_ROOTENUM);
    status = IopOpenRegistryKeyPersist (
                 &handle,
                 parentHandle,
                 &unicodeName,
                 KEY_ALL_ACCESS,
                 TRUE,
                 NULL
                 );
    NtClose(parentHandle);
    if (!NT_SUCCESS(status)) {
        goto init_Exit;
    }
    NtClose(handle);

#if _PNP_POWER_

    //
    // Convert Hardware/Firmware tree to Pnp required format.
    //

    status = IopInitializeHardwareConfiguration(LoaderBlock);
    if (!NT_SUCCESS(status)) {
        goto init_Exit;
    }
#endif // _PNP_POWER_

    //
    // Initialize the FoundAtEnum= value entry to 1 for HTREE\ROOT\0
    //

    status = IopOpenRegistryKey(&handle,
                                NULL,
                                &CmRegistryMachineSystemCurrentControlSetEnumName,
                                KEY_ALL_ACCESS,
                                FALSE
                                );
    if (NT_SUCCESS(status)) {
        PiWstrToUnicodeString(&unicodeName, REGSTR_VALUE_HTREE_ROOT_0);
        status = IopOpenRegistryKeyPersist(&hTreeHandle,
                                           handle,
                                           &unicodeName,
                                           KEY_ALL_ACCESS,
                                           TRUE,
                                           NULL
                                           );
        NtClose(handle);
        if (NT_SUCCESS(status)) {
            PiWstrToUnicodeString(&unicodeName, REGSTR_VALUE_FOUNDATENUM);
            foundAtEnum = 1;
            NtSetValueKey(hTreeHandle,
                          &unicodeName,
                          TITLE_INDEX_VALUE,
                          REG_DWORD,
                          &foundAtEnum,
                          sizeof(foundAtEnum)
                          );
            NtClose(hTreeHandle);
        }
    }

    //
    // Set up Enum subkey for service list.
    //

    status = IopInitServiceEnumList();

init_Exit:

    //
    // Free our scratch buffers and exit.
    //

    ExFreePool(IopPnpScratchBuffer1);
    ExFreePool(IopPnpScratchBuffer2);

    return status;
}

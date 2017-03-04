/*++

Copyright (c) 2016  Microsoft Corporation
Copyright (c) 2016  OpenNT Project

Module Name:

   intrlock.c

Abstract:

   This module implements functions to support interlocked operations.
   Interlocked operations can only operate on nonpaged data.

Author:

    Stephanos Ioannidis (stephanos) 07-May-2016

Revision History:

--*/

#include "exp.h"

#undef InterlockedIncrement
#undef InterlockedDecrement
#undef InterlockedExchange
#undef InterlockedExchangeAdd
#undef InterlockedCompareExchange

LONG
InterlockedIncrement(
    IN OUT PLONG Addend
    )
{
    *Addend = *Addend + 1;
    return *Addend;
}

LONG
InterlockedDecrement(
    IN OUT PLONG Addend
    )
{
    *Addend = *Addend - 1;
    return *Addend;
}

LONG
InterlockedExchange(
    IN OUT PLONG Target,
    IN LONG Value
    )
{
    LONG InitialValue = *Target;
    *Target = Value;
    return InitialValue;
}

NTKERNELAPI
LONG
InterlockedExchangeAdd(
    IN OUT PLONG Addend,
    IN LONG Value
    )
{
    LONG InitialValue = *Addend;
    *Addend = *Addend + Value;
    return InitialValue;
}

NTKERNELAPI
PVOID
InterlockedCompareExchange (
    IN OUT PVOID *Destination,
    IN PVOID Exchange,
    IN PVOID Comperand
    )
{
    PVOID InitialValue = *Destination;
    if (*Destination == Comperand)
    {
        *Destination = Exchange;
    }
    return InitialValue;
}

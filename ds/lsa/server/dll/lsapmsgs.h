/*++ BUILD Version: 0001    // Increment this if a change has global effects

Copyright (c) 1991-1993  Microsoft Corporation

Module Name:

    lsapmsgs.mc

Abstract:

    LSA localizable text

Author:

    Jim Kelly  1-Apr-1993

Revision History:

Notes:


--*/

#ifndef _LSAPMSGS_
#define _LSAPMSGS_

/*lint -save -e767 */  // Don't complain about different definitions // winnt
//
// Force facility code message to be placed in .h file
//
//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//


//
// MessageId: LSAP_UNUSED_MESSAGE
//
// MessageText:
//
//  LSAP_UNUSED_MESSAGE
//
#define LSAP_UNUSED_MESSAGE              ((DWORD)0x00001FFFL)

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//                                                                        //
//                         Well Known SID & RID Names                     //
//
//
//                                                                        //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
//
// MessageId: LSAP_SID_NAME_NULL
//
// MessageText:
//
//  NULL SID
//
#define LSAP_SID_NAME_NULL               ((DWORD)0x00002000L)

//
// MessageId: LSAP_SID_NAME_WORLD
//
// MessageText:
//
//  Everyone
//
#define LSAP_SID_NAME_WORLD              ((DWORD)0x00002001L)

//
// MessageId: LSAP_SID_NAME_LOCAL
//
// MessageText:
//
//  LOCAL
//
#define LSAP_SID_NAME_LOCAL              ((DWORD)0x00002002L)

//
// MessageId: LSAP_SID_NAME_CREATOR_OWNER
//
// MessageText:
//
//  CREATOR OWNER
//
#define LSAP_SID_NAME_CREATOR_OWNER      ((DWORD)0x00002003L)

//
// MessageId: LSAP_SID_NAME_CREATOR_GROUP
//
// MessageText:
//
//  CREATOR GROUP
//
#define LSAP_SID_NAME_CREATOR_GROUP      ((DWORD)0x00002004L)

//
// MessageId: LSAP_SID_NAME_NT_DOMAIN
//
// MessageText:
//
//  NT Pseudo Domain
//
#define LSAP_SID_NAME_NT_DOMAIN          ((DWORD)0x00002005L)

//
// MessageId: LSAP_SID_NAME_NT_AUTHORITY
//
// MessageText:
//
//  NT AUTHORITY
//
#define LSAP_SID_NAME_NT_AUTHORITY       ((DWORD)0x00002006L)

//
// MessageId: LSAP_SID_NAME_DIALUP
//
// MessageText:
//
//  DIALUP
//
#define LSAP_SID_NAME_DIALUP             ((DWORD)0x00002007L)

//
// MessageId: LSAP_SID_NAME_NETWORK
//
// MessageText:
//
//  NETWORK
//
#define LSAP_SID_NAME_NETWORK            ((DWORD)0x00002008L)

//
// MessageId: LSAP_SID_NAME_BATCH
//
// MessageText:
//
//  BATCH
//
#define LSAP_SID_NAME_BATCH              ((DWORD)0x00002009L)

//
// MessageId: LSAP_SID_NAME_INTERACTIVE
//
// MessageText:
//
//  INTERACTIVE
//
#define LSAP_SID_NAME_INTERACTIVE        ((DWORD)0x0000200AL)

//
// MessageId: LSAP_SID_NAME_SERVICE
//
// MessageText:
//
//  SERVICE
//
#define LSAP_SID_NAME_SERVICE            ((DWORD)0x0000200BL)

//
// MessageId: LSAP_SID_NAME_BUILTIN
//
// MessageText:
//
//  BUILTIN
//
#define LSAP_SID_NAME_BUILTIN            ((DWORD)0x0000200CL)

//
// MessageId: LSAP_SID_NAME_SYSTEM
//
// MessageText:
//
//  SYSTEM
//
#define LSAP_SID_NAME_SYSTEM             ((DWORD)0x0000200DL)

//
// MessageId: LSAP_SID_NAME_ANONYMOUS
//
// MessageText:
//
//  ANONYMOUS LOGON
//
#define LSAP_SID_NAME_ANONYMOUS          ((DWORD)0x0000200EL)

//
// MessageId: LSAP_SID_NAME_CREATOR_OWNER_SERVER
//
// MessageText:
//
//  CREATOR OWNER SERVER
//
#define LSAP_SID_NAME_CREATOR_OWNER_SERVER ((DWORD)0x0000200FL)

//
// MessageId: LSAP_SID_NAME_CREATOR_GROUP_SERVER
//
// MessageText:
//
//  CREATOR GROUP SERVER
//
#define LSAP_SID_NAME_CREATOR_GROUP_SERVER ((DWORD)0x00002010L)

//
// MessageId: LSAP_SID_NAME_SERVER
//
// MessageText:
//
//  SERVER LOGON
//
#define LSAP_SID_NAME_SERVER             ((DWORD)0x00002011L)

/*lint -restore */  // Resume checking for different macro definitions // winnt


#endif // _LSAPMSGS_

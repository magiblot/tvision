/*++

Copyright (c) 1990-1993  Microsoft Corporation

Module Name:

    WinSpool.h

Abstract:

    Header file for Print APIs

Author:

    Dave Snipp (DaveSn) 15-Mar-1991

Revision History:

--*/

#ifndef __WINSPOOL_H
#define __WINSPOOL_H

#ifndef __WINBASE_H
#include <winbase.h>
#endif

#ifndef __WINGDI_H
#include <wingdi.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _PRINTER_INFO_1A {
    DWORD   Flags;
    LPSTR   pDescription;
    LPSTR   pName;
    LPSTR   pComment;
} PRINTER_INFO_1A, *PPRINTER_INFO_1A, *LPPRINTER_INFO_1A;

typedef struct _PRINTER_INFO_1W {
    DWORD   Flags;
    LPWSTR  pDescription;
    LPWSTR  pName;
    LPWSTR  pComment;
} PRINTER_INFO_1W, *PPRINTER_INFO_1W, *LPPRINTER_INFO_1W;

#ifdef UNICODE
#define PRINTER_INFO_1 PRINTER_INFO_1W
#define PPRINTER_INFO_1 PPRINTER_INFO_1W
#define LPPRINTER_INFO_1 LPPRINTER_INFO_1W
#else
#define PRINTER_INFO_1 PRINTER_INFO_1A
#define PPRINTER_INFO_1 PPRINTER_INFO_1A
#define LPPRINTER_INFO_1 LPPRINTER_INFO_1A
#endif // UNICODE

typedef struct _PRINTER_INFO_2A {
    LPSTR   pServerName;
    LPSTR   pPrinterName;
    LPSTR   pShareName;
    LPSTR   pPortName;
    LPSTR   pDriverName;
    LPSTR   pComment;
    LPSTR   pLocation;
    LPDEVMODEA pDevMode;
    LPSTR   pSepFile;
    LPSTR   pPrintProcessor;
    LPSTR   pDatatype;
    LPSTR   pParameters;
    PSECURITY_DESCRIPTOR pSecurityDescriptor;
    DWORD   Attributes;
    DWORD   Priority;
    DWORD   DefaultPriority;
    DWORD   StartTime;
    DWORD   UntilTime;
    DWORD   Status;
    DWORD   cJobs;
    DWORD   AveragePPM;
} PRINTER_INFO_2A, *PPRINTER_INFO_2A, *LPPRINTER_INFO_2A;

typedef struct _PRINTER_INFO_2W {
    LPWSTR  pServerName;
    LPWSTR  pPrinterName;
    LPWSTR  pShareName;
    LPWSTR  pPortName;
    LPWSTR  pDriverName;
    LPWSTR  pComment;
    LPWSTR  pLocation;
    LPDEVMODEW pDevMode;
    LPWSTR  pSepFile;
    LPWSTR  pPrintProcessor;
    LPWSTR  pDatatype;
    LPWSTR  pParameters;
    PSECURITY_DESCRIPTOR pSecurityDescriptor;
    DWORD   Attributes;
    DWORD   Priority;
    DWORD   DefaultPriority;
    DWORD   StartTime;
    DWORD   UntilTime;
    DWORD   Status;
    DWORD   cJobs;
    DWORD   AveragePPM;
} PRINTER_INFO_2W, *PPRINTER_INFO_2W, *LPPRINTER_INFO_2W;

#ifdef UNICODE
#define PRINTER_INFO_2 PRINTER_INFO_2W
#define PPRINTER_INFO_2 PPRINTER_INFO_2W
#define LPPRINTER_INFO_2 LPPRINTER_INFO_2W
#else
#define PRINTER_INFO_2 PRINTER_INFO_2A
#define PPRINTER_INFO_2 PPRINTER_INFO_2A
#define LPPRINTER_INFO_2 LPPRINTER_INFO_2A
#endif // UNICODE

typedef struct _PRINTER_INFO_3 {
    PSECURITY_DESCRIPTOR pSecurityDescriptor;
} PRINTER_INFO_3, *PPRINTER_INFO_3, *LPPRINTER_INFO_3;

#define PRINTER_CONTROL_PAUSE            1
#define PRINTER_CONTROL_RESUME           2
#define PRINTER_CONTROL_PURGE            3

#define PRINTER_STATUS_PAUSED            0x00000001
#define PRINTER_STATUS_ERROR             0x00000002
#define PRINTER_STATUS_PENDING_DELETION  0x00000004
#define PRINTER_STATUS_PAPER_JAM         0x00000008
#define PRINTER_STATUS_PAPER_OUT         0x00000010
#define PRINTER_STATUS_MANUAL_FEED       0x00000020
#define PRINTER_STATUS_PAPER_PROBLEM     0x00000040
#define PRINTER_STATUS_OFFLINE           0x00000080
#define PRINTER_STATUS_IO_ACTIVE         0x00000100
#define PRINTER_STATUS_BUSY              0x00000200
#define PRINTER_STATUS_PRINTING          0x00000400
#define PRINTER_STATUS_OUTPUT_BIN_FULL   0x00000800
#define PRINTER_STATUS_NOT_AVAILABLE     0x00001000
#define PRINTER_STATUS_WAITING           0x00002000
#define PRINTER_STATUS_PROCESSING        0x00004000
#define PRINTER_STATUS_INITIALIZING      0x00008000
#define PRINTER_STATUS_WARMING_UP        0x00010000
#define PRINTER_STATUS_TONER_LOW         0x00020000
#define PRINTER_STATUS_NO_TONER          0x00040000
#define PRINTER_STATUS_PAGE_PUNT         0x00080000
#define PRINTER_STATUS_USER_INTERVENTION 0x00100000
#define PRINTER_STATUS_OUT_OF_MEMORY     0x00200000
#define PRINTER_STATUS_DOOR_OPEN         0x00400000


#define PRINTER_ATTRIBUTE_QUEUED         0x00000001
#define PRINTER_ATTRIBUTE_DIRECT         0x00000002
#define PRINTER_ATTRIBUTE_DEFAULT        0x00000004
#define PRINTER_ATTRIBUTE_SHARED         0x00000008
#define PRINTER_ATTRIBUTE_NETWORK        0x00000010
#define PRINTER_ATTRIBUTE_HIDDEN         0x00000020
#define PRINTER_ATTRIBUTE_LOCAL          0x00000040

#define NO_PRIORITY   0
#define MAX_PRIORITY 99
#define MIN_PRIORITY  1
#define DEF_PRIORITY  1

typedef struct _JOB_INFO_1A {
   DWORD    JobId;
   LPSTR    pPrinterName;
   LPSTR    pMachineName;
   LPSTR    pUserName;
   LPSTR    pDocument;
   LPSTR    pDatatype;
   LPSTR    pStatus;
   DWORD    Status;
   DWORD    Priority;
   DWORD    Position;
   DWORD    TotalPages;
   DWORD    PagesPrinted;
   SYSTEMTIME Submitted;
} JOB_INFO_1A, *PJOB_INFO_1A, *LPJOB_INFO_1A;

typedef struct _JOB_INFO_1W {
   DWORD    JobId;
   LPWSTR   pPrinterName;
   LPWSTR   pMachineName;
   LPWSTR   pUserName;
   LPWSTR   pDocument;
   LPWSTR   pDatatype;
   LPWSTR   pStatus;
   DWORD    Status;
   DWORD    Priority;
   DWORD    Position;
   DWORD    TotalPages;
   DWORD    PagesPrinted;
   SYSTEMTIME Submitted;
} JOB_INFO_1W, *PJOB_INFO_1W, *LPJOB_INFO_1W;

#ifdef UNICODE
#define JOB_INFO_1 JOB_INFO_1W
#define PJOB_INFO_1 PJOB_INFO_1W
#define LPJOB_INFO_1 LPJOB_INFO_1W
#else
#define JOB_INFO_1 JOB_INFO_1A
#define PJOB_INFO_1 PJOB_INFO_1A
#define LPJOB_INFO_1 LPJOB_INFO_1A
#endif // UNICODE

typedef struct _JOB_INFO_2A {
   DWORD    JobId;
   LPSTR    pPrinterName;
   LPSTR    pMachineName;
   LPSTR    pUserName;
   LPSTR    pDocument;
   LPSTR    pNotifyName;
   LPSTR    pDatatype;
   LPSTR    pPrintProcessor;
   LPSTR    pParameters;
   LPSTR    pDriverName;
   LPDEVMODE pDevMode;
   LPSTR    pStatus;
   PSECURITY_DESCRIPTOR pSecurityDescriptor;
   DWORD    Status;
   DWORD    Priority;
   DWORD    Position;
   DWORD    StartTime;
   DWORD    UntilTime;
   DWORD    TotalPages;
   DWORD    Size;
   SYSTEMTIME Submitted;    // Time the job was spooled
   DWORD    Time;           // How many seconds the job has been printing
   DWORD    PagesPrinted;
} JOB_INFO_2A, *PJOB_INFO_2A, *LPJOB_INFO_2A;

typedef struct _JOB_INFO_2W {
   DWORD    JobId;
   LPWSTR   pPrinterName;
   LPWSTR   pMachineName;
   LPWSTR   pUserName;
   LPWSTR   pDocument;
   LPWSTR   pNotifyName;
   LPWSTR   pDatatype;
   LPWSTR   pPrintProcessor;
   LPWSTR   pParameters;
   LPWSTR   pDriverName;
   LPDEVMODE pDevMode;
   LPWSTR   pStatus;
   PSECURITY_DESCRIPTOR pSecurityDescriptor;
   DWORD    Status;
   DWORD    Priority;
   DWORD    Position;
   DWORD    StartTime;
   DWORD    UntilTime;
   DWORD    TotalPages;
   DWORD    Size;
   SYSTEMTIME Submitted;    // Time the job was spooled
   DWORD    Time;           // How many seconds the job has been printing
   DWORD    PagesPrinted;
} JOB_INFO_2W, *PJOB_INFO_2W, *LPJOB_INFO_2W;

#ifdef UNICODE
#define JOB_INFO_2 JOB_INFO_2W
#define PJOB_INFO_2 PJOB_INFO_2W
#define LPJOB_INFO_2 LPJOB_INFO_2W
#else
#define JOB_INFO_2 JOB_INFO_2A
#define PJOB_INFO_2 PJOB_INFO_2A
#define LPJOB_INFO_2 LPJOB_INFO_2A
#endif // UNICODE

#define JOB_CONTROL_PAUSE              1
#define JOB_CONTROL_RESUME             2
#define JOB_CONTROL_CANCEL             3
#define JOB_CONTROL_RESTART            4

#define JOB_STATUS_PAUSED     0x00000001
#define JOB_STATUS_ERROR      0x00000002
#define JOB_STATUS_DELETING   0x00000004
#define JOB_STATUS_SPOOLING   0x00000008
#define JOB_STATUS_PRINTING   0x00000010
#define JOB_STATUS_OFFLINE    0x00000020
#define JOB_STATUS_PAPEROUT   0x00000040
#define JOB_STATUS_PRINTED    0x00000080

#define JOB_POSITION_UNSPECIFIED       0

typedef struct _ADDJOB_INFO_1A {
    LPSTR   Path;
    DWORD   JobId;
} ADDJOB_INFO_1A, *PADDJOB_INFO_1A, *LPADDJOB_INFO_1A;

typedef struct _ADDJOB_INFO_1W {
    LPWSTR  Path;
    DWORD   JobId;
} ADDJOB_INFO_1W, *PADDJOB_INFO_1W, *LPADDJOB_INFO_1W;

#ifdef UNICODE
#define ADDJOB_INFO_1 ADDJOB_INFO_1W
#define PADDJOB_INFO_1 PADDJOB_INFO_1W
#define LPADDJOB_INFO_1 LPADDJOB_INFO_1W
#else
#define ADDJOB_INFO_1 ADDJOB_INFO_1A
#define PADDJOB_INFO_1 PADDJOB_INFO_1A
#define LPADDJOB_INFO_1 LPADDJOB_INFO_1A
#endif // UNICODE

typedef struct _DRIVER_INFO_1A {
    LPSTR   pName;              // QMS 810
} DRIVER_INFO_1A, *PDRIVER_INFO_1A, *LPDRIVER_INFO_1A;

typedef struct _DRIVER_INFO_1W {
    LPWSTR  pName;              // QMS 810
} DRIVER_INFO_1W, *PDRIVER_INFO_1W, *LPDRIVER_INFO_1W;

#ifdef UNICODE
#define DRIVER_INFO_1 DRIVER_INFO_1W
#define PDRIVER_INFO_1 PDRIVER_INFO_1W
#define LPDRIVER_INFO_1 LPDRIVER_INFO_1W
#else
#define DRIVER_INFO_1 DRIVER_INFO_1A
#define PDRIVER_INFO_1 PDRIVER_INFO_1A
#define LPDRIVER_INFO_1 LPDRIVER_INFO_1A
#endif // UNICODE

typedef struct _DRIVER_INFO_2A {
    DWORD   cVersion;
    LPSTR   pName;              // QMS 810
    LPSTR   pEnvironment;       // Win32 x86
    LPSTR   pDriverPath;        // c:\drivers\pscript.dll
    LPSTR   pDataFile;          // c:\drivers\QMS810.PPD
    LPSTR   pConfigFile;        // c:\drivers\PSCRPTUI.DLL
} DRIVER_INFO_2A, *PDRIVER_INFO_2A, *LPDRIVER_INFO_2A;

typedef struct _DRIVER_INFO_2W {
    DWORD   cVersion;
    LPWSTR  pName;              // QMS 810
    LPWSTR  pEnvironment;       // Win32 x86
    LPWSTR  pDriverPath;        // c:\drivers\pscript.dll
    LPWSTR  pDataFile;          // c:\drivers\QMS810.PPD
    LPWSTR  pConfigFile;        // c:\drivers\PSCRPTUI.DLL
} DRIVER_INFO_2W, *PDRIVER_INFO_2W, *LPDRIVER_INFO_2W;

#ifdef UNICODE
#define DRIVER_INFO_2 DRIVER_INFO_2W
#define PDRIVER_INFO_2 PDRIVER_INFO_2W
#define LPDRIVER_INFO_2 LPDRIVER_INFO_2W
#else
#define DRIVER_INFO_2 DRIVER_INFO_2A
#define PDRIVER_INFO_2 PDRIVER_INFO_2A
#define LPDRIVER_INFO_2 LPDRIVER_INFO_2A
#endif // UNICODE

typedef struct _DOC_INFO_1A {
    LPSTR   pDocName;
    LPSTR   pOutputFile;
    LPSTR   pDatatype;
} DOC_INFO_1A, *PDOC_INFO_1A, *LPDOC_INFO_1A;

typedef struct _DOC_INFO_1W {
    LPWSTR  pDocName;
    LPWSTR  pOutputFile;
    LPWSTR  pDatatype;
} DOC_INFO_1W, *PDOC_INFO_1W, *LPDOC_INFO_1W;

#ifdef UNICODE
#define DOC_INFO_1 DOC_INFO_1W
#define PDOC_INFO_1 PDOC_INFO_1W
#define LPDOC_INFO_1 LPDOC_INFO_1W
#else
#define DOC_INFO_1 DOC_INFO_1A
#define PDOC_INFO_1 PDOC_INFO_1A
#define LPDOC_INFO_1 LPDOC_INFO_1A
#endif // UNICODE

typedef struct _FORM_INFO_1A {
    DWORD   Flags;
    LPSTR   pName;
    SIZEL   Size;
    RECTL   ImageableArea;
} FORM_INFO_1A, *PFORM_INFO_1A, *LPFORM_INFO_1A;

typedef struct _FORM_INFO_1W {
    DWORD   Flags;
    LPWSTR  pName;
    SIZEL   Size;
    RECTL   ImageableArea;
} FORM_INFO_1W, *PFORM_INFO_1W, *LPFORM_INFO_1W;

#ifdef UNICODE
#define FORM_INFO_1 FORM_INFO_1W
#define PFORM_INFO_1 PFORM_INFO_1W
#define LPFORM_INFO_1 LPFORM_INFO_1W
#else
#define FORM_INFO_1 FORM_INFO_1A
#define PFORM_INFO_1 PFORM_INFO_1A
#define LPFORM_INFO_1 LPFORM_INFO_1A
#endif // UNICODE

#define FORM_BUILTIN    0x00000001


typedef struct _PRINTPROCESSOR_INFO_1A {
    LPSTR   pName;
} PRINTPROCESSOR_INFO_1A, *PPRINTPROCESSOR_INFO_1A, *LPPRINTPROCESSOR_INFO_1A;

typedef struct _PRINTPROCESSOR_INFO_1W {
    LPWSTR  pName;
} PRINTPROCESSOR_INFO_1W, *PPRINTPROCESSOR_INFO_1W, *LPPRINTPROCESSOR_INFO_1W;

#ifdef UNICODE
#define PRINTPROCESSOR_INFO_1 PRINTPROCESSOR_INFO_1W
#define PPRINTPROCESSOR_INFO_1 PPRINTPROCESSOR_INFO_1W
#define LPPRINTPROCESSOR_INFO_1 LPPRINTPROCESSOR_INFO_1W
#else
#define PRINTPROCESSOR_INFO_1 PRINTPROCESSOR_INFO_1A
#define PPRINTPROCESSOR_INFO_1 PPRINTPROCESSOR_INFO_1A
#define LPPRINTPROCESSOR_INFO_1 LPPRINTPROCESSOR_INFO_1A
#endif // UNICODE

typedef struct _PORT_INFO_1A {
    LPSTR   pName;
} PORT_INFO_1A, *PPORT_INFO_1A, *LPPORT_INFO_1A;

typedef struct _PORT_INFO_1W {
    LPWSTR  pName;
} PORT_INFO_1W, *PPORT_INFO_1W, *LPPORT_INFO_1W;

#ifdef UNICODE
#define PORT_INFO_1 PORT_INFO_1W
#define PPORT_INFO_1 PPORT_INFO_1W
#define LPPORT_INFO_1 LPPORT_INFO_1W
#else
#define PORT_INFO_1 PORT_INFO_1A
#define PPORT_INFO_1 PPORT_INFO_1A
#define LPPORT_INFO_1 LPPORT_INFO_1A
#endif // UNICODE

typedef struct _MONITOR_INFO_1A {
    LPSTR   pName;
} MONITOR_INFO_1A, *PMONITOR_INFO_1A, *LPMONITOR_INFO_1A;

typedef struct _MONITOR_INFO_1W {
    LPWSTR  pName;
} MONITOR_INFO_1W, *PMONITOR_INFO_1W, *LPMONITOR_INFO_1W;

#ifdef UNICODE
#define MONITOR_INFO_1 MONITOR_INFO_1W
#define PMONITOR_INFO_1 PMONITOR_INFO_1W
#define LPMONITOR_INFO_1 LPMONITOR_INFO_1W
#else
#define MONITOR_INFO_1 MONITOR_INFO_1A
#define PMONITOR_INFO_1 PMONITOR_INFO_1A
#define LPMONITOR_INFO_1 LPMONITOR_INFO_1A
#endif // UNICODE

typedef struct _MONITOR_INFO_2A {
    LPSTR   pName;
    LPSTR   pEnvironment;
    LPSTR   pDLLName;
} MONITOR_INFO_2A, *PMONITOR_INFO_2A, *LPMONITOR_INFO_2A;

typedef struct _MONITOR_INFO_2W {
    LPWSTR  pName;
    LPWSTR  pEnvironment;
    LPWSTR  pDLLName;
} MONITOR_INFO_2W, *PMONITOR_INFO_2W, *LPMONITOR_INFO_2W;

#ifdef UNICODE
#define MONITOR_INFO_2 MONITOR_INFO_2W
#define PMONITOR_INFO_2 PMONITOR_INFO_2W
#define LPMONITOR_INFO_2 LPMONITOR_INFO_2W
#else
#define MONITOR_INFO_2 MONITOR_INFO_2A
#define PMONITOR_INFO_2 PMONITOR_INFO_2A
#define LPMONITOR_INFO_2 LPMONITOR_INFO_2A
#endif // UNICODE

typedef struct _DATATYPES_INFO_1A {
    LPSTR   pName;
} DATATYPES_INFO_1A, *PDATATYPES_INFO_1A, *LPDATATYPES_INFO_1A;

typedef struct _DATATYPES_INFO_1W {
    LPWSTR  pName;
} DATATYPES_INFO_1W, *PDATATYPES_INFO_1W, *LPDATATYPES_INFO_1W;

#ifdef UNICODE
#define DATATYPES_INFO_1 DATATYPES_INFO_1W
#define PDATATYPES_INFO_1 PDATATYPES_INFO_1W
#define LPDATATYPES_INFO_1 LPDATATYPES_INFO_1W
#else
#define DATATYPES_INFO_1 DATATYPES_INFO_1A
#define PDATATYPES_INFO_1 PDATATYPES_INFO_1A
#define LPDATATYPES_INFO_1 LPDATATYPES_INFO_1A
#endif // UNICODE

typedef struct _PRINTER_DEFAULTSA {
    LPSTR       pDatatype;
    LPDEVMODEA  pDevMode;
    ACCESS_MASK DesiredAccess;
} PRINTER_DEFAULTSA, *PPRINTER_DEFAULTSA, *LPPRINTER_DEFAULTSA;

typedef struct _PRINTER_DEFAULTSW {
    LPWSTR      pDatatype;
    LPDEVMODEW  pDevMode;
    ACCESS_MASK DesiredAccess;
} PRINTER_DEFAULTSW, *PPRINTER_DEFAULTSW, *LPPRINTER_DEFAULTSW;

#ifdef UNICODE
#define PRINTER_DEFAULTS PRINTER_DEFAULTSW
#define PPRINTER_DEFAULTS PPRINTER_DEFAULTSW
#define LPPRINTER_DEFAULTS LPPRINTER_DEFAULTSW
#else
#define PRINTER_DEFAULTS PRINTER_DEFAULTSA
#define PPRINTER_DEFAULTS PPRINTER_DEFAULTSA
#define LPPRINTER_DEFAULTS LPPRINTER_DEFAULTSA
#endif // UNICODE

BOOL
WINAPI
EnumPrintersA(
    DWORD   Flags,
    LPSTR   Name,
    DWORD   Level,
    LPBYTE  pPrinterEnum,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);

BOOL
WINAPI
EnumPrintersW(
    DWORD   Flags,
    LPWSTR  Name,
    DWORD   Level,
    LPBYTE  pPrinterEnum,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);

#ifdef UNICODE
#define EnumPrinters EnumPrintersW
#else
#define EnumPrinters EnumPrintersA
#endif // !UNICODE

#define PRINTER_ENUM_DEFAULT     0x00000001
#define PRINTER_ENUM_LOCAL       0x00000002
#define PRINTER_ENUM_CONNECTIONS 0x00000004
#define PRINTER_ENUM_FAVORITE    0x00000004
#define PRINTER_ENUM_NAME        0x00000008
#define PRINTER_ENUM_REMOTE      0x00000010
#define PRINTER_ENUM_SHARED      0x00000020
#define PRINTER_ENUM_NETWORK     0x00000040

#define PRINTER_ENUM_EXPAND      0x00004000
#define PRINTER_ENUM_CONTAINER   0x00008000

#define PRINTER_ENUM_ICONMASK    0x00ff0000
#define PRINTER_ENUM_ICON1       0x00010000
#define PRINTER_ENUM_ICON2       0x00020000
#define PRINTER_ENUM_ICON3       0x00040000
#define PRINTER_ENUM_ICON4       0x00080000
#define PRINTER_ENUM_ICON5       0x00100000
#define PRINTER_ENUM_ICON6       0x00200000
#define PRINTER_ENUM_ICON7       0x00400000
#define PRINTER_ENUM_ICON8       0x00800000

BOOL
WINAPI
OpenPrinterA(
   LPSTR    pPrinterName,
   LPHANDLE phPrinter,
   LPPRINTER_DEFAULTSA pDefault
);

BOOL
WINAPI
OpenPrinterW(
   LPWSTR   pPrinterName,
   LPHANDLE phPrinter,
   LPPRINTER_DEFAULTSW pDefault
);

#ifdef UNICODE
#define OpenPrinter OpenPrinterW
#else
#define OpenPrinter OpenPrinterA
#endif // !UNICODE

BOOL
WINAPI
ResetPrinterA(
   HANDLE   hPrinter,
   LPPRINTER_DEFAULTSA pDefault
);

BOOL
WINAPI
ResetPrinterW(
   HANDLE   hPrinter,
   LPPRINTER_DEFAULTSW pDefault
);

#ifdef UNICODE
#define ResetPrinter ResetPrinterW
#else
#define ResetPrinter ResetPrinterA
#endif // !UNICODE

BOOL
WINAPI
SetJobA(
    HANDLE  hPrinter,
    DWORD   JobId,
    DWORD   Level,
    LPBYTE  pJob,
    DWORD   Command
);

BOOL
WINAPI
SetJobW(
    HANDLE  hPrinter,
    DWORD   JobId,
    DWORD   Level,
    LPBYTE  pJob,
    DWORD   Command
);

#ifdef UNICODE
#define SetJob SetJobW
#else
#define SetJob SetJobA
#endif // !UNICODE

BOOL
WINAPI
GetJobA(
   HANDLE   hPrinter,
   DWORD    JobId,
   DWORD    Level,
   LPBYTE   pJob,
   DWORD    cbBuf,
   LPDWORD  pcbNeeded
);

BOOL
WINAPI
GetJobW(
   HANDLE   hPrinter,
   DWORD    JobId,
   DWORD    Level,
   LPBYTE   pJob,
   DWORD    cbBuf,
   LPDWORD  pcbNeeded
);

#ifdef UNICODE
#define GetJob GetJobW
#else
#define GetJob GetJobA
#endif // !UNICODE

BOOL
WINAPI
EnumJobsA(
    HANDLE  hPrinter,
    DWORD   FirstJob,
    DWORD   NoJobs,
    DWORD   Level,
    LPBYTE  pJob,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);

BOOL
WINAPI
EnumJobsW(
    HANDLE  hPrinter,
    DWORD   FirstJob,
    DWORD   NoJobs,
    DWORD   Level,
    LPBYTE  pJob,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);

#ifdef UNICODE
#define EnumJobs EnumJobsW
#else
#define EnumJobs EnumJobsA
#endif // !UNICODE

HANDLE
WINAPI
AddPrinterA(
    LPSTR   pName,
    DWORD   Level,
    LPBYTE  pPrinter
);

HANDLE
WINAPI
AddPrinterW(
    LPWSTR  pName,
    DWORD   Level,
    LPBYTE  pPrinter
);

#ifdef UNICODE
#define AddPrinter AddPrinterW
#else
#define AddPrinter AddPrinterA
#endif // !UNICODE

BOOL
WINAPI
DeletePrinter(
   HANDLE   hPrinter
);

BOOL
WINAPI
SetPrinterA(
    HANDLE  hPrinter,
    DWORD   Level,
    LPBYTE  pPrinter,
    DWORD   Command
);

BOOL
WINAPI
SetPrinterW(
    HANDLE  hPrinter,
    DWORD   Level,
    LPBYTE  pPrinter,
    DWORD   Command
);

#ifdef UNICODE
#define SetPrinter SetPrinterW
#else
#define SetPrinter SetPrinterA
#endif // !UNICODE

BOOL
WINAPI
GetPrinterA(
    HANDLE  hPrinter,
    DWORD   Level,
    LPBYTE  pPrinter,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
);

BOOL
WINAPI
GetPrinterW(
    HANDLE  hPrinter,
    DWORD   Level,
    LPBYTE  pPrinter,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
);

#ifdef UNICODE
#define GetPrinter GetPrinterW
#else
#define GetPrinter GetPrinterA
#endif // !UNICODE

BOOL
WINAPI
AddPrinterDriverA(
    LPSTR   pName,
    DWORD   Level,
    LPBYTE  pDriverInfo
);

BOOL
WINAPI
AddPrinterDriverW(
    LPWSTR  pName,
    DWORD   Level,
    LPBYTE  pDriverInfo
);

#ifdef UNICODE
#define AddPrinterDriver AddPrinterDriverW
#else
#define AddPrinterDriver AddPrinterDriverA
#endif // !UNICODE

BOOL
WINAPI
EnumPrinterDriversA(
    LPSTR   pName,
    LPSTR   pEnvironment,
    DWORD   Level,
    LPBYTE  pDriverInfo,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);

BOOL
WINAPI
EnumPrinterDriversW(
    LPWSTR  pName,
    LPWSTR  pEnvironment,
    DWORD   Level,
    LPBYTE  pDriverInfo,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);

#ifdef UNICODE
#define EnumPrinterDrivers EnumPrinterDriversW
#else
#define EnumPrinterDrivers EnumPrinterDriversA
#endif // !UNICODE

BOOL
WINAPI
GetPrinterDriverA(
    HANDLE  hPrinter,
    LPSTR   pEnvironment,
    DWORD   Level,
    LPBYTE  pDriverInfo,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
);

BOOL
WINAPI
GetPrinterDriverW(
    HANDLE  hPrinter,
    LPWSTR  pEnvironment,
    DWORD   Level,
    LPBYTE  pDriverInfo,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
);

#ifdef UNICODE
#define GetPrinterDriver GetPrinterDriverW
#else
#define GetPrinterDriver GetPrinterDriverA
#endif // !UNICODE

BOOL
WINAPI
GetPrinterDriverDirectoryA(
    LPSTR   pName,
    LPSTR   pEnvironment,
    DWORD   Level,
    LPBYTE  pDriverDirectory,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
);

BOOL
WINAPI
GetPrinterDriverDirectoryW(
    LPWSTR  pName,
    LPWSTR  pEnvironment,
    DWORD   Level,
    LPBYTE  pDriverDirectory,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
);

#ifdef UNICODE
#define GetPrinterDriverDirectory GetPrinterDriverDirectoryW
#else
#define GetPrinterDriverDirectory GetPrinterDriverDirectoryA
#endif // !UNICODE

BOOL
WINAPI
DeletePrinterDriverA(
   LPSTR    pName,
   LPSTR    pEnvironment,
   LPSTR    pDriverName
);

BOOL
WINAPI
DeletePrinterDriverW(
   LPWSTR   pName,
   LPWSTR   pEnvironment,
   LPWSTR   pDriverName
);

#ifdef UNICODE
#define DeletePrinterDriver DeletePrinterDriverW
#else
#define DeletePrinterDriver DeletePrinterDriverA
#endif // !UNICODE

BOOL
WINAPI
AddPrintProcessorA(
    LPSTR   pName,
    LPSTR   pEnvironment,
    LPSTR   pPathName,
    LPSTR   pPrintProcessorName
);

BOOL
WINAPI
AddPrintProcessorW(
    LPWSTR  pName,
    LPWSTR  pEnvironment,
    LPWSTR  pPathName,
    LPWSTR  pPrintProcessorName
);

#ifdef UNICODE
#define AddPrintProcessor AddPrintProcessorW
#else
#define AddPrintProcessor AddPrintProcessorA
#endif // !UNICODE

BOOL
WINAPI
EnumPrintProcessorsA(
    LPSTR   pName,
    LPSTR   pEnvironment,
    DWORD   Level,
    LPBYTE  pPrintProcessorInfo,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);

BOOL
WINAPI
EnumPrintProcessorsW(
    LPWSTR  pName,
    LPWSTR  pEnvironment,
    DWORD   Level,
    LPBYTE  pPrintProcessorInfo,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);

#ifdef UNICODE
#define EnumPrintProcessors EnumPrintProcessorsW
#else
#define EnumPrintProcessors EnumPrintProcessorsA
#endif // !UNICODE

BOOL
WINAPI
GetPrintProcessorDirectoryA(
    LPSTR   pName,
    LPSTR   pEnvironment,
    DWORD   Level,
    LPBYTE  pPrintProcessorInfo,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
);

BOOL
WINAPI
GetPrintProcessorDirectoryW(
    LPWSTR  pName,
    LPWSTR  pEnvironment,
    DWORD   Level,
    LPBYTE  pPrintProcessorInfo,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
);

#ifdef UNICODE
#define GetPrintProcessorDirectory GetPrintProcessorDirectoryW
#else
#define GetPrintProcessorDirectory GetPrintProcessorDirectoryA
#endif // !UNICODE

BOOL
EnumPrintProcessorDatatypesW(
    LPWSTR  pName,
    LPWSTR  pPrintProcessorName,
    DWORD   Level,
    LPBYTE  pDatatypes,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);

BOOL
EnumPrintProcessorDatatypesA(
    LPSTR   pName,
    LPSTR   pPrintProcessorName,
    DWORD   Level,
    LPBYTE  pDatatypes,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);

#ifdef UNICODE
#define EnumPrintProcessorDatatypes EnumPrintProcessorDatatypesW
#else
#define EnumPrintProcessorDatatypes EnumPrintProcessorDatatypesA
#endif // !UNICODE

BOOL
WINAPI
DeletePrintProcessorA(
    LPSTR   pName,
    LPSTR   pEnvironment,
    LPSTR   pPrintProcessorName
);

BOOL
WINAPI
DeletePrintProcessorW(
    LPWSTR  pName,
    LPWSTR  pEnvironment,
    LPWSTR  pPrintProcessorName
);

#ifdef UNICODE
#define DeletePrintProcessor DeletePrintProcessorW
#else
#define DeletePrintProcessor DeletePrintProcessorA
#endif // !UNICODE

DWORD
WINAPI
StartDocPrinterA(
    HANDLE  hPrinter,
    DWORD   Level,
    LPBYTE  pDocInfo
);

DWORD
WINAPI
StartDocPrinterW(
    HANDLE  hPrinter,
    DWORD   Level,
    LPBYTE  pDocInfo
);

#ifdef UNICODE
#define StartDocPrinter StartDocPrinterW
#else
#define StartDocPrinter StartDocPrinterA
#endif // !UNICODE

BOOL
WINAPI
StartPagePrinter(
    HANDLE  hPrinter
);

BOOL
WINAPI
WritePrinter(
    HANDLE  hPrinter,
    LPVOID  pBuf,
    DWORD   cbBuf,
    LPDWORD pcWritten
);

BOOL
WINAPI
EndPagePrinter(
   HANDLE   hPrinter
);

BOOL
WINAPI
AbortPrinter(
   HANDLE   hPrinter
);

BOOL
WINAPI
ReadPrinter(
    HANDLE  hPrinter,
    LPVOID  pBuf,
    DWORD   cbBuf,
    LPDWORD pNoBytesRead
);

BOOL
WINAPI
EndDocPrinter(
   HANDLE   hPrinter
);

BOOL
WINAPI
AddJobA(
    HANDLE  hPrinter,
    DWORD   Level,
    LPBYTE  pData,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
);

BOOL
WINAPI
AddJobW(
    HANDLE  hPrinter,
    DWORD   Level,
    LPBYTE  pData,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
);

#ifdef UNICODE
#define AddJob AddJobW
#else
#define AddJob AddJobA
#endif // !UNICODE

BOOL
WINAPI
ScheduleJob(
    HANDLE  hPrinter,
    DWORD   JobId
);

BOOL
WINAPI
PrinterProperties(
    HWND    hWnd,
    HANDLE  hPrinter
);

LONG
WINAPI
DocumentPropertiesA(
    HWND    hWnd,
    HANDLE  hPrinter,
    LPSTR   pDeviceName,
    PDEVMODEA pDevModeOutput,
    PDEVMODEA pDevModeInput,
    DWORD   fMode
);

LONG
WINAPI
DocumentPropertiesW(
    HWND    hWnd,
    HANDLE  hPrinter,
    LPWSTR  pDeviceName,
    PDEVMODEW pDevModeOutput,
    PDEVMODEW pDevModeInput,
    DWORD   fMode
);

#ifdef UNICODE
#define DocumentProperties DocumentPropertiesW
#else
#define DocumentProperties DocumentPropertiesA
#endif // !UNICODE

LONG
WINAPI
AdvancedDocumentPropertiesA(
    HWND    hWnd,
    HANDLE  hPrinter,
    LPSTR   pDeviceName,
    PDEVMODEA pDevModeOutput,
    PDEVMODEA pDevModeInput
);

LONG
WINAPI
AdvancedDocumentPropertiesW(
    HWND    hWnd,
    HANDLE  hPrinter,
    LPWSTR  pDeviceName,
    PDEVMODEW pDevModeOutput,
    PDEVMODEW pDevModeInput
);

#ifdef UNICODE
#define AdvancedDocumentProperties AdvancedDocumentPropertiesW
#else
#define AdvancedDocumentProperties AdvancedDocumentPropertiesA
#endif // !UNICODE

DWORD
WINAPI
GetPrinterDataA(
    HANDLE   hPrinter,
    LPSTR    pValueName,
    LPDWORD  pType,
    LPBYTE   pData,
    DWORD    nSize,
    LPDWORD  pcbNeeded
);

DWORD
WINAPI
GetPrinterDataW(
    HANDLE   hPrinter,
    LPWSTR   pValueName,
    LPDWORD  pType,
    LPBYTE   pData,
    DWORD    nSize,
    LPDWORD  pcbNeeded
);

#ifdef UNICODE
#define GetPrinterData GetPrinterDataW
#else
#define GetPrinterData GetPrinterDataA
#endif // !UNICODE

DWORD
WINAPI
SetPrinterDataA(
    HANDLE  hPrinter,
    LPSTR   pValueName,
    DWORD   Type,
    LPBYTE  pData,
    DWORD   cbData
);

DWORD
WINAPI
SetPrinterDataW(
    HANDLE  hPrinter,
    LPWSTR  pValueName,
    DWORD   Type,
    LPBYTE  pData,
    DWORD   cbData
);

#ifdef UNICODE
#define SetPrinterData SetPrinterDataW
#else
#define SetPrinterData SetPrinterDataA
#endif // !UNICODE

DWORD
WINAPI
WaitForPrinterChange(
    HANDLE  hPrinter,
    DWORD   Flags
);

#define PRINTER_CHANGE_ADD_PRINTER              0x00000001
#define PRINTER_CHANGE_SET_PRINTER              0x00000002
#define PRINTER_CHANGE_DELETE_PRINTER           0x00000004
#define PRINTER_CHANGE_PRINTER                  0x000000FF
#define PRINTER_CHANGE_ADD_JOB                  0x00000100
#define PRINTER_CHANGE_SET_JOB                  0x00000200
#define PRINTER_CHANGE_DELETE_JOB               0x00000400
#define PRINTER_CHANGE_WRITE_JOB                0x00000800
#define PRINTER_CHANGE_JOB                      0x0000FF00
#define PRINTER_CHANGE_ADD_FORM                 0x00010000
#define PRINTER_CHANGE_SET_FORM                 0x00020000
#define PRINTER_CHANGE_DELETE_FORM              0x00040000
#define PRINTER_CHANGE_FORM                     0x00070000
#define PRINTER_CHANGE_ADD_PORT                 0x00100000
#define PRINTER_CHANGE_CONFIGURE_PORT           0x00200000
#define PRINTER_CHANGE_DELETE_PORT              0x00400000
#define PRINTER_CHANGE_PORT                     0x00700000
#define PRINTER_CHANGE_ADD_PRINT_PROCESSOR      0x01000000
#define PRINTER_CHANGE_DELETE_PRINT_PROCESSOR   0x04000000
#define PRINTER_CHANGE_PRINT_PROCESSOR          0x07000000
#define PRINTER_CHANGE_ADD_PRINTER_DRIVER       0x10000000
#define PRINTER_CHANGE_DELETE_PRINTER_DRIVER    0x40000000
#define PRINTER_CHANGE_PRINTER_DRIVER           0x70000000
#define PRINTER_CHANGE_TIMEOUT                  0x80000000
#define PRINTER_CHANGE_ALL                      0x7777FFFF

DWORD
WINAPI
PrinterMessageBoxA(
    HANDLE  hPrinter,
    DWORD   Error,
    HWND    hWnd,
    LPSTR   pText,
    LPSTR   pCaption,
    DWORD   dwType
);

DWORD
WINAPI
PrinterMessageBoxW(
    HANDLE  hPrinter,
    DWORD   Error,
    HWND    hWnd,
    LPWSTR  pText,
    LPWSTR  pCaption,
    DWORD   dwType
);

#ifdef UNICODE
#define PrinterMessageBox PrinterMessageBoxW
#else
#define PrinterMessageBox PrinterMessageBoxA
#endif // !UNICODE

#define PRINTER_ERROR_INFORMATION   0x80000000
#define PRINTER_ERROR_WARNING       0x40000000
#define PRINTER_ERROR_SEVERE        0x20000000

#define PRINTER_ERROR_OUTOFPAPER    0x00000001
#define PRINTER_ERROR_JAM           0x00000002
#define PRINTER_ERROR_OUTOFTONER    0x00000004

BOOL
WINAPI
ClosePrinter(
    HANDLE hPrinter
);

BOOL
WINAPI
AddFormA(
    HANDLE  hPrinter,
    DWORD   Level,
    LPBYTE  pForm
);

BOOL
WINAPI
AddFormW(
    HANDLE  hPrinter,
    DWORD   Level,
    LPBYTE  pForm
);

#ifdef UNICODE
#define AddForm AddFormW
#else
#define AddForm AddFormA
#endif // !UNICODE

BOOL
WINAPI
DeleteFormA(
    HANDLE  hPrinter,
    LPSTR   pFormName
);

BOOL
WINAPI
DeleteFormW(
    HANDLE  hPrinter,
    LPWSTR  pFormName
);

#ifdef UNICODE
#define DeleteForm DeleteFormW
#else
#define DeleteForm DeleteFormA
#endif // !UNICODE

BOOL
WINAPI
GetFormA(
    HANDLE  hPrinter,
    LPSTR   pFormName,
    DWORD   Level,
    LPBYTE  pForm,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
);

BOOL
WINAPI
GetFormW(
    HANDLE  hPrinter,
    LPWSTR  pFormName,
    DWORD   Level,
    LPBYTE  pForm,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
);

#ifdef UNICODE
#define GetForm GetFormW
#else
#define GetForm GetFormA
#endif // !UNICODE

BOOL
WINAPI
SetFormA(
    HANDLE  hPrinter,
    LPSTR   pFormName,
    DWORD   Level,
    LPBYTE  pForm
);

BOOL
WINAPI
SetFormW(
    HANDLE  hPrinter,
    LPWSTR  pFormName,
    DWORD   Level,
    LPBYTE  pForm
);

#ifdef UNICODE
#define SetForm SetFormW
#else
#define SetForm SetFormA
#endif // !UNICODE

BOOL
WINAPI
EnumFormsA(
    HANDLE  hPrinter,
    DWORD   Level,
    LPBYTE  pForm,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);

BOOL
WINAPI
EnumFormsW(
    HANDLE  hPrinter,
    DWORD   Level,
    LPBYTE  pForm,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);

#ifdef UNICODE
#define EnumForms EnumFormsW
#else
#define EnumForms EnumFormsA
#endif // !UNICODE

BOOL
WINAPI
EnumMonitorsA(
    LPSTR   pName,
    DWORD   Level,
    LPBYTE  pMonitors,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);

BOOL
WINAPI
EnumMonitorsW(
    LPWSTR  pName,
    DWORD   Level,
    LPBYTE  pMonitors,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);

#ifdef UNICODE
#define EnumMonitors EnumMonitorsW
#else
#define EnumMonitors EnumMonitorsA
#endif // !UNICODE

BOOL
WINAPI
AddMonitorA(
    LPSTR   pName,
    DWORD   Level,
    LPBYTE  pMonitors
);

BOOL
WINAPI
AddMonitorW(
    LPWSTR  pName,
    DWORD   Level,
    LPBYTE  pMonitors
);

#ifdef UNICODE
#define AddMonitor AddMonitorW
#else
#define AddMonitor AddMonitorA
#endif // !UNICODE

BOOL
WINAPI
DeleteMonitorA(
    LPSTR   pName,
    LPSTR   pEnvironment,
    LPSTR   pMonitorName
);

BOOL
WINAPI
DeleteMonitorW(
    LPWSTR  pName,
    LPWSTR  pEnvironment,
    LPWSTR  pMonitorName
);

#ifdef UNICODE
#define DeleteMonitor DeleteMonitorW
#else
#define DeleteMonitor DeleteMonitorA
#endif // !UNICODE

BOOL
WINAPI
EnumPortsA(
    LPSTR   pName,
    DWORD   Level,
    LPBYTE  pPorts,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);

BOOL
WINAPI
EnumPortsW(
    LPWSTR  pName,
    DWORD   Level,
    LPBYTE  pPorts,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);

#ifdef UNICODE
#define EnumPorts EnumPortsW
#else
#define EnumPorts EnumPortsA
#endif // !UNICODE

BOOL
WINAPI
AddPortA(
    LPSTR   pName,
    HWND    hWnd,
    LPSTR   pMonitorName
);

BOOL
WINAPI
AddPortW(
    LPWSTR  pName,
    HWND    hWnd,
    LPWSTR  pMonitorName
);

#ifdef UNICODE
#define AddPort AddPortW
#else
#define AddPort AddPortA
#endif // !UNICODE

BOOL
WINAPI
ConfigurePortA(
    LPSTR   pName,
    HWND    hWnd,
    LPSTR   pPortName
);

BOOL
WINAPI
ConfigurePortW(
    LPWSTR  pName,
    HWND    hWnd,
    LPWSTR  pPortName
);

#ifdef UNICODE
#define ConfigurePort ConfigurePortW
#else
#define ConfigurePort ConfigurePortA
#endif // !UNICODE

BOOL
WINAPI
DeletePortA(
    LPSTR   pName,
    HWND    hWnd,
    LPSTR   pPortName
);

BOOL
WINAPI
DeletePortW(
    LPWSTR  pName,
    HWND    hWnd,
    LPWSTR  pPortName
);

#ifdef UNICODE
#define DeletePort DeletePortW
#else
#define DeletePort DeletePortA
#endif // !UNICODE

BOOL
WINAPI
AddPrinterConnectionA(
    LPSTR   pName
);

BOOL
WINAPI
AddPrinterConnectionW(
    LPWSTR  pName
);

#ifdef UNICODE
#define AddPrinterConnection AddPrinterConnectionW
#else
#define AddPrinterConnection AddPrinterConnectionA
#endif // !UNICODE

BOOL
WINAPI
DeletePrinterConnectionA(
    LPSTR   pName
);

BOOL
WINAPI
DeletePrinterConnectionW(
    LPWSTR  pName
);

#ifdef UNICODE
#define DeletePrinterConnection DeletePrinterConnectionW
#else
#define DeletePrinterConnection DeletePrinterConnectionA
#endif // !UNICODE

HANDLE
WINAPI
ConnectToPrinterDlg(
    HWND    hwnd,
    DWORD   Flags
);

typedef struct _PROVIDOR_INFO_1A {
    LPSTR   pName;
    LPSTR   pEnvironment;
    LPSTR   pDLLName;
} PROVIDOR_INFO_1A, *PPROVIDOR_INFO_1A, *LPPROVIDOR_INFO_1A;

typedef struct _PROVIDOR_INFO_1W {
    LPWSTR  pName;
    LPWSTR  pEnvironment;
    LPWSTR  pDLLName;
} PROVIDOR_INFO_1W, *PPROVIDOR_INFO_1W, *LPPROVIDOR_INFO_1W;

#ifdef UNICODE
#define PROVIDOR_INFO_1 PROVIDOR_INFO_1W
#define PPROVIDOR_INFO_1 PPROVIDOR_INFO_1W
#define LPPROVIDOR_INFO_1 LPPROVIDOR_INFO_1W
#else
#define PROVIDOR_INFO_1 PROVIDOR_INFO_1A
#define PPROVIDOR_INFO_1 PPROVIDOR_INFO_1A
#define LPPROVIDOR_INFO_1 LPPROVIDOR_INFO_1A
#endif // UNICODE

BOOL
WINAPI
AddPrintProvidorA(
    LPSTR   pName,
    DWORD   Level,
    LPBYTE  pProvidorInfo
);

BOOL
WINAPI
AddPrintProvidorW(
    LPWSTR  pName,
    DWORD   Level,
    LPBYTE  pProvidorInfo
);

#ifdef UNICODE
#define AddPrintProvidor AddPrintProvidorW
#else
#define AddPrintProvidor AddPrintProvidorA
#endif // !UNICODE

BOOL
WINAPI
DeletePrintProvidorA(
    LPSTR   pName,
    LPSTR   pEnvironment,
    LPSTR   pPrintProvidorName
);

BOOL
WINAPI
DeletePrintProvidorW(
    LPWSTR  pName,
    LPWSTR  pEnvironment,
    LPWSTR  pPrintProvidorName
);

#ifdef UNICODE
#define DeletePrintProvidor DeletePrintProvidorW
#else
#define DeletePrintProvidor DeletePrintProvidorA
#endif // !UNICODE

#define SERVER_ACCESS_ADMINISTER    0x00000001
#define SERVER_ACCESS_ENUMERATE     0x00000002

#define PRINTER_ACCESS_ADMINISTER   0x00000004
#define PRINTER_ACCESS_USE          0x00000008

#define JOB_ACCESS_ADMINISTER       0x00000010


/*
 * Access rights for print servers
 */

#define SERVER_ALL_ACCESS    (STANDARD_RIGHTS_REQUIRED      |\
                              SERVER_ACCESS_ADMINISTER      |\
                              SERVER_ACCESS_ENUMERATE)

#define SERVER_READ          (STANDARD_RIGHTS_READ          |\
                              SERVER_ACCESS_ENUMERATE)

#define SERVER_WRITE         (STANDARD_RIGHTS_WRITE         |\
                              SERVER_ACCESS_ADMINISTER      |\
                              SERVER_ACCESS_ENUMERATE)

#define SERVER_EXECUTE       (STANDARD_RIGHTS_EXECUTE       |\
                              SERVER_ACCESS_ENUMERATE)

/*
 * Access rights for printers
 */

#define PRINTER_ALL_ACCESS    (STANDARD_RIGHTS_REQUIRED     |\
                               PRINTER_ACCESS_ADMINISTER    |\
                               PRINTER_ACCESS_USE)

#define PRINTER_READ          (STANDARD_RIGHTS_READ         |\
                               PRINTER_ACCESS_USE)

#define PRINTER_WRITE         (STANDARD_RIGHTS_WRITE        |\
                               PRINTER_ACCESS_USE)

#define PRINTER_EXECUTE       (STANDARD_RIGHTS_EXECUTE      |\
                               PRINTER_ACCESS_USE)

/*
 * Access rights for jobs
 */

#define JOB_ALL_ACCESS         (STANDARD_RIGHTS_REQUIRED    |\
                                JOB_ACCESS_ADMINISTER)

#define JOB_READ               (STANDARD_RIGHTS_READ        |\
                                JOB_ACCESS_ADMINISTER)

#define JOB_WRITE              (STANDARD_RIGHTS_WRITE       |\
                                JOB_ACCESS_ADMINISTER)

#define JOB_EXECUTE            (STANDARD_RIGHTS_EXECUTE     |\
                                JOB_ACCESS_ADMINISTER)


#ifdef __cplusplus
}
#endif

#endif // _WINSPOOL_

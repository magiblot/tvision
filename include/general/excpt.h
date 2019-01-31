/***
*excpt.h - defines exception values, types and routines
*
*Purpose:
*   This file contains the definitions and prototypes for the compiler-
*   dependent intrinsics, support functions and keywords which implement
*   the structured exception handling extensions.
*
****/

/*
 *      C/C++ Run Time Library - Version 6.0
 *
 *      Copyright (c) 1990, 1993 by Borland International
 *      All Rights Reserved.
 *
 */



#ifndef __EXCPT_H
#define __EXCPT_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__BORLANDC__) && !defined(__FLAT__)

#if !defined(__WINDOWS_H)
typedef unsigned char       BYTE;
typedef unsigned long       DWORD;
typedef void __far *        LPVOID;
typedef unsigned int        UINT;
typedef DWORD  __far *      LPDWORD;
#define WINAPI              _far _pascal
#endif

/* From WINNT.H */

//
//  Define the size of the 80387 save area, which is in the context frame.
//

#define SIZE_OF_80387_REGISTERS      80

typedef struct _FLOATING_SAVE_AREA {
    DWORD   ControlWord;
    DWORD   StatusWord;
    DWORD   TagWord;
    DWORD   ErrorOffset;
    DWORD   ErrorSelector;
    DWORD   DataOffset;
    DWORD   DataSelector;
    BYTE    RegisterArea[SIZE_OF_80387_REGISTERS];
    DWORD   Cr0NpxState;
} FLOATING_SAVE_AREA;

//
// Context Frame
//
//  This frame has a several purposes: 1) it is used as an argument to
//  NtContinue, 2) is is used to constuct a call frame for APC delivery,
//  and 3) it is used in the user level thread creation routines.
//
//  The layout of the record conforms to a standard call frame.
//

typedef struct _CONTEXT {

    //
    // The flags values within this flag control the contents of
    // a CONTEXT record.
    //
    // If the context record is used as an input parameter, then
    // for each portion of the context record controlled by a flag
    // whose value is set, it is assumed that that portion of the
    // context record contains valid context. If the context record
    // is being used to modify a threads context, then only that
    // portion of the threads context will be modified.
    //
    // If the context record is used as an IN OUT parameter to capture
    // the context of a thread, then only those portions of the thread's
    // context corresponding to set flags will be returned.
    //
    // The context record is never used as an OUT only parameter.
    //

    DWORD ContextFlags;

    //
    // This section is specified/returned if CONTEXT_DEBUG_REGISTERS is
    // set in ContextFlags.  Note that CONTEXT_DEBUG_REGISTERS is NOT
    // included in CONTEXT_FULL.
    //

    DWORD   Dr0;
    DWORD   Dr1;
    DWORD   Dr2;
    DWORD   Dr3;
    DWORD   Dr6;
    DWORD   Dr7;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_FLOATING_POINT.
    //

    FLOATING_SAVE_AREA FloatSave;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_SEGMENTS.
    //

    DWORD   SegGs;
    DWORD   SegFs;
    DWORD   SegEs;
    DWORD   SegDs;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_INTEGER.
    //

    DWORD   Edi;
    DWORD   Esi;
    DWORD   Ebx;
    DWORD   Edx;
    DWORD   Ecx;
    DWORD   Eax;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_CONTROL.
    //

    DWORD   Ebp;
    DWORD   Eip;
    DWORD   SegCs;              // MUST BE SANITIZED
    DWORD   EFlags;             // MUST BE SANITIZED
    DWORD   Esp;
    DWORD   SegSs;

} CONTEXT;


typedef CONTEXT *PCONTEXT;

/* From WINNT.H */
#define STATUS_WAIT_0                    ((DWORD   )0x00000000L)
#define STATUS_ABANDONED_WAIT_0          ((DWORD   )0x00000080L)
#define STATUS_USER_APC                  ((DWORD   )0x000000C0L)
#define STATUS_TIMEOUT                   ((DWORD   )0x00000102L)
#define STATUS_PENDING                   ((DWORD   )0x00000103L)
#define STATUS_DATATYPE_MISALIGNMENT     ((DWORD   )0x80000002L)
#define STATUS_BREAKPOINT                ((DWORD   )0x80000003L)
#define STATUS_SINGLE_STEP               ((DWORD   )0x80000004L)
#define STATUS_ACCESS_VIOLATION          ((DWORD   )0xC0000005L)
#define STATUS_IN_PAGE_ERROR             ((DWORD   )0xC0000006L)
#define STATUS_NO_MEMORY                 ((DWORD   )0xC0000017L)
#define STATUS_ILLEGAL_INSTRUCTION       ((DWORD   )0xC000001DL)
#define STATUS_NONCONTINUABLE_EXCEPTION  ((DWORD   )0xC0000025L)
#define STATUS_INVALID_DISPOSITION       ((DWORD   )0xC0000026L)
#define STATUS_ARRAY_BOUNDS_EXCEEDED     ((DWORD   )0xC000008CL)
#define STATUS_FLOAT_DENORMAL_OPERAND    ((DWORD   )0xC000008DL)
#define STATUS_FLOAT_DIVIDE_BY_ZERO      ((DWORD   )0xC000008EL)
#define STATUS_FLOAT_INEXACT_RESULT      ((DWORD   )0xC000008FL)
#define STATUS_FLOAT_INVALID_OPERATION   ((DWORD   )0xC0000090L)
#define STATUS_FLOAT_OVERFLOW            ((DWORD   )0xC0000091L)
#define STATUS_FLOAT_STACK_CHECK         ((DWORD   )0xC0000092L)
#define STATUS_FLOAT_UNDERFLOW           ((DWORD   )0xC0000093L)
#define STATUS_INTEGER_DIVIDE_BY_ZERO    ((DWORD   )0xC0000094L)
#define STATUS_INTEGER_OVERFLOW          ((DWORD   )0xC0000095L)
#define STATUS_PRIVILEGED_INSTRUCTION    ((DWORD   )0xC0000096L)
#define STATUS_STACK_OVERFLOW            ((DWORD   )0xC00000FDL)
#define STATUS_CONTROL_C_EXIT            ((DWORD   )0xC000013AL)

/* From WINNT.H */
#define EXCEPTION_CONTINUABLE        0      // Continuable exception
#define EXCEPTION_NONCONTINUABLE     0x1    // Noncontinuable exception
#define EXCEPTION_MAXIMUM_PARAMETERS 15     // maximum number of exception parameters

//
// Exception record definition.
//

typedef struct _EXCEPTION_RECORD {
    DWORD ExceptionCode;
    DWORD ExceptionFlags;
    struct _EXCEPTION_RECORD __ss *ExceptionRecord;
    LPVOID ExceptionAddress;
    UINT NumberParameters;
    DWORD ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
} EXCEPTION_RECORD;

typedef EXCEPTION_RECORD __ss *PEXCEPTION_RECORD;

//
// Typedef for pointer returned by exception_info()
//

typedef struct _EXCEPTION_POINTERS {
    PEXCEPTION_RECORD ExceptionRecord;
    PCONTEXT ContextRecord;
} EXCEPTION_POINTERS, __ss *PEXCEPTION_POINTERS;


/* From WINBASE.H */
#define EXCEPTION_ACCESS_VIOLATION      STATUS_ACCESS_VIOLATION
#define EXCEPTION_DATATYPE_MISALIGNMENT STATUS_DATATYPE_MISALIGNMENT
#define EXCEPTION_BREAKPOINT            STATUS_BREAKPOINT
#define EXCEPTION_SINGLE_STEP           STATUS_SINGLE_STEP
#define EXCEPTION_ARRAY_BOUNDS_EXCEEDED STATUS_ARRAY_BOUNDS_EXCEEDED
#define EXCEPTION_FLT_DENORMAL_OPERAND  STATUS_FLOAT_DENORMAL_OPERAND
#define EXCEPTION_FLT_DIVIDE_BY_ZERO    STATUS_FLOAT_DIVIDE_BY_ZERO
#define EXCEPTION_FLT_INEXACT_RESULT    STATUS_FLOAT_INEXACT_RESULT
#define EXCEPTION_FLT_INVALID_OPERATION STATUS_FLOAT_INVALID_OPERATION
#define EXCEPTION_FLT_OVERFLOW          STATUS_FLOAT_OVERFLOW
#define EXCEPTION_FLT_STACK_CHECK       STATUS_FLOAT_STACK_CHECK
#define EXCEPTION_FLT_UNDERFLOW         STATUS_FLOAT_UNDERFLOW
#define EXCEPTION_INT_DIVIDE_BY_ZERO    STATUS_INTEGER_DIVIDE_BY_ZERO
#define EXCEPTION_INT_OVERFLOW          STATUS_INTEGER_OVERFLOW
#define EXCEPTION_PRIV_INSTRUCTION      STATUS_PRIVILEGED_INSTRUCTION
#define EXCEPTION_IN_PAGE_ERROR         STATUS_IN_PAGE_ERROR

void
__cdecl __far
RaiseException(
    DWORD dwExceptionCode,
    DWORD dwExceptionFlags,
    DWORD nNumberOfArguments,
    const LPDWORD lpArguments
    );

long
WINAPI
UnhandledExceptionFilter(PEXCEPTION_POINTERS ExceptionInfo);

typedef long (WINAPI *PTOP_LEVEL_EXCEPTION_FILTER)(
   PEXCEPTION_POINTERS ExceptionInfo
   );
typedef PTOP_LEVEL_EXCEPTION_FILTER LPTOP_LEVEL_EXCEPTION_FILTER;

LPTOP_LEVEL_EXCEPTION_FILTER
WINAPI
SetUnhandledExceptionFilter(
   LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter
   );

#endif  /* __BORLANDC__ && !__FLAT__ */


/*
 * Conditional macro definition for function calling type and variable type
 * qualifiers.
 */
#if   ( (_MSC_VER >= 800) && (_M_IX86 >= 300) ) || defined(__BORLANDC__)

/*
 * Definitions for MS C8-32 (386/486) compiler
 */
#define _CRTAPI1 __cdecl
#define _CRTAPI2 __cdecl

#else

/*
 * Other compilers (e.g., MIPS)
 */
#define _CRTAPI1
#define _CRTAPI2

#endif


/*
 * Exception disposition return values.
 */
typedef enum _EXCEPTION_DISPOSITION {
    ExceptionContinueExecution,
    ExceptionContinueSearch,
    ExceptionNestedException,
    ExceptionCollidedUnwind
} EXCEPTION_DISPOSITION;


/*
 * Prototype for SEH support function.
 */

#if defined(_M_IX86)

/*
 * Declarations to keep MS C 8 (386/486) compiler happy
 */
struct _EXCEPTION_RECORD;
struct _CONTEXT;

EXCEPTION_DISPOSITION _CRTAPI2 _except_handler (
        struct _EXCEPTION_RECORD *ExceptionRecord,
        void *EstablisherFrame,
        struct _CONTEXT *ContextRecord,
        void *DispatcherContext
        );

#elif defined(_M_MRX000) || defined(_MIPS_) || defined(_ALPHA_)

/*
 * Declarations to keep MIPS and ALPHA compiler happy
 */
typedef struct _EXCEPTION_POINTERS *Exception_info_ptr;
struct _EXCEPTION_RECORD;
struct _CONTEXT;
struct _DISPATCHER_CONTEXT;


EXCEPTION_DISPOSITION __C_specific_handler (
        struct _EXCEPTION_RECORD *ExceptionRecord,
        void *EstablisherFrame,
        struct _CONTEXT *ContextRecord,
        struct _DISPATCHER_CONTEXT *DispatcherContext
        );

#endif


/*
 * Keywords and intrinsics for SEH
 */

#if     ( _MSC_VER >= 800 )
/*
 * MS C8-32 (386/486)
 */
#define try                             __try
#define except                          __except
#define finally                         __finally
#define leave                           __leave
#define GetExceptionCode                _exception_code
#define exception_code                  _exception_code
#define GetExceptionInformation         (struct _EXCEPTION_POINTERS *)_exception_info
#define exception_info                  (struct _EXCEPTION_POINTERS *)_exception_info
#define AbnormalTermination             _abnormal_termination
#define abnormal_termination            _abnormal_termination

unsigned long _CRTAPI1 _exception_code(void);
void *        _CRTAPI1 _exception_info(void);
int           _CRTAPI1 _abnormal_termination(void);

#elif   ( __BORLANDC__ )
/*
 * Borland C++
 */
#ifndef __cplusplus
#  define try                       __try
#  define finally                   __finally
#  define AbnormalTermination()     __abnormal_termination
#  define abnormal_termination()    __abnormal_termination
#endif

#  define except                    __except
#  define GetExceptionCode()        __exception_code
#  define exception_code()          __exception_code
#  define GetExceptionInformation() ((PEXCEPTION_POINTERS)__exception_info)
#  define exception_info()          ((PEXCEPTION_POINTERS)__exception_info)


#elif defined(_M_MRX000) || defined(_MIPS_) || defined(_ALPHA_)
/*
 * MIPS or ALPHA compiler
 */
#define try                             __builtin_try
#define except                          __builtin_except
#define finally                         __builtin_finally
#define leave                           __builtin_leave
#define GetExceptionCode()              __exception_code
#define exception_code()                __exception_code
#define GetExceptionInformation()       (struct _EXCEPTION_POINTERS *)__exception_info
#define exception_info()                (struct _EXCEPTION_POINTERS *)__exception_info
#define AbnormalTermination()           __abnormal_termination
#define abnormal_termination()          __abnormal_termination

extern unsigned long __exception_code;
extern int           __exception_info;
extern int           __abnormal_termination;

#endif


/*
 * Legal values for expression in except().
 */

#define EXCEPTION_EXECUTE_HANDLER        1
#define EXCEPTION_CONTINUE_SEARCH        0
#define EXCEPTION_CONTINUE_EXECUTION    -1

#ifdef __cplusplus
}
#endif

#endif  /* __EXCPT_H */

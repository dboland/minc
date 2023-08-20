/*
 * https://www.nirsoft.net/kernel_struct/vista/NT_TIB.html
 */

typedef enum _EXCEPTION_DISPOSITION
{
         ExceptionContinueExecution = 0,
         ExceptionContinueSearch = 1,
         ExceptionNestedException = 2,
         ExceptionCollidedUnwind = 3
} EXCEPTION_DISPOSITION, *PEXCEPTION_DISPOSITION;

typedef struct _EXCEPTION_REGISTRATION_RECORD
{
     struct _EXCEPTION_REGISTRATION_RECORD *Next;
     PEXCEPTION_DISPOSITION Handler;
} EXCEPTION_REGISTRATION_RECORD, *PEXCEPTION_REGISTRATION_RECORD;

typedef struct _NT_TIB
{
     PEXCEPTION_REGISTRATION_RECORD ExceptionList;
     PVOID StackBase;
     PVOID StackLimit;
     PVOID SubSystemTib;
     union
     {
          PVOID FiberData;
          ULONG Version;
     };
     PVOID ArbitraryUserPointer;
     struct _NT_TIB *Self;
} NT_TIB, *PNT_TIB;

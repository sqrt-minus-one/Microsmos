/* date = July 13th 2022 0:48 pm */

#ifndef OS_SEMAPHORE_H
#define OS_SEMAPHORE_H


internal void OS_CreateSemaphore(semaphore_handle *Semaphore, u32 InitialValue, u32 MaxValue);
internal void OS_AcquireSemaphore(semaphore_handle *Semaphore);
internal void OS_ReleaseSemaphore(semaphore_handle *Semaphore);


#endif //OS_SEMAPHORE_H

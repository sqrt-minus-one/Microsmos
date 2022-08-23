/* date = July 13th 2022 1:37 pm */

#ifndef LINUX_SEMAPHORE_H
#define LINUX_SEMAPHORE_H


#define IMPL_CreateSemaphore  Linux_CreateSemaphore
#define IMPL_AcquireSemaphore Linux_AcquireSemaphore
#define IMPL_ReleaseSemaphore  Linux_ReleaseSemaphore


internal void Linux_CreateSemaphore(semaphore_handle *Semaphore, u32 InitialValue, u32 MaxValue);
internal void Linux_AcquireSemaphore(semaphore_handle *Semaphore);
internal void Linux_ReleaseSemaphore(semaphore_handle *Semaphore);


#endif //LINUX_SEMAPHORE_H

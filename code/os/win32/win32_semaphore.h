/* date = July 22nd 2022 9:42 pm */

#ifndef WIN32_SEMAPHORE_H
#define WIN32_SEMAPHORE_H

#define IMPL_CreateSemaphore  W32_CreateSemaphore
#define IMPL_AcquireSemaphore W32_AcquireSemaphore
#define IMPL_ReleaseSemaphore  W32_ReleaseSemaphore


internal void W32_CreateSemaphore(semaphore_handle *Semaphore, u32 InitialValue, u32 MaxValue);
internal void W32_AcquireSemaphore(semaphore_handle *Semaphore);
internal void W32_ReleaseSemaphore(semaphore_handle *Semaphore);

#endif //WIN32_SEMAPHORE_H

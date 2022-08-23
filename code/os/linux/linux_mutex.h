/* date = July 31st 2022 11:18 am */

#ifndef LINUX_MUTEX_H
#define LINUX_MUTEX_H

#define IMPL_CreateMutex  Linux_CreateMutex
#define IMPL_AcquireMutex Linux_AcquireMutex
#define IMPL_ReleaseMutex Linux_ReleaseMutex



internal void Linux_CreateMutex(mutex_handle *Mutex);
internal void Linux_AcquireMutex(mutex_handle *Mutex);
internal void Linux_ReleaseMutex(mutex_handle *Mutex);

#endif //LINUX_MUTEX_H

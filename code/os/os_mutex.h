/* date = July 31st 2022 11:13 am */

#ifndef OS_MUTEX_H
#define OS_MUTEX_H

internal void OS_CreateMutex(mutex_handle *Mutex);
internal void OS_AcquireMutex(mutex_handle *Mutex);
internal void OS_ReleaseMutex(mutex_handle *Mutex);

#endif //OS_MUTEX_H

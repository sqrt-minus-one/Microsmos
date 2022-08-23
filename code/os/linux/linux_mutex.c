
internal void
Linux_CreateMutex(mutex_handle *Mutex)
{
    pthread_mutex_init(Mutex, 0);
}


internal void
Linux_AcquireMutex(mutex_handle *Mutex)
{
    pthread_mutex_lock(Mutex);
}

internal void
Linux_ReleaseMutex(mutex_handle *Mutex)
{
    pthread_mutex_unlock(Mutex);
}

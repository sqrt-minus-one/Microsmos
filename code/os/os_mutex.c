

internal void
OS_CreateMutex(mutex_handle *Mutex)
{
#if defined(IMPL_CreateMutex)
    IMPL_CreateMutex(Mutex);
#endif
}


internal void
OS_AcquireMutex(mutex_handle *Mutex)
{
#if defined(IMPL_AcquireMutex)
    IMPL_AcquireMutex(Mutex);
#endif
}

internal void
OS_ReleaseMutex(mutex_handle *Mutex)
{
#if defined(IMPL_ReleaseMutex)
    IMPL_ReleaseMutex(Mutex);
#endif
}


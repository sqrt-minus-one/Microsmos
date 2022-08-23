
internal void
OS_CreateSemaphore(semaphore_handle *Semaphore, u32 InitialValue, u32 MaxValue)
{
#if defined(IMPL_CreateSemaphore)
    IMPL_CreateSemaphore(Semaphore, InitialValue, MaxValue);
#endif
}


internal void
OS_AcquireSemaphore(semaphore_handle *Semaphore)
{
#if defined(IMPL_AcquireSemaphore)
    IMPL_AcquireSemaphore(Semaphore);
#endif
}

internal void
OS_ReleaseSemaphore(semaphore_handle *Semaphore)
{
#if defined(IMPL_ReleaseSemaphore)
    IMPL_ReleaseSemaphore(Semaphore);
#endif
}


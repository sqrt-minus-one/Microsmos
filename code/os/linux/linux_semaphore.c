
internal void
Linux_CreateSemaphore(semaphore_handle *Semaphore, u32 InitialValue, u32 MaxValue)
{
    sem_init(Semaphore, 0, InitialValue);
}

internal void
Linux_AcquireSemaphore(semaphore_handle *Semaphore)
{
    sem_wait(Semaphore);
}

internal void
Linux_ReleaseSemaphore(semaphore_handle *Semaphore)
{
    sem_post(Semaphore);
}



internal void
W32_CreateSemaphore(semaphore_handle *Semaphore, u32 InitialValue, u32 MaxValue)
{
    *Semaphore = CreateSemaphoreA(0, InitialValue, MaxValue, 0);
}

internal void
W32_AcquireSemaphore(semaphore_handle *Semaphore)
{
    WaitForSingleObject(*Semaphore, INFINITE);
}

internal void
W32_ReleaseSemaphore(semaphore_handle *Semaphore)
{
    ReleaseSemaphore(*Semaphore, 1, 0);
}
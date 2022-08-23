

internal void *
W32_Reserve(u64 Size)
{
    void *Memory = VirtualAlloc(0, Size, MEM_RESERVE, PAGE_NOACCESS);
    return Memory;
}

internal void
W32_Release(void *Memory, u64 Size)
{
    UnusedVariable(Size);
    VirtualFree(Memory, 0, MEM_RELEASE);
}

internal b32
W32_Commit(void *Memory, u64 Size)
{
    VirtualAlloc(Memory, Size, MEM_COMMIT, PAGE_READWRITE);
    return true;
}

internal void
W32_Decommit(void *Memory, u64 Size)
{
    VirtualFree(Memory, Size, MEM_DECOMMIT);
}
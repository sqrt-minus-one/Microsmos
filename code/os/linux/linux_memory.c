
internal void *
Linux_Reserve(u64 Size)
{
    void *Result = mmap(0, Size, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, (off_t)0);
    return Result;
};

internal void 
Linux_Release(void *Memory, u64 Size)
{
    munmap(Memory, Size);
};

internal b32 
Linux_Commit(void *Memory, u64 Size)
{
    b32 Result = (mprotect(Memory, Size, PROT_READ|PROT_WRITE) == 0);
    return Result;
};

internal void 
Linux_Decommit(void *Memory, u64 Size)
{
    mprotect(Memory, Size, PROT_NONE);
    madvise(Memory, Size, MADV_DONTNEED);
};

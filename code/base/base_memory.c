
#if !defined(M_IMPL_Reserve)
#define M_IMPL_Reserve 
//#error M_IMPL_Reserve must be defined to use base memory.
#endif
#if !defined(M_IMPL_Release)
#define M_IMPL_Release 
//#error M_IMPL_Release must be defined to use base memory.
#endif
#if !defined(M_IMPL_Commit)
#define M_IMPL_Commit 
//#error M_IMPL_Commit must be defined to use base memory.
#endif
#if !defined(M_IMPL_Decommit)
#define M_IMPL_Decommit
//#error M_IMPL_Decommit must be defined to use base memory.
#endif

////////////////////////////////
//~ NOTE(fakhri): Arena Functions

internal m_arena *
M_NilArena(void)
{
    return &NilArena;
}

internal b32
M_ArenaIsNil(m_arena *arena)
{
    return arena == 0 || arena == &NilArena;
}

internal m_arena *
M_ArenaAlloc(u64 Capacity)
{
    m_arena *Result = (m_arena *)M_IMPL_Reserve(Capacity);
    M_IMPL_Commit(Result, M_COMMIT_SIZE);
    Result->First     = Result->Last = Result->Next = Result->Prev = Result->Parent = M_NilArena();
    Result->Memory    = (u8*)Result + sizeof(m_arena);
    Result->Capacity  = Capacity;
    Result->Pos       = sizeof(m_arena);
    Result->CommitPos = M_COMMIT_SIZE;
    Result->Align     = 8;
    return Result;
}

internal m_arena *
M_ArenaAllocDefault(void)
{
    return M_ArenaAlloc(Gigabytes(4));
}

internal void
M_ArenaRelease(m_arena *Arena)
{
    for(m_arena *Child = Arena->First, *Next = 0; !M_ArenaIsNil(Child); Child = Next)
    {
        Next = Child->Next;
        M_ArenaRelease(Child);
    }
    M_IMPL_Release(Arena, Arena->Capacity);
}

internal void
M_ArenaPushChild(m_arena *Parent, m_arena *NewChild)
{
    DLLPushBack_NPZ(Parent->First, Parent->Last, NewChild, Next, Prev, M_CheckNilArena, M_SetNilArena);
}

internal void *
M_ArenaPushAligned(m_arena *Arena, u64 Size, u64 Align)
{
    void *Memory = 0;
    Align = ClampBot(Arena->Align, Align);
    
    u64 Pos = Arena->Pos;
    u64 PosAddresss = IntFromPtr(Arena) + Pos;
    u64 AlignedAddress = PosAddresss + Align - 1;
    AlignedAddress -= AlignedAddress % Align;
    
    u64 AlignmentSize = AlignedAddress - PosAddresss;
    if (Pos + AlignmentSize + Size <= Arena->Capacity)
    {
        u8 *MemoryBase = (u8*)Arena;
        Memory = MemoryBase + Pos + AlignmentSize;
        u64 NewPos = Pos + AlignmentSize + Size;
        Arena->Pos = NewPos;
        
        if (NewPos > Arena->CommitPos)
        {
            u64 CommitGrow = NewPos - Arena->CommitPos;
            CommitGrow += M_COMMIT_SIZE - 1;
            CommitGrow -= CommitGrow % M_COMMIT_SIZE;
            M_IMPL_Commit(MemoryBase + Arena->CommitPos, CommitGrow);
            Arena->CommitPos += CommitGrow;
        }
    }
    
    return(Memory);
}

internal void *
M_ArenaPush(m_arena *Arena, u64 Size)
{
    return M_ArenaPushAligned(Arena, Size, Arena->Align);
}

internal void *
M_ArenaPushZero(m_arena *Arena, u64 Size)
{
    void *Memory = M_ArenaPush(Arena, Size);
    MemoryZero(Memory, Size);
    return Memory;
}

internal void
M_ArenaSetPosBack(m_arena *Arena, u64 Pos)
{
    Pos = MAX(Pos, sizeof(*Arena));
    if(Arena->Pos > Pos)
    {
        Arena->Pos = Pos;
        
        u64 DecommitPos = Pos;
        DecommitPos += M_COMMIT_SIZE - 1;
        DecommitPos -= DecommitPos % M_COMMIT_SIZE;
        u64 OverCommitted = Arena->CommitPos - DecommitPos;
        OverCommitted -= OverCommitted % M_COMMIT_SIZE;
        if(DecommitPos > 0 && OverCommitted >= M_DECOMMIT_THRESHOLD)
        {
            M_IMPL_Decommit((u8*)Arena + DecommitPos, OverCommitted);
            Arena->CommitPos -= OverCommitted;
        }
    }
}

internal void
M_ArenaPop(m_arena *Arena, u64 Size)
{
    M_ArenaSetPosBack(Arena, Arena->Pos - Size);
}

internal void
M_ArenaClear(m_arena *Arena)
{
    M_ArenaPop(Arena, Arena->Pos);
}

internal u64
M_ArenaGetPos(m_arena *Arena)
{
    return Arena->Pos;
}

internal void
M_ArenaSetAutoAlign(m_arena *Arena, u64 Align)
{
    Arena->Align = Align;
}
////////////////////////////////
//~ NOTE(fakhri): Temp

internal m_temp
M_BeginTemp(m_arena *Arena)
{
    m_temp result = {Arena, Arena->Pos};
    return(result);
}

internal void
M_EndTemp(m_temp Temp)
{
    M_ArenaSetPosBack(Temp.Arena, Temp.Pos);
}

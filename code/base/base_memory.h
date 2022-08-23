/* date = June 20th 2022 0:25 pm */

#ifndef BASE_MEMORY_H
#define BASE_MEMORY_H

////////////////////////////////
//~ NOTE(fakhri): Limits

#if !defined(M_COMMIT_SIZE)
#define M_COMMIT_SIZE Kilobytes(4)
#endif

#if !defined(M_DECOMMIT_THRESHOLD)
#define M_DECOMMIT_THRESHOLD Kilobytes(64)
#endif

////////////////////////////////
//~ NOTE(fakhri): Arena

typedef struct m_arena m_arena;
struct m_arena
{
    m_arena *First;
    m_arena *Last;
    m_arena *Next;
    m_arena *Prev;
    m_arena *Parent;
    void *Memory;
    u64 CommitPos;
    u64 Capacity;
    u64 Pos;
    u64 Align;
};

////////////////////////////////
//~ NOTE(fakhri): Arena Helpers

typedef struct m_temp m_temp;
struct m_temp
{
    m_arena *Arena;
    u64 Pos;
};

////////////////////////////////
//~ NOTE(fakhri): Globals

read_only global m_arena NilArena =
{
    &NilArena,
    &NilArena,
    &NilArena,
    &NilArena,
    &NilArena,
};

////////////////////////////////
//~ NOTE(fakhri): Arena Functions

#define M_CheckNilArena(p) (M_ArenaIsNil(p))
#define M_SetNilArena(p) ((p) = M_NilArena())
internal m_arena *M_NilArena(void);
internal b32      M_ArenaIsNil(m_arena *arena);
internal m_arena *M_ArenaAlloc(u64 cap);
internal m_arena *M_ArenaAllocDefault(void);
internal void     M_ArenaRelease(m_arena *arena);
internal void     M_ArenaPushChild(m_arena *parent, m_arena *new_child);
internal void *   M_ArenaPushAligned(m_arena *arena, u64 size, u64 align);
internal void *   M_ArenaPush(m_arena *arena, u64 size);
internal void *   M_ArenaPushZero(m_arena *arena, u64 size);
internal void     M_ArenaSetPosBack(m_arena *arena, u64 pos);
internal void     M_ArenaSetAutoAlign(m_arena *arena, u64 align);
internal void     M_ArenaPop(m_arena *arena, u64 size);
internal void     M_ArenaClear(m_arena *arena);
internal u64      M_ArenaGetPos(m_arena *arena);
internal void     M_ArenaSetAutoAlign(m_arena *Arena, u64 Align);

#define PushArray(a,T,c)     (T*)M_ArenaPush((a), sizeof(T)*(c))
#define PushArrayZero(a,T,c) (T*)M_ArenaPushZero((a), sizeof(T)*(c))
#define PushStruct(a,T)          PushArray(a,T,1)
#define PushStructZero(a,T)      PushArrayZero(a,T,1)

////////////////////////////////
//~ NOTE(fakhri): Temp

internal m_temp M_BeginTemp(m_arena *arena);
internal void M_EndTemp(m_temp temp);

#endif //BASE_MEMORY_H

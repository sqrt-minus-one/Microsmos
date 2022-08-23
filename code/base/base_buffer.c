
internal u64
CeilLog2(u64 X)
{
    u64 Log2 = 0;
    // NOTE(fakhri): compute from what list we git the buffer
    while((1ull << Log2) < X) ++Log2;
    return Log2;
}

internal buffer_header *
MakeBuffer(m_arena *Arena, u64 Log2Capacity)
{
    u64 Capacity = (1ull << Log2Capacity);
    buffer_header *Result = M_ArenaPush(Arena, sizeof(buffer_header) + Capacity);
    Result->CeilLog2Capacity = (u32)Log2Capacity;
    Result->UsedBytes = 0;
    Result->Next = 0;
    return Result;
}

internal b32
Buffer_CopyToBuffer(buffer_header *Buffer, string8 Data)
{
    b32 Result = 0;
    if (Data.size <= (1ull << Buffer->CeilLog2Capacity))
    {
        Result = 1;
        MemoryCopy((u8*)Buffer + sizeof(buffer_header), Data.str, Data.size);
        Buffer->UsedBytes = Data.size;
    }
    return Result;
}


internal void
BufferListPushBuffer(buffer_list *BufferList, buffer_header *Buffer)
{
    QueuePush(BufferList->First, BufferList->Last, Buffer);
    BufferList->NodeCount += 1;
}

internal buffer_header *
BufferListPopBuffer(buffer_list *BufferList)
{
    buffer_header *Result = BufferList->First;
    if (BufferList->First)
    {
        QueuePop(BufferList->First, BufferList->Last);
        BufferList->NodeCount -= 1;
    }
    return Result;
}

internal void
BufferPool_PushBuffer(buffer_pool *Pool, buffer_header *Buffer)
{
    if (Buffer)
    {
        Assert(Buffer->CeilLog2Capacity < ArrayCount(Pool->Buffers));
        buffer_list *List = Pool->Buffers + Buffer->CeilLog2Capacity;
        BufferListPushBuffer(List, Buffer);
    }
}

// NOTE(fakhri): returns a buffer of a power of two capacity
// large enough to hold Size bytes
internal buffer_header *
BufferPool_GetBuffer(m_arena *Arena, buffer_pool *Pool, u64 Size)
{
    buffer_header *Result = 0;
    u32 Index = (u32)CeilLog2(Size);
    Assert(Index < ArrayCount(Pool->Buffers));
    if (Index < ArrayCount(Pool->Buffers))
    {
        buffer_list *List = Pool->Buffers + Index;
        
        // NOTE(fakhri): grab a buffer from the chosen list
        Result = BufferListPopBuffer(List);
        if (!Result)
        {
            // NOTE(fakhri): if the list had no buffer then allocate one
            Result = MakeBuffer(Arena, Index);
        }
    }
    return Result;
}

internal string8
String8FromBuffer(buffer_header *Buffer)
{
    string8 Result;
    Result.str = (u8*)Buffer + sizeof(buffer_header);
    Result.size = Buffer->UsedBytes;
    return Result;
}

internal buffer_header *
BufferPool_PushString(m_arena *Arena, buffer_pool *Pool, buffer_header *OldBuffer, string8 String)
{
    BufferPool_PushBuffer(Pool, OldBuffer);
    buffer_header *Buffer = BufferPool_GetBuffer(Arena, Pool, String.size);
    Buffer_CopyToBuffer(Buffer, String);
    return Buffer;
}

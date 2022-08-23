/* date = June 22nd 2022 4:00 pm */

#ifndef BASE_BUFFER_H
#define BASE_BUFFER_H

typedef struct buffer_header buffer_header;
struct buffer_header
{
    buffer_header *Next;
    u32 CeilLog2Capacity; // NOTE(fakhri): index in the buffer_pool 
    u64 UsedBytes;
};

typedef struct buffer_list buffer_list;
struct buffer_list
{
    buffer_header *First;
    buffer_header *Last;
    u32 NodeCount;
};

// NOTE(fakhri): allocate and free power of two sized buffers
typedef struct buffer_pool buffer_pool;
struct buffer_pool
{
    // NOTE(fakhri): buffers in Buffers[i] list has 
    // a capacity of (1 << i)
    buffer_list Buffers[32];
};


internal buffer_header *MakeBuffer(m_arena *Arena, u64 Log2Capacity);
internal b32     Buffer_CopyToBuffer(buffer_header *Buffer, string8 Data);

internal void         BufferPool_PushBuffer(buffer_pool *Pool, buffer_header *Buffer);
internal buffer_header *BufferPool_GetBuffer(m_arena *Arena, buffer_pool *Pool, u64 Size);
internal string8 String8FromBuffer(buffer_header *Buffer);

internal buffer_header *BufferPool_PushString(m_arena *Arena, buffer_pool *Pool, buffer_header *OldBuffer, string8 String);

#endif //BASE_BUFFER_H

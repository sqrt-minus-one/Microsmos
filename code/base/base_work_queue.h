/* date = July 27th 2022 7:13 pm */

#ifndef BASE_WORK_QUEUE_H
#define BASE_WORK_QUEUE_H

typedef u64 worker_id;
typedef void worker_work(void *);
#define WORK_TASK_SIG(Name) void Name(void *Input)

typedef struct work_entry work_entry;
struct work_entry
{
    worker_work *Work;
    void *Data;
};

typedef struct next_work next_work;
struct next_work
{
    work_entry WorkEntry;
    b32 IsValid;
};

struct workers_queue
{
    work_entry Entries[256];
    volatile u64 Head;
    volatile u64 Tail;
};

typedef struct work_queue work_queue;
struct work_queue
{
    work_entry Entries[1024];
    volatile u64 Head;
    
    volatile u64 CommittedTail;
    volatile u64 ClaimedTail;
    
    semaphore_handle AvailableWorkSemaphore;
    volatile i64 AvailableWorkSemaphoreValue;
    
    u32 WorkersCount;
};

struct worker_info
{
    work_queue *WorkQueue;
    worker_id WorkerID;
};

//- NOTE(fakhri): work queue functions
internal void      WorkQueue_SetupQueue(work_queue *WorkQueue, u32 WorkersCount);
internal next_work WorkQueue_GetNextWorkToDo(work_queue *WorkQueue);
internal b32       WorkQueue_ProcessOneEntry(work_queue *WorkQueue);
internal b32       WorkQueue_PushEntrySP(work_queue *WorkQueue, worker_work *Work, void *Data);
internal b32       WorkQueue_PushEntry(work_queue *WorkQueue, worker_work *Work, void *Data);


#endif //BASE_WORK_QUEUE_H

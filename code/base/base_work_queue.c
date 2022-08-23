#include <emmintrin.h>

global work_queue GlobalWorkQueue;

internal void
WorkQueue_SetupQueue(work_queue *WorkQueue, u32 WorkersCount)
{
    OS_CreateSemaphore(&WorkQueue->AvailableWorkSemaphore, 0, WorkersCount);
    
    WorkQueue->WorkersCount = WorkersCount;
}

internal void
SignalWaitingWorkers(work_queue *WorkQueue)
{
    for (;;)
    {
        i64 SemValue = WorkQueue->AvailableWorkSemaphoreValue;
        if (SemValue + 1 < (i64)WorkQueue->WorkersCount)
        {
            if (AtomicCompareAndExchange64(&WorkQueue->AvailableWorkSemaphoreValue, SemValue, SemValue + 1))
            {
                OS_ReleaseSemaphore(&WorkQueue->AvailableWorkSemaphore);
            }
            else
            {
                continue;
            }
        }
        break;
    }
    
}

internal next_work
WorkQueue_GetNextWorkToDo(work_queue *WorkQueue)
{
    next_work Result = ZERO_STRUCT;
    
    u64 OldHead = WorkQueue->Head;
    
    if (WorkQueue->Head != WorkQueue->CommittedTail)
    {
        u64 EntryIndex = OldHead % ArrayCount(WorkQueue->Entries);
        Result.WorkEntry = WorkQueue->Entries[EntryIndex];
        if (AtomicCompareAndExchange64(&WorkQueue->Head, OldHead, OldHead + 1))
        {
            Result.IsValid = true;
        }
    }
    
    return Result;
}

internal b32
WorkQueue_ProcessOneEntry(work_queue *WorkQueue)
{
    b32 Result = false;
    
    next_work NextWorkToDo = WorkQueue_GetNextWorkToDo(WorkQueue);
    if (NextWorkToDo.IsValid)
    {
        Result = true;
        work_entry WorkEntry = NextWorkToDo.WorkEntry;
        WorkEntry.Work(WorkEntry.Data);
    }
    Assert(WorkQueue->Head <= WorkQueue->CommittedTail);
    return Result;
}

internal b32
WorkQueue_PushEntrySP(work_queue *WorkQueue, worker_work *Work, void *Data)
{
    b32 Result = false;
    Assert(Work && Data);
    if ((WorkQueue->CommittedTail - WorkQueue->Head) != ArrayCount(WorkQueue->Entries))
    {
        Result = true;
        u64 EntryIndex = WorkQueue->CommittedTail % ArrayCount(WorkQueue->Entries);
        
        WorkQueue->Entries[EntryIndex].Work = Work;
        WorkQueue->Entries[EntryIndex].Data = Data;
        
        CompletePreviousWritesBeforeFutureWrites();
        // NOTE(fakhri): make the changes visible
        ++ WorkQueue->CommittedTail;
        // NOTE(fakhri): wake up any worker thread that is waiting for a work entry
        SignalWaitingWorkers(WorkQueue);
    }
    
    return Result;
}

internal b32
WorkQueue_PushEntry(work_queue *WorkQueue, worker_work *Work, void *Data)
{
    b32 Result = false;
    
    u64 OurTail = AtomicIncrement64(&WorkQueue->ClaimedTail);
    
    if (OurTail - WorkQueue->Head >= ArrayCount(WorkQueue->Entries))
    {
        // NOTE(fakhri): queue was full
        u64 Expected = OurTail + 1;
        while((OurTail - WorkQueue->Head >= ArrayCount(WorkQueue->Entries)) &&
              (!AtomicCompareAndExchange64(&WorkQueue->ClaimedTail, Expected, OurTail)))
        {
            _mm_pause();
            Expected = OurTail + 1;
        }
    }
    
    if (OurTail - WorkQueue->Head < ArrayCount(WorkQueue->Entries))
    {
        u64 EntryIndex = OurTail % ArrayCount(WorkQueue->Entries);
        
        WorkQueue->Entries[EntryIndex].Work = Work;
        WorkQueue->Entries[EntryIndex].Data = Data;
        
        u64 Expected = OurTail;
        while(!AtomicCompareAndExchange64(&WorkQueue->CommittedTail, Expected, OurTail + 1))
        {
            _mm_pause();
            Expected = OurTail;
        }
        
        Result = true;
        // NOTE(fakhri): wake up any worker thread that is waiting for a work entry
        SignalWaitingWorkers(WorkQueue);
    }
    
    return Result;
}

internal
SYS_THREAD_SIG(WorkerThreadMain)
{
    thread_ctx WorkerThreadContext = MakeThreadContext();
    SetThreadCtx(&WorkerThreadContext);
    
    struct worker_info *WorkerInfo = (struct worker_info *)Input;
    work_queue *WorkQueue = WorkerInfo->WorkQueue;
    for(;;)
    {
        next_work WorkToDo = WorkQueue_GetNextWorkToDo(WorkQueue);
        if (WorkToDo.IsValid)
        {
            WorkToDo.WorkEntry.Work(WorkToDo.WorkEntry.Data);
        }
        else
        {
            // NOTE(fakhri): work queue was empty, call the os
            // to see what should we do about it
            Log("Thread[%d] going to sleep after finding nothing to do", WorkerInfo->WorkerID);
            AtomicDecrement64(&WorkQueue->AvailableWorkSemaphoreValue);
            OS_AcquireSemaphore(&WorkQueue->AvailableWorkSemaphore);
        }
    }
}

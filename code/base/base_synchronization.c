
// NOTE(fakhri): https://en.wikipedia.org/wiki/Readers%E2%80%93writers_problem

//~ NOTE(fakhri): prefer readers

internal void 
Sync_MakeSynchronizationObject(sync_reader_pref *Sync)
{
    OS_CreateMutex(&Sync->ReadersMutex);
    OS_CreateMutex(&Sync->ResourceMutex);
}

internal void
Begin_SyncSection_Read(sync_reader_pref *Sync)
{
    OS_AcquireMutex(&Sync->ReadersMutex);
    ++Sync->ReadersCount;
    if (Sync->ReadersCount == 1)
    {
        // NOTE(fakhri): prevent writers from change the storage
        OS_AcquireMutex(&Sync->ResourceMutex);
    }
    OS_ReleaseMutex(&Sync->ReadersMutex);
}

internal void
End_SyncSection_Read(sync_reader_pref *Sync)
{
    OS_AcquireMutex(&Sync->ReadersMutex);
    --Sync->ReadersCount;
    if (Sync->ReadersCount == 0)
    {
        // NOTE(fakhri): all write operations are done, allow writing again
        OS_ReleaseMutex(&Sync->ResourceMutex);
    }
    OS_ReleaseMutex(&Sync->ReadersMutex);
}

internal void
Begin_SyncSection_Write(sync_reader_pref *Sync)
{
    OS_AcquireMutex(&Sync->ResourceMutex);
}

internal void
End_SyncSection_Write(sync_reader_pref *Sync)
{
    OS_ReleaseMutex(&Sync->ResourceMutex);
}




internal b32
W32_BeginWatchDirectory(directory_watcher *watcher, m_arena *Arena, string8 dir_path)
{
    b32 result = 0;
    
    m_temp Scratch = GetScratch(&Arena, 1);
    string16 path16 = Str16From8(Scratch.Arena, dir_path);
    watcher->DirHandle = CreateFileW((WCHAR*)path16.str,
                                     GENERIC_READ,
                                     FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE,
                                     0, 
                                     OPEN_EXISTING,
                                     FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED ,
                                     0);
    
    
    if (watcher->buffer && watcher->DirHandle != INVALID_HANDLE_VALUE)
    {
        watcher->overlapped = (OVERLAPPED *)M_ArenaPush(Arena, sizeof(OVERLAPPED));
        
        if (ReadDirectoryChangesW(watcher->DirHandle,
                                  watcher->buffer,
                                  ArrayCount(watcher->buffer),
                                  FALSE,
                                  FILE_NOTIFY_CHANGE_LAST_WRITE,
                                  0,
                                  watcher->overlapped,
                                  0))
        {
            // NOTE(fakhri): good
            result = 1;
        }
        else
        {
            Assert(!"ReadDirectoryChangesW failed\n");
        }
    }
    else
    {
        LogError("couldn't open directory %s\n", dir_path.str);
    }
    
    ReleaseScratch(Scratch);
    return result;
}


internal string8
W32_CheckDirectoryChanges(directory_watcher *watcher, m_arena *Arena)
{
    string8 result = ZERO_STRUCT;
    if (HasOverlappedIoCompleted(watcher->overlapped))
    {
        if (GetOverlappedResult(watcher->DirHandle, 
                                watcher->overlapped,
                                &(watcher->bytes_filled),
                                0)
            && (watcher->bytes_filled != 0))
        {
            // NOTE(fakhri): a file has changed
            
            FILE_NOTIFY_INFORMATION *information = (FILE_NOTIFY_INFORMATION *)watcher->buffer;
            result = Str8From16(Arena, Str16C((u16 *)information->FileName));
            // NOTE(fakhri): make sure we only got one file change notification
            Assert(information->NextEntryOffset == 0);
            
            MemoryZero(watcher->buffer, sizeof(watcher->buffer));
            b32 read_result = ReadDirectoryChangesW(watcher->DirHandle,
                                                    watcher->buffer,
                                                    (DWORD)ArrayCount(watcher->buffer),
                                                    FALSE,
                                                    FILE_NOTIFY_CHANGE_LAST_WRITE,
                                                    0,
                                                    watcher->overlapped,
                                                    0);
            Assert(read_result != 0);
        }
        else
        {
            InvalidPath;
        }
    }
    return result;
}

/* date = August 19th 2022 8:28 pm */

#ifndef WIN32_DIRECTORY_WATCHER_H
#define WIN32_DIRECTORY_WATCHER_H

// NOTE(fakhri): directory watcher
typedef struct directory_watcher
{
    HANDLE DirHandle;
    OVERLAPPED *overlapped;
    char buffer[1024];
    DWORD bytes_filled;
} directory_watcher;

internal b32 W32_BeginWatchDirectory(directory_watcher *watcher, m_arena *Arena, string8 dir_path);

internal string8 W32_CheckDirectoryChanges(directory_watcher *watcher, m_arena *Arena);

#endif //WIN32_DIRECTORY_WATCHER_H

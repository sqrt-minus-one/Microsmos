/* date = July 17th 2022 11:03 pm */

#ifndef WIN32_FILES_H
#define WIN32_FILES_H

#define IMPL_MakeDirectory       W32_MakeDirectory
#define IMPL_LoadEntireFile      W32_LoadEntireFile
#define IMPL_SaveToFile          W32_SaveToFile
#define IMPL_AppendToFile        W32_AppendToFile
#define IMPL_DeleteFile          W32_DeleteFile
#define IMPL_FileIterBegin       W32_FileIterBegin
#define IMPL_FileIterNext        W32_FileIterNext
#define IMPL_FileIterEnd         W32_FileIterEnd
#define IMPL_GetSystemPath       W32_GetSystemPath
#define IMPL_DoesFileExist       W32_DoesFileExist
#define IMPL_DoesDirectoryExist  W32_DoesDirectoryExist

typedef struct w32_file_iterator w32_file_iterator;
struct w32_file_iterator
{
    HANDLE State;
    u64 First;
    WIN32_FIND_DATAW FindData;
};

StaticAssert(sizeof(w32_file_iterator) <= sizeof(file_iterator), file_iterator_size_check);

internal b32       W32_MakeDirectory(string8 Path);
internal string8   W32_LoadEntireFile(m_arena *Arena, string8 Filename);
internal b32       W32_SaveToFile(string8 Filename, string8_list stream);
internal b32       W32_AppendToFile(string8 path, string8_list data);
internal void      W32_DeleteFile(string8 Filename);
internal b32       W32_DoesFileExist(string8 Path);
internal b32       W32_DoesDirectoryExist(string8 path);

internal b32       W32_FileIterBegin(file_iterator *Iterator, string8 Path);
internal file_info W32_FileIterNext(m_arena *Arena, file_iterator *Iterator);
internal void      W32_FileIterEnd(file_iterator *Iterator);

internal string8   W32_GetSystemPath(m_arena *Arena, system_path SystemPath);

#endif //WIN32_FILES_H

/* date = June 20th 2022 3:26 pm */

#ifndef OS_FILES_H
#define OS_FILES_H

typedef u32 file_flags;
enum
{
    FileFlag_Directory = (1<<0),
    FileFlag_Valid     = (1<<1),
};

typedef struct file_info file_info;
struct file_info
{
    file_flags Flags;
    string8 Path;
    string8 Name;
    string8 Extension;
    u64 FileSize;
};

typedef struct file_iterator file_iterator;
struct file_iterator
{
    // This is opaque state to store OS-specific file-system iteration data.
    u8 Opaque[Kilobytes(1)];
};


typedef enum system_path system_path;
enum system_path
{
    SystemPath_Null,
    SystemPath_Initial,
    SystemPath_Current,
    SystemPath_Binary,
    SystemPath_AppData,
    SystemPath_COUNT,
};


internal b32       OS_MakeDirectory(string8 Path);
internal string8   OS_LoadEntireFile(m_arena *Arena, string8 Filename);
internal b32       OS_SaveToFile(string8 Filename, string8_list Stream);
internal b32       OS_SaveStringToFile(string8 Filename, string8 String);
internal file_info OS_GetFileInfo(string8 FilePath);
internal b32       OS_MakeEmptyFile(string8 Filename);
internal void      OS_DeleteFile(string8 Filename);
internal b32       OS_DeleteFilesInDir(string8 DirPath);
internal b32       OS_FileIterBegin(file_iterator *Iterator, string8 Path);
internal file_info OS_FileIterNext(m_arena *Arena, file_iterator *Iterator);
internal void      OS_FileIterEnd(file_iterator *Iterator);
internal string8   OS_GetSystemPath(m_arena *Arena, system_path SystemPath);

#endif //OS_FILES_H

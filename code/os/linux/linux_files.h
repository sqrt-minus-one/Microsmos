/* date = June 20th 2022 3:46 pm */

#ifndef LINUX_FILES_H
#define LINUX_FILES_H

#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/mman.h>

#ifndef O_PATH
# define O_PATH                010000000
#endif
#ifndef AT_NO_AUTOMOUNT
# define AT_NO_AUTOMOUNT        0x800
#endif
#ifndef AT_SYMLINK_NOFOLLOW
# define AT_SYMLINK_NOFOLLOW    0x100
#endif

#define IMPL_MakeDirectory  Linux_MakeDirectory
#define IMPL_LoadEntireFile Linux_LoadEntireFile
#define IMPL_SaveToFile     Linux_SaveToFile
#define IMPL_GetFileInfo    Linux_GetFileInfo
#define IMPL_DeleteFile     Linux_DeleteFile
#define IMPL_FileIterBegin  Linux_FileIterBegin
#define IMPL_FileIterNext   Linux_FileIterNext
#define IMPL_FileIterEnd    Linux_FileIterEnd
#define IMPL_GetSystemPath  Linux_GetSystemPath


typedef struct linux_file_iterator linux_file_iterator;
struct linux_file_iterator
{
    int DirFD;
    DIR *Directory;
};

StaticAssert(sizeof(linux_file_iterator) <= sizeof(file_iterator), file_iterator_size_check);

internal b32       Linux_MakeDirectory(string8 Path);
internal string8   Linux_LoadEntireFile(m_arena *Arena, string8 Filename);
internal b32       Linux_SaveToFile(string8 Filename, string8_list stream);
internal file_info Linux_GetFileInfo(string8 Filename);
internal void      Linux_DeleteFile(string8 Filename);
internal b32       Linux_FileIterBegin(file_iterator *Iterator, string8 Path);
internal file_info Linux_FileIterNext(m_arena *Arena, file_iterator *Iterator);
internal void      Linux_FileIterEnd(file_iterator *Iterator);
internal string8   Linux_GetSystemPath(m_arena *Arena, system_path SystemPath);

#endif //LINUX_FILES_H


internal void
W32_ReadWholeBlock(HANDLE file, void *data, u64 data_len)
{
    u8 *ptr = (u8*)data;
    u8 *opl = ptr + data_len;
    for (;;){
        u64 unread = (u64)(opl - ptr);
        DWORD to_read = (DWORD)(ClampTop(unread, u32Max));
        DWORD did_read = 0;
        if (!ReadFile(file, ptr, to_read, &did_read, 0))
        {
            break;
        }
        ptr += did_read;
        if (ptr >= opl)
        {
            break;
        }
    }
}

internal void
W32_WriteWholeBlock(HANDLE file, string8_list data)
{
    for(string8_node *node = data.First; node != 0; node = node->Next)
    {
        u8 *ptr = node->String.str;
        u8 *opl = ptr + node->String.size;
        for(;;)
        {
            u64 unwritten = (u64)(opl - ptr);
            DWORD to_write = (DWORD)(ClampTop(unwritten, u32Max));
            DWORD did_write = 0;
            if(!WriteFile(file, ptr, to_write, &did_write, 0))
            {
                goto fail_out;
            }
            ptr += did_write;
            if(ptr >= opl)
            {
                break;
            }
        }
    }
    fail_out:;
}

internal b32
W32_MakeDirectory(string8 Path)
{
    m_temp Scratch = GetScratch(0, 0);
    string16 Path16 = Str16From8(Scratch.Arena, Path);
    b32 Result = 1;
    if(!CreateDirectoryW((WCHAR *)Path16.str, 0))
    {
        Result = 0;
    }
    ReleaseScratch(Scratch);
    return Result;
}

internal string8
W32_LoadEntireFile(m_arena *Arena, string8 Path)
{
    string8 Result = {0};
    
    DWORD DesiredAccess = GENERIC_READ | GENERIC_WRITE;
    DWORD ShareMode = 0;
    SECURITY_ATTRIBUTES SecurityAttributes = {
        (DWORD)sizeof(SECURITY_ATTRIBUTES),
        0,
        0,
    };
    DWORD CreationDisposition = OPEN_EXISTING;
    DWORD FlagsAndAttributes = 0;
    HANDLE TemplateFile = 0;
    
    m_temp Scratch = GetScratch(&Arena, 1);
    string16 Path16 = Str16From8(Scratch.Arena, Path);
    HANDLE File = CreateFileW((WCHAR*)Path16.str,
                              DesiredAccess,
                              ShareMode,
                              &SecurityAttributes,
                              CreationDisposition,
                              FlagsAndAttributes,
                              TemplateFile);
    
    if(File != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER SizeInt;
        if(GetFileSizeEx(File, &SizeInt) && SizeInt.QuadPart > 0)
        {
            u64 Size = SizeInt.QuadPart;
            void *Data = PushArray(Arena, u8, Size + 1);
            W32_ReadWholeBlock(File, Data, Size);
            ((u8*)Data)[Size] = 0;
            Result = Str8((u8 *)Data, Size);
        }
        else
        {
            Result.str = PushArrayZero(Arena, u8, 1);
            Result.size = 0;
        }
        CloseHandle(File);
    }
    else
    {
        LogError("CreateFile Failed With error %d", GetLastError());
    }
    
    ReleaseScratch(Scratch);
    return(Result);
}


internal b32
W32_SaveToFile(string8 path, string8_list data)
{
    b32 result = 0;
    HANDLE file = {0};
    {
        DWORD desired_access = GENERIC_READ | GENERIC_WRITE;
        DWORD share_mode = 0;
        SECURITY_ATTRIBUTES security_attributes = { (DWORD)sizeof(SECURITY_ATTRIBUTES) };
        DWORD creation_disposition = CREATE_ALWAYS;
        DWORD flags_and_attributes = 0;
        HANDLE template_file = 0;
        m_temp Scratch = GetScratch(0, 0);
        string16 path16 = Str16From8(Scratch.Arena, path);
        if((file = CreateFileW((WCHAR*)path16.str,
                               desired_access,
                               share_mode,
                               &security_attributes,
                               creation_disposition,
                               flags_and_attributes,
                               template_file)) != INVALID_HANDLE_VALUE)
        {
            result = 1;
            W32_WriteWholeBlock(file, data);
            CloseHandle(file);
        }
        ReleaseScratch(Scratch);
    }
    return result;
}


internal b32
W32_AppendToFile(string8 path, string8_list data)
{
    b32 result = 0;
    HANDLE file = {0};
    {
        DWORD desired_access = FILE_APPEND_DATA;
        DWORD share_mode = 0;
        SECURITY_ATTRIBUTES security_attributes = {
            (DWORD)sizeof(SECURITY_ATTRIBUTES),
            0,
            0,
        };
        DWORD creation_disposition = OPEN_ALWAYS;
        DWORD flags_and_attributes = 0;
        HANDLE template_file = 0;
        m_temp Scratch = GetScratch(0, 0);
        string16 path16 = Str16From8(Scratch.Arena, path);
        if((file = CreateFileW((WCHAR*)path16.str,
                               desired_access,
                               share_mode,
                               &security_attributes,
                               creation_disposition,
                               flags_and_attributes,
                               template_file)) != INVALID_HANDLE_VALUE)
        {
            result = 1;
            SetFilePointer(file, 0, 0, FILE_END);
            W32_WriteWholeBlock(file, data);
            
            CloseHandle(file);
        }
        else
        {
            LogError("File I/O Error", "Could not save to \"%s\"", path);
        }
        ReleaseScratch(Scratch);
    }
    return result;
}

internal void
W32_DeleteFile(string8 Filename)
{
    m_temp Scratch = GetScratch(0, 0);
    string16 path16 = Str16From8(Scratch.Arena, Filename);
    DeleteFileW((WCHAR*)path16.str);
    ReleaseScratch(Scratch);
}

internal b32
W32_DoesFileExist(string8 Path)
{
    m_temp Scratch = GetScratch(0, 0);
    string8 path_copy = PushStr8Copy(Scratch.Arena, Path);
    b32 found = (GetFileAttributesA((char*)path_copy.str) != INVALID_FILE_ATTRIBUTES);
    ReleaseScratch(Scratch);
    return found;
}

internal b32
W32_DoesDirectoryExist(string8 path)
{
    m_temp Scratch = GetScratch(0, 0);
    string8 path_copy = PushStr8Copy(Scratch.Arena, path);
    DWORD file_attributes = GetFileAttributesA((char*)(path_copy.str));
    b32 found = (file_attributes != INVALID_FILE_ATTRIBUTES &&
                 !!(file_attributes & FILE_ATTRIBUTE_DIRECTORY));
    
    ReleaseScratch(Scratch);
    
    return found;
}

internal b32
W32_FileIterBegin(file_iterator *Iterator, string8 Path)
{
    m_temp Scratch = GetScratch(0, 0);
    
    u8 c = Path.str[Path.size - 1];
    b32 NeedStar = (c == '/' || c == '\\');
    string8 cPath = NeedStar ? PushStr8F(Scratch.Arena, "%.*s*", Str8Expand(Path)) : Path;
    string16 cPath16 = Str16From8(Scratch.Arena, cPath);
    
    WIN32_FIND_DATAW FindData = ZERO_STRUCT;
    HANDLE State = FindFirstFileW((WCHAR*)cPath16.str, &FindData);
    
    ReleaseScratch(Scratch);
    
    //- fill results
    b32 Result = !!State;
    if (Result)
    {
        w32_file_iterator *W32Iterator = (w32_file_iterator*)Iterator; 
        W32Iterator->State = State;
        W32Iterator->First = 1;
        MemoryCopy(&W32Iterator->FindData, &FindData, sizeof(FindData));
    }
    return(Result);
}

internal file_info
W32_FileIterNext(m_arena *Arena, file_iterator *Iterator)
{
    //- get low-level file info for this step
    b32 Good = 0;
    
    w32_file_iterator *W32Iterator = (w32_file_iterator *)Iterator; 
    WIN32_FIND_DATAW *FindData = &W32Iterator->FindData;
    if (W32Iterator->First)
    {
        W32Iterator->First = 0;
        Good = 1;
    }
    else
    {
        Good = FindNextFileW(W32Iterator->State, FindData);
    }
    
    //- convert to FileInfo
    file_info Result = {0};
    if (Good)
    {
        if (FindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            Result.Flags |= FileFlag_Directory;
        }
        u16 *FilenameBase = (u16*)FindData->cFileName;
        u16 *ptr = FilenameBase;
        for (;*ptr != 0; ptr += 1);
        string16 Filename16 = {0};
        Filename16.str = FilenameBase;
        Filename16.size = (u64)(ptr - FilenameBase);
        Result.Name = Str8From16(Arena, Filename16);
        Result.FileSize = ((((u64)FindData->nFileSizeHigh) << 32) |
                           ((u64)FindData->nFileSizeLow));
    }
    return(Result);
}

internal void
W32_FileIterEnd(file_iterator *Iterator)
{
    w32_file_iterator *W32Iterator = (w32_file_iterator *)Iterator;
    FindClose(W32Iterator->State);
}

#include "Shlobj.h"

internal string8
W32_GetSystemPath(m_arena *Arena, system_path SystemPath)
{
    m_temp Scratch = GetScratch(&Arena, 1);
    string8 Result = {0};
    
    switch (SystemPath)
    {
        case SystemPath_Current:
        {
            DWORD length = GetCurrentDirectoryW(0, 0);
            u16 *memory = PushArrayZero(Scratch.Arena, u16, length + 1);
            length = GetCurrentDirectoryW(length + 1, (WCHAR*)memory);
            Result = Str8From16(Arena, Str16(memory, length));
        } break;
        
        case SystemPath_Binary:
        {
            local_persist b32 first = 1;
            local_persist string8 name = {0};
            if (first)
            {
                first = 0;
                u64 size = Kilobytes(32);
                u16 *Buffer = PushArrayZero(Scratch.Arena, u16, size);
                DWORD length = GetModuleFileNameW(0, (WCHAR*)Buffer, (DWORD)size);
                name = Str8From16(Scratch.Arena, Str16(Buffer, length));
                name = Str8ChopLastSlash(name);
                u8 *buffer8 = PushArrayZero(Arena, u8, name.size);
                MemoryCopy(buffer8, name.str, name.size);
                name.str = buffer8;
            }
            Result = name;
        } break;
        
        case SystemPath_AppData:
        {
            local_persist b32 first = 1;
            local_persist string8 name = {0};
            if (first)
            {
                first = 0;
                u64 size = Kilobytes(32);
                u16 *Buffer = PushArrayZero(Scratch.Arena, u16, size);
                if (SUCCEEDED(SHGetFolderPathW(0, CSIDL_APPDATA, 0, 0, (WCHAR*)Buffer)))
                {
                    name = Str8From16(Scratch.Arena, Str16C(Buffer));
                    u8 *buffer8 = PushArrayZero(Arena, u8, name.size);
                    MemoryCopy(buffer8, name.str, name.size);
                    name.str = buffer8;
                }
            }
            Result = name;
        } break;
    }
    
    ReleaseScratch(Scratch);
    
    return Result;
}

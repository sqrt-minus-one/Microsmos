
internal b32
OS_MakeDirectory(string8 Path)
{
    b32 Result = 0;
#if defined(IMPL_MakeDirectory)
    Result = IMPL_MakeDirectory(Path);
#endif
    return Result;
}

internal string8
OS_LoadEntireFile(m_arena *Arena, string8 Filename)
{
    string8 Result = ZERO_STRUCT;
#if defined(IMPL_LoadEntireFile)
    Result = IMPL_LoadEntireFile(Arena, Filename);
#endif
    return Result;
}

internal b32
OS_SaveToFile(string8 Filename, string8_list Stream)
{
    b32 Result = 0;
#if defined(IMPL_SaveToFile)
    Result = IMPL_SaveToFile(Filename, Stream);
#endif
    return Result;
}

internal b32
OS_SaveStringToFile(string8 Filename, string8 String)
{
    string8_list Stream = ZERO_STRUCT;
    m_temp Scratch = GetScratch(0, 0);
    Str8ListPush(Scratch.Arena, &Stream, String);
    b32 Result = OS_SaveToFile(Filename, Stream);
    ReleaseScratch(Scratch);
    return Result;
}

internal b32
OS_MakeEmptyFile(string8 Filename)
{
    b32 Result = OS_SaveToFile(Filename, (string8_list)ZERO_STRUCT);
    return Result;
}

internal file_info
OS_GetFileInfo(string8 FilePath)
{
    file_info Result = ZERO_STRUCT;
#if defined(IMPL_GetFileInfo)
    Result = IMPL_GetFileInfo(FilePath);
#endif
    return Result;
}

internal void
OS_DeleteFile(string8 Filename)
{
#if defined(IMPL_DeleteFile)
    IMPL_DeleteFile(Filename);
#endif
}

internal b32
OS_FileIterBegin(file_iterator *Iterator, string8 Path)
{
    b32 Result = 0;
#if defined(IMPL_FileIterBegin)
    Result = IMPL_FileIterBegin(Iterator, Path);
#endif
    return Result;
}

internal file_info
OS_FileIterNext(m_arena *Arena, file_iterator *Iterator)
{
#if defined(IMPL_FileIterNext)
    return IMPL_FileIterNext(Arena, Iterator);
#endif
}

internal void
OS_FileIterEnd(file_iterator *Iterator)
{
#if defined(IMPL_FileIterEnd)
    IMPL_FileIterEnd(Iterator);
#endif
}

internal string8
OS_GetSystemPath(m_arena *Arena, system_path SystemPath)
{
    string8 Result = ZERO_STRUCT;
#if defined(IMPL_GetSystemPath)
    Result = IMPL_GetSystemPath(Arena, SystemPath);
#endif
    return Result;
}

internal b32
OS_DeleteFilesInDir(string8 DirPath)
{
    b32 Result = false;
    file_iterator Iterator;
    if (OS_FileIterBegin(&Iterator, DirPath))
    {
        Result = true;
        b32 ShouldStop = false;
        for(;!ShouldStop;)
        {
            m_temp Scratch = GetScratch(0, 0);
            
            file_info FileInfo = OS_FileIterNext(Scratch.Arena, &Iterator);
            if (HasFlag(FileInfo.Flags, FileFlag_Valid))
            {
                if (!HasFlag(FileInfo.Flags, FileFlag_Directory))
                {
                    string8 Path = PushStr8F(Scratch.Arena, "%.*s/%.*s", Str8Expand(DirPath), Str8Expand(FileInfo.Name));
                    OS_DeleteFile(Path);
                }
            }
            else
            {
                OS_FileIterEnd(&Iterator);
                ShouldStop = true;
            }
            ReleaseScratch(Scratch);
        }
    }
    return Result;
}
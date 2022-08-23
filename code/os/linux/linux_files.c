
internal b32
Linux_MakeDirectory(string8 Path)
{
    b32 Result = (mkdir(Path.cstr, 0777) == 0);
    return Result;
}

internal string8
Linux_LoadEntireFile(m_arena *Arena, string8 Filename)
{
    m_temp Scratch = GetScratch(&Arena, 1);
    string8 FileContents = ZERO_STRUCT;
    string8 FilenameCopy = PushStr8Copy(Scratch.Arena, Filename);
    FILE *File = fopen((char*)FilenameCopy.str, "rb");
    if(File != 0)
    {
        fseek(File, 0, SEEK_END);
        u64 FileSize = ftell(File);
        fseek(File, 0, SEEK_SET);
        FileContents.str = PushArray(Arena, u8, FileSize+1);
        if(FileContents.str)
        {
            FileContents.size = FileSize;
            Assert(fread(FileContents.str, 1, FileSize, File) == FileSize);
            FileContents.str[FileContents.size] = 0;
        }
        fclose(File);
    }
    ReleaseScratch(Scratch);
    return FileContents;
}

internal b32
Linux_SaveToFile(string8 Filename, string8_list Stream)
{
    b32 Result = 0;
    int File = open(Filename.cstr, O_TRUNC|O_WRONLY|O_CREAT, 0666);
    
    if (File != -1)
    {
        for(string8_node *Node = Stream.First; Node != 0; Node = Node->Next)
        {
            u8 *Ptr = Node->String.str;
            u8 *OnePastLast = Ptr + Node->String.size;
            for(;;)
            {
                u64 Unwritten = (u64)(OnePastLast - Ptr);
                u32 ToWrite = (u32)(ClampTop(Unwritten, u32Max));
                i32 DidWrite = 0;
                DidWrite = write(File, Ptr, ToWrite);
                if (DidWrite == -1)
                {
                    goto fail_out;
                }
                
                Ptr += DidWrite;
                if(Ptr >= OnePastLast)
                {
                    break;
                }
            }
        }
        Result = 1;
        close(File);
        fail_out:;
    }
    
    return Result;
}

internal file_info
Linux_GetFileInfo(string8 FilePath)
{
    file_info Result = ZERO_STRUCT;
    m_temp Scratch = GetScratch(0, 0);
    string8 FilenameCopy;
    {
        FilenameCopy.cstr = PushArray(Scratch.Arena, char, FilePath.size + 1);
        FilenameCopy.size = FilePath.size;
        MemoryCopy(FilenameCopy.cstr, FilePath.cstr, FilePath.size);
        FilenameCopy.cstr[FilenameCopy.size] = 0;
    }
    int File = open(FilenameCopy.cstr, O_RDONLY, 0666);
    if (File != -1)
    {
        struct stat FileStat;
        if (fstat(File, &FileStat) == 0){
            SetFlag(Result.Flags, FileFlag_Valid);
            Result.Path = FilePath;
            Result.FileSize = FileStat.st_size;
            if (HasFlag(FileStat.st_mode, S_IFDIR))
            {
                SetFlag(Result.Flags, FileFlag_Directory);
                if (FilePath.str[FilePath.size - 1] == '/')
                {
                    --FilePath.size;
                }
                string8 ParentDir = Str8SkipLastSlash(FilePath);
                Result.Name = Str8SkipFirst(FilePath, ParentDir.size);
            }
            else
            {
                string8 FileName = Str8SkipLastSlash(FilePath);
                Result.Extension = Str8SkipLastPeriod(FileName);
                Result.Name = Str8ChopLastPeriod(FileName);
            }
        }
        close(File);
    }
    ReleaseScratch(Scratch);
    return Result;
}


internal void
Linux_DeleteFile(string8 Filename)
{
    file_info FileInfo = Linux_GetFileInfo(Filename);
    if (HasFlag(FileInfo.Flags, FileFlag_Valid))
    {
        if (HasFlag(FileInfo.Flags, FileFlag_Directory))
        {
            rmdir(Filename.cstr);
        }
        else
        {
            unlink(Filename.cstr); 
        }
    }
}

internal b32
Linux_FileIterBegin(file_iterator *OpaqueIterator, string8 Path)
{
    b32 Result = 0;
    if (Path.cstr)
    {
        linux_file_iterator *Iterator = (linux_file_iterator *)OpaqueIterator;
        Iterator->Directory = opendir(Path.cstr);
        Iterator->DirFD = open(Path.cstr, O_PATH|O_CLOEXEC);
        if (Iterator->Directory != 0 && Iterator->DirFD != -1)
        {
            Result = 1;
        }
    }
    return Result;
}

internal file_info
Linux_FileIterNext(m_arena *Arena, file_iterator *OpaqueIterator)
{
    linux_file_iterator *Iterator = (linux_file_iterator *)OpaqueIterator;
    file_info Result = ZERO_STRUCT;
    
    struct dirent *DirectoryEntry = readdir(Iterator->Directory);
    
    if(DirectoryEntry)
    {
        Result.Name = PushStr8F(Arena, "%s", DirectoryEntry->d_name);
        Result.Flags = 0;
        
        struct stat Stat; 
        if(fstatat(Iterator->DirFD, DirectoryEntry->d_name, &Stat, AT_NO_AUTOMOUNT|AT_SYMLINK_NOFOLLOW) == 0)
        {
            if((Stat.st_mode & S_IFMT) == S_IFDIR)
            {
                SetFlag(Result.Flags, FileFlag_Directory);
            }
            else
            {
                Result.Extension = Str8SkipLastPeriod(Result.Name);
            }
            Result.FileSize = Stat.st_size;
        }
        SetFlag(Result.Flags, FileFlag_Valid);
    }
    
    return Result;
}

internal void
Linux_FileIterEnd(file_iterator *OpaqueIterator)
{
    linux_file_iterator *Iterator = (linux_file_iterator *)OpaqueIterator;
    closedir(Iterator->Directory);
    close(Iterator->DirFD);
}

internal string8
Linux_GetSystemPath(m_arena *Arena, system_path SystemPath)
{
    string8 Result = ZERO_STRUCT;
    
    switch(SystemPath)
    {
        case SystemPath_Null:
        {
            // nothign
        } break;
        case SystemPath_Current:
        {
            char *WorkingDir = getcwd(0 ,0);
            Result = PushStr8F(Arena, "%s/", WorkingDir);
            free(WorkingDir);
        } break;
        case SystemPath_Binary:
        {
            local_persist b32 First = true;
            local_persist string8 Name = ZERO_STRUCT;
            if (First)
            {
                First = false;
                // linux-specific: binary path symlinked at /proc/self/exe
                // PATH_MAX is probably good enough...
                // read the 'readlink' manpage for some comedy about it being 'broken by design'.
                
                char *Buffer = PushArray(Arena, char, PATH_MAX);
                isize Length = readlink("/proc/self/exe", Buffer, PATH_MAX);
                
                if(Length == -1) {
                    perror("readlink");
                    *Buffer = Length = 0;
                }
                Name = Str8ChopLastSlash(Str8C(Buffer));
            }
            Result = Name;
        } break;
        case SystemPath_AppData:
        {
            local_persist b32 First = true;
            local_persist string8 Name = ZERO_STRUCT;
            if (First)
            {
                First = false;
                char *home_cstr = getenv("HOME");
                if (home_cstr)
                {
                    Name = PushStr8F(Arena, "%s/.%s/", home_cstr, APP_NAME);
                }
            }
            Result = Name;
        } break;
        default: NotImplemented; break;
    }
    
    return Result;
}
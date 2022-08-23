
internal b32
Compare_u64(u64 A, u64 B, comparison Comparison)
{
    b32 Result = 0;
    switch(Comparison)
    {
        default: break;
        case comparison_EqualTo:               Result = (A == B); break;
        case comparison_NotEqualTo:            Result = (A != B); break;
        case comparison_LessThan:              Result = (A <  B); break;
        case comparison_LessThanOrEqualTo:     Result = (A <= B); break;
        case comparison_GreaterThan:           Result = (A >  B); break;
        case comparison_GreaterThanOrEqualTo:  Result = (A >= B); break;
    }
    return Result;
}

internal u32
RoundR64U32(r64 Real)
{
    Assert(Real > 0);
    u32 Result = (u32)(Real + 0.5);
    return Result;
}

internal void
_DebugLog(i32 flags, char *File, int line, char *format, ...)
{
    // NOTE(fakhri): Log to stdout
    {
        char *name = "Info";
        if(flags & Log_Error)
        {
            name = "Error";
        }
        else if(flags & Log_Warning)
        {
            name = "Warning";
        }
        
        m_temp Scratch = GetScratch(0, 0);
        va_list args;
        va_start(args, format);
        string8 FormatedLog = PushStr8FV(Scratch.Arena, format, args);
        va_end(args);
        ReleaseScratch(Scratch);
        
        os_data_time DateTime = OS_GetCurrentDateTime();
        
        fprintf(stdout, "%s ([%d/%s/%d:%dh:%dm:%ds(GMT)]:%s:%i) %s\n", name, DateTime.Day, MonthAbbrevdName[DateTime.Month].cstr, DateTime.Year, DateTime.Hour, DateTime.Minute, DateTime.Second, File, line, FormatedLog.cstr);
    }
    
#if OS_WINDOWS
    // NOTE(fakhri): Log to VS output, etc.
    {
        local_persist char string[4096] = {0};
        va_list args;
        va_start(args, format);
        stbsp_vsnprintf(string, sizeof(string), format, args);
        va_end(args);
        OutputDebugStringA(string);
        OutputDebugStringA("\n");
    }
#endif
    
}

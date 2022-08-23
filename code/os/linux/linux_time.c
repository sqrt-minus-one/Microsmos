
internal os_data_time
Linux_GetCurrentDateTime()
{
    os_time Time = Linux_GetCurrentTime();
    os_data_time Result = Linux_GetDateTimeFromTime(Time);
    return Result;
}

internal os_time
Linux_GetCurrentTime()
{
    time_t CurrentTime = time(0);
    os_time Result = (os_time) CurrentTime;
    return Result;
}

internal os_data_time
Linux_GetDateTimeFromTime(os_time Time)
{
    time_t LinuxTime = (time_t)Time;
    struct tm *TM_Time = gmtime(&LinuxTime);
    os_data_time Result = ZERO_STRUCT;
    Result.Year       = TM_Time->tm_year + 1900;
    Result.Month      = (os_month)TM_Time->tm_mon;
    Result.Day        = TM_Time->tm_mday;
    Result.Hour       = TM_Time->tm_hour;
    Result.Minute     = TM_Time->tm_min;
    Result.Second     = TM_Time->tm_sec;
    Result.MiliSecond = 0;
    return Result;
}

internal os_time
Linux_GetTimeFromDateTime(os_data_time DateTime)
{
    struct tm TM_Time = ZERO_STRUCT;
    
    TM_Time.tm_year = DateTime.Year - 1900;
    TM_Time.tm_mon  = DateTime.Month;
    TM_Time.tm_mday = DateTime.Day;
    TM_Time.tm_hour = DateTime.Hour;
    TM_Time.tm_min  = DateTime.Minute;
    TM_Time.tm_sec  = DateTime.Second;
    
    os_time Result = (os_time)timegm(&TM_Time);
    return Result;
}

internal os_time
Linux_GetCurrentTimeMicroSeconds()
{
    struct timespec TimeSpec;
    timespec_get(&TimeSpec, TIME_UTC);
    os_time Result = TimeSpec.tv_nsec/Thousand(1) + Million(1) * TimeSpec.tv_sec;
    return Result;
}
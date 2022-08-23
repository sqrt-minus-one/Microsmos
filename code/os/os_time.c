
internal os_data_time
OS_GetCurrentDateTime()
{
    os_data_time Result = ZERO_STRUCT;
#if defined(IMPL_GetCurrentDateTime)
    Result = IMPL_GetCurrentDateTime();
#else
    NotImplemented;
#endif
    return Result;
}

internal os_time
OS_GetCurrentTime()
{
    os_time Result = ZERO_STRUCT;
#if defined(IMPL_GetCurrentTime)
    Result = IMPL_GetCurrentTime();
#else
    NotImplemented;
#endif
    return Result;
}

internal os_data_time
OS_GetDateTimeFromTime(os_time Time)
{
    os_data_time Result = ZERO_STRUCT;
#if defined(IMPL_GetDateTimeFromTime)
    Result = IMPL_GetDateTimeFromTime(Time);
#else
    NotImplemented;
#endif
    return Result;
}

internal os_time
OS_GetTimeFromDateTime(os_data_time DateTime)
{
    os_time Result = ZERO_STRUCT;
#if defined(IMPL_GetTimeFromDateTime)
    Result = IMPL_GetTimeFromDateTime(DateTime);
#else
    NotImplemented;
#endif
    return Result;
}

internal os_time
OS_GetCurrentTimeMicroSeconds()
{
    os_time Result = ZERO_STRUCT;
#if defined(IMPL_GetCurrentTimeMicroSeconds)
    Result = IMPL_GetCurrentTimeMicroSeconds();
#else
    NotImplemented;
#endif
    return Result;
}
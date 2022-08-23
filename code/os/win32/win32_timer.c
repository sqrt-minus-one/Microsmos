
internal b32  
W32_TimerInit(w32_timer *Timer)
{
    b32 result = 0;
    
    if(QueryPerformanceFrequency(&Timer->CountsPerSeconds))
    {
        result = 1;
    }
    
    Timer->IsSleepGranular = (timeBeginPeriod(1) == TIMERR_NOERROR);
    
    return result;
}

internal void 
W32_TimerBeginFrame(w32_timer *Timer)
{
    QueryPerformanceCounter(&Timer->BeginFrame);
}

internal void 
W32_TimerEndFrame(os_state *OS_State, w32_timer *Timer)
{
    LARGE_INTEGER end_frame;
    QueryPerformanceCounter(&end_frame);
    i64 frame_counts = end_frame.QuadPart - Timer->BeginFrame.QuadPart;
    OS_State->WallDeltaTimeForFrame = frame_counts  / (r32)Timer->CountsPerSeconds.QuadPart;
    OS_State->WallTime += OS_State->WallDeltaTimeForFrame;
}

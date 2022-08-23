
internal void
OS_EndFrame(os_state *OS_State)
{
    ++OS_State->FrameIndex;
    OS_State->GameTime += OS_State->GameDeltaTimeForFrame;
    OS_EatAllEvents(&OS_State->Events);
}

internal m_arena *
OS_FrameArena(os_state *OS_State)
{
    u64 ArenaIndex = OS_State->FrameIndex % ArrayCount(OS_State->FrameArenas);
    m_arena *Result = OS_State->FrameArenas[ArenaIndex];
    return Result;
}

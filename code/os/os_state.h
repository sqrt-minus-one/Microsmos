/* date = August 19th 2022 0:50 pm */

#ifndef OS_STATE_H
#define OS_STATE_H

typedef struct
{
    // NOTE(fakhri): Memory
    m_arena *PermanentArena;
    m_arena *FrameArenas[2];
    
    // NOTE(fakhri): Options
    volatile b32 Quit;
    b32 Vsync;
    b32 Fullscreen;
    v2i32 WindowSize;
    u32 FrameIndex;
    
    r32 TargetFramePerSeconds;
    
    r32 GameTime;
    r32 WallTime;
    r32 GameDeltaTimeForFrame;
    r32 WallDeltaTimeForFrame;
    
    // NOTE(fakhri): Event List and Input Data
    v2r32 MousePosition;
    os_event_list Events;
} os_state;



internal void OS_EndFrame(os_state *OS_State);
internal m_arena * OS_FrameArena(os_state *OS_State);

#endif //OS_STATE_H

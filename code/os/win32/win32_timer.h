/* date = August 19th 2022 2:43 pm */

#ifndef WIN32_TIMER_H
#define WIN32_TIMER_H

typedef struct
{
    LARGE_INTEGER CountsPerSeconds;
    LARGE_INTEGER BeginFrame;
    b32 IsSleepGranular;
} w32_timer;


internal b32  W32_TimerInit(w32_timer *Timer);
internal void W32_TimerBeginFrame(w32_timer *Timer);
internal void W32_TimerEndFrame(os_state *OS_State, w32_timer *Timer);

#endif //WIN32_TIMER_H


//- NOTE(fakhri): Library Functions
internal HMODULE
W32_LibraryOpen(string8 path)
{
    m_temp Scratch = GetScratch(0, 0);
    string16 path16 = Str16From8(Scratch.Arena, path);
    HMODULE hmodule = LoadLibraryW((WCHAR*)path16.str);
    ReleaseScratch(Scratch);
    return hmodule;
}

internal void
W32_LibraryClose(HMODULE hmodule)
{
    FreeLibrary(hmodule);
}

internal void_function *
W32_LibraryLoadFunction(HMODULE hmodule, string8 name)
{
    m_temp Scratch = GetScratch(0, 0);
    string8 name_copy = PushStr8Copy(Scratch.Arena, name);
    void_function *result = (void_function *)GetProcAddress(hmodule, (char *)name_copy.str);
    ReleaseScratch(Scratch);
    return result;
}

// NOTE(fakhri): fullscreen
internal void
W32_ToggleFullscreen(HWND hwnd)
{
    local_persist WINDOWPLACEMENT last_window_placement = {
        sizeof(last_window_placement)
    };
    
    DWORD window_style = GetWindowLong(hwnd, GWL_STYLE);
    if(window_style & WS_OVERLAPPEDWINDOW)
    {
        MONITORINFO monitor_info = { sizeof(monitor_info) };
        if(GetWindowPlacement(hwnd, &last_window_placement) &&
           GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY),
                          &monitor_info))
        {
            
            SetWindowLong(hwnd, GWL_STYLE,
                          window_style & ~WS_OVERLAPPEDWINDOW);
            
            SetWindowPos(hwnd, HWND_TOP,
                         monitor_info.rcMonitor.left,
                         monitor_info.rcMonitor.top,
                         monitor_info.rcMonitor.right -
                         monitor_info.rcMonitor.left,
                         monitor_info.rcMonitor.bottom -
                         monitor_info.rcMonitor.top,
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    }
    else
    {
        SetWindowLong(hwnd, GWL_STYLE,
                      window_style | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(hwnd, &last_window_placement);
        SetWindowPos(hwnd, 0, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}


internal os_modiferes
W32_GetModifiers(void)
{
    os_modiferes modifiers = 0;
    if(GetKeyState(VK_CONTROL) & 0x8000)
    {
        modifiers |= OS_Modifier_Ctrl;
    }
    if(GetKeyState(VK_SHIFT) & 0x8000)
    {
        modifiers |= OS_Modifier_Shift;
    }
    if(GetKeyState(VK_MENU) & 0x8000)
    {
        modifiers |= OS_Modifier_Alt;
    }
    return modifiers;
}

//- NOTE(fakhri): file helpers
internal FILETIME
W32_GetLastWriteTime(char *filename)
{
    FILETIME last_write_time = {0};
    WIN32_FIND_DATA find_data;
    HANDLE find_handle = FindFirstFileA(filename, &find_data);
    if(find_handle != INVALID_HANDLE_VALUE)
    {
        FindClose(find_handle);
        last_write_time = find_data.ftLastWriteTime;
    }
    return last_write_time;
}

internal v2r32
W32_GetMousePosition(HWND window)
{
    v2r32 result = ZERO_STRUCT;
    POINT mouse;
    GetCursorPos(&mouse);
    ScreenToClient(window, &mouse);
    result.X = (r32)(mouse.x);
    result.Y = (r32)(mouse.y);
    return result;
}

internal r32
W32_GetTime(os_state *OS_State, w32_timer *Timer)
{
    LARGE_INTEGER curr_time;
    QueryPerformanceCounter(&curr_time);
    r32 Result = OS_State->WallTime + (r32)(curr_time.QuadPart - Timer->BeginFrame.QuadPart) / (r32)Timer->CountsPerSeconds.QuadPart;
    return Result;
}

internal u64
W32_GetCycles(void)
{
    u64 result = __rdtsc();
    return result;
}

